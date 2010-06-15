#ifndef GDBSERVERTCP_H_
#define GDBSERVERTCP_H_
#include <set>

#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>

#include "GdbServerNone.h"

#include "utils.h"

#include "Parameters.h"

/// debug level
#define GDBSERVERTCP_DEBUG_LEVEL 0

/// GDB server with TCP connection
struct GdbServerTcp:GdbServerNone
{
    /** Constructor.
     * This function is called directly from the CpuBase constructor
     * @param[in] parameters Command line parameters
     * @param[in] config Parameters of the current block (and sub-blocks)
     */
    GdbServerTcp(Parameters& parameters, MSP& config)
    : GdbServerNone(parameters, config)
    , port(12345)
    , state(DS_IDLE)
    , run(false)
    {
        do
        {
            Parameter* parameter;
            MSP* gdbserver_config;
            // check if there are specific gdbserver parameters
            if (config.count("gdbserver") != 1)
            {
                break;
            }
            parameter = config["gdbserver"];
            gdbserver_config = parameter->get_config();
            if (gdbserver_config->count("port") == 1)
            {
                this->port = (*gdbserver_config)["port"]->get_int();
            }
        } while (false);
    }

    /// Start the GDB server
    virtual void start()
    {
        struct sockaddr_in sockaddr;
        int fd, val, ret;

        // by default, no remote debugger connected
        this->clientfd = -1;

        // open the server socket
        if ((this->serverfd = socket(PF_INET, SOCK_STREAM, 0)) < -1)
        {
            SYS_ERR("gdbservertcp", "Can not open server socket %d", this->serverfd);
        }

        // allow fast reuse
        val = 1;
        setsockopt(this->serverfd, SOL_SOCKET, SO_REUSEADDR, (char *)&val, sizeof(val));

        // set the server port and address
        sockaddr.sin_family = AF_INET;
        sockaddr.sin_port = htons(this->port);
        sockaddr.sin_addr.s_addr = 0;
        ret = bind(this->serverfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
        if (ret < 0)
        {
            SYS_ERR("gdbservertcp", "Can not bind server socket %d", ret);
        }

        ret = listen(fd, 0);
        if (ret < 0)
        {
            SYS_ERR("gdbservertcp", "Can not listen on server socket %d", ret);
        }

        // if non blocking mode was requested, set the socket non blocking
        if (this->gdb_wait == false)
        {
            // by default configure the server socket in non blocking
            fcntl(this->serverfd, F_SETFL, O_NONBLOCK);
        }
    }

    /** Tell the remote GDB that the process has exited
     * @param code Exit code
     */
    virtual void
    end(int code)
    {
        char buf[4];

        snprintf(buf, sizeof(buf), "W%02x", code);
        this->put_packet(buf);
    }

    /** Debugger call right after instruction fetched and before decoded
     * This function enables the debugger to check for a breakpoint reached, or for an
     * incoming connection request.
     * @return True if the programs should resume normally, false if the just fetched instruction
     * should not be executed
     */
    virtual bool
    after_ins_fetch(void)
    {
        // if no client already accepted
        if (this->clientfd == -1)
        {
            struct sockaddr_in sockaddr;
            socklen_t len;
            int val;

            // if the connection is configured as blocking, print message
            if (this->gdb_wait)
            {
                std::cout << "GDB Server TCP: waiting for connection on port " << this->port << std::endl;
            }

            len = sizeof(sockaddr);
            this->clientfd = accept(this->serverfd, (struct sockaddr *)&sockaddr, &len);
            if (this->clientfd < 0)
            {
                return true;
            }

            // set short latency
            val = 1;
            setsockopt(this->clientfd, IPPROTO_TCP, TCP_NODELAY, (char *)&val, sizeof(val));

            fcntl(this->clientfd, F_SETFL, O_NONBLOCK);

            // this is a first connection -> handle messages from debugger
            this->handlesig(0);

            return true;
        }
        else
        {
            // client already accepted
            return true;
        }

        return true;
    }


private:
    enum DecodeState {
        DS_IDLE,
        DS_GETLINE,
        DS_CHKSUM1,
        DS_CHKSUM2,
    };

    /** Convert an hex digit into its scalar value
     * @param[in] v Hex digit
     * @return The scalar value of the digit
     */
    int
    fromhex(int v)
    {
        if (v >= '0' && v <= '9')
            return v - '0';
        else if (v >= 'A' && v <= 'F')
            return v - 'A' + 10;
        else if (v >= 'a' && v <= 'f')
            return v - 'a' + 10;
        else
            return 0;
    }

    /** Convert a scalar value into an hex digit
     * @param[in] v Scalar value
     * @return The equivalent hex digit
     */
    int
    tohex(int v)
    {
        if (v < 10)
            return v + '0';
        else
            return v - 10 + 'a';
    }

    /** Convert an entire buffer into the equivalent hex representation
     * @param[in, out] buf The buffer to fill with hex representation
     * @param[in] mem The memory buffer to convert
     * @param[in] len The length of the memory buffer to convert
     */
    void
    memtohex(char *buf, const char *mem, int len)
    {
        int i, c;
        char *q;
        q = buf;
        for(i = 0; i < len; i++) {
            c = mem[i];
            *q++ = tohex(c >> 4);
            *q++ = tohex(c & 0xf);
        }
        *q = '\0';
    }

    /** Convert an entire memory hex representation into the memory
     * @param[in, out] mem The memory to fill after conversion
     * @param[in] buf The memory hex representation
     * @param[in] len The length of the memory buffer to fill
     */
    void
    hextomem(char *mem, const char *buf, int len)
    {
        int i;

        for(i = 0; i < len; i++) {
            mem[i] = (fromhex(buf[0]) << 4) | fromhex(buf[1]);
            buf += 2;
        }
    }


    /** Get a single character from the remote GDB
     * @return The received char or -1 if error
     */
    int
    get_char()
    {
        char ch;
        int ret;

        for(;;) {
            ret = recv(this->clientfd, &ch, 1, 0);
            if (ret < 0) {
                if (errno != EINTR && errno != EAGAIN)
                    return -1;
            } else if (ret == 0) {
                return -1;
            } else {
                break;
            }
        }
        return ch;
    }

    /** Handle the memory read access request from the remote debugger
     * @param[in] addr Memory address
     * @param[in, out] buf Buffer
     * @param[in] len Length of the access
     * @return False if successful, True otherwise
     */
    bool
    gdb_rd_mem(uint32_t addr, char* buf, uint32_t len)
    {
        int num_bytes;

        // read the bytes
        num_bytes = this->callbacks.gdb_rd_mem_cb(this->callbacks.obj, addr, (uint8_t*)buf, len);

        if (num_bytes == -1)
        {
            return true;
        }
        // sanity check
        assert(num_bytes == (int)len);
        return false;
    }

    /** Handle the memory write access request from the remote debugger
     * @param[in] addr Memory address
     * @param[in] buf Buffer
     * @param[in] len Length of the access
     * @return False if successful, True otherwise
     */
    bool
    gdb_wr_mem(uint32_t addr, const char* buf, uint32_t len)
    {
        int num_bytes;

        // write the bytes
        num_bytes = this->callbacks.gdb_wr_mem_cb(this->callbacks.obj, addr, (uint8_t*)buf, len);

        if (num_bytes == -1)
        {
            return true;
        }
        // sanity check
        assert(num_bytes == (int)len);
        return false;
    }

    /** Insert an instruction breakpoint
     * @param[in] addr Break address
     * @return False if successful, true otherwise
     */
    bool
    gdb_breakpoint_insert(uint64_t addr)
    {
        this->brkpts.insert(addr);
        return false;
    }

    /** Remove an instruction breakpoint
     * @param[in] addr Break address
     */
    void
    gdb_breakpoint_remove(uint64_t addr)
    {
        this->brkpts.erase(addr);
    }

    /** Insert a memory watchpoint
     * @param[in] addr Watch address
     * @return False if successful, true otherwise
     */
    bool
    gdb_watchpoint_insert(uint64_t addr)
    {
        this->wtcpts.insert(addr);
        return false;
    }

    /** Remove a memory watchpoint
     * @param[in] addr Watch address
     */
    void
    gdb_watchpoint_remove(uint64_t addr)
    {
        this->wtcpts.erase(addr);
    }

    /** Handle a correctly received serial packet
     * @return The decoding state in which the main state machine should move
     */
    DecodeState
    handle_packet()
    {
        char *p;
        int ch, reg_size, type;
        char buf[4096];
        char mem_buf[4096];
        uint32_t *registers;
        uint32_t addr, len;

    #ifdef DEBUG_GDB
        printf("command='%s'\n", line_buf);
    #endif
        p = this->line_buf;
        ch = *p++;
        switch(ch) {
        case '?':
            snprintf(buf, sizeof(buf), "S%02x", SIGTRAP);
            this->put_packet(buf);
            break;
        case 'c':
            if (*p != '\0') {
                addr = strtoull(p, (char **)&p, 16);
                this->callbacks.gdb_set_pc_cb(this->callbacks.obj, addr);
            }
            this->run = true;
            return DS_IDLE;
        case 's':
            if (*p != '\0') {
                addr = strtoull(p, (char **)&p, 16);
                this->callbacks.gdb_set_pc_cb(this->callbacks.obj, addr);
            }
            this->set_singlestep(true);
            this->run = true;
            return DS_IDLE;
        case 'g':
            reg_size = this->callbacks.gdb_rd_reg_cb(this->callbacks.obj, (uint8_t*)mem_buf);
            memtohex(buf, mem_buf, reg_size);
            this->put_packet(buf);
            break;
        case 'G':
            registers = (uint32_t *)mem_buf;
            len = strlen(p) / 2;
            hextomem((char*)registers, p, len);
            this->callbacks.gdb_wr_reg_cb(this->callbacks.obj, (uint8_t*)mem_buf, len);
            this->put_packet("OK");
            break;
        case 'm':
            addr = strtoull(p, (char **)&p, 16);
            if (*p == ',')
                p++;
            len = strtoull(p, NULL, 16);

            // check if the memory access is supported
            if (this->gdb_rd_mem(addr, mem_buf, len)) {
                this->put_packet("E14");
            } else {
                memtohex(buf, mem_buf, len);
                this->put_packet(buf);
            }
            break;
        case 'M':
            addr = strtoull(p, (char **)&p, 16);
            if (*p == ',')
                p++;
            len = strtoull(p, (char **)&p, 16);
            if (*p == ':')
                p++;
            hextomem(mem_buf, p, len);
            if (this->gdb_wr_mem(addr, mem_buf, len))
                this->put_packet("E14");
            else
                this->put_packet("OK");
            break;
        case 'Z':
            type = strtoul(p, (char **)&p, 16);
            if (*p == ',')
                p++;
            addr = strtoull(p, (char **)&p, 16);
            if (*p == ',')
                p++;
            len = strtoull(p, (char **)&p, 16);
            if (type == 0 || type == 1) {
                if (this->gdb_breakpoint_insert(addr))
                    goto breakpoint_error;
                this->put_packet("OK");
            } else if (type == 2) {
                if (this->gdb_watchpoint_insert(addr))
                    goto breakpoint_error;
                this->put_packet("OK");
            } else {
                breakpoint_error:
                this->put_packet("E22");
            }
            break;
        case 'z':
            type = strtoul(p, (char **)&p, 16);
            if (*p == ',')
                p++;
            addr = strtoull(p, (char **)&p, 16);
            if (*p == ',')
                p++;
            len = strtoull(p, (char **)&p, 16);
            if (type == 0 || type == 1) {
                this->gdb_breakpoint_remove(addr);
                this->put_packet("OK");
            } else if (type == 2) {
                this->gdb_watchpoint_remove(addr);
                this->put_packet("OK");
            } else {
                goto breakpoint_error;
            }
            break;
        default:
            // unknown_command:
            /* put empty packet */
            buf[0] = '\0';
            this->put_packet(buf);
            break;
        }
        return DS_IDLE;
    }

    /** Handle received bytes on by one
     * @param[in] ch Received char
     */
    void
    read_byte(int ch)
    {
        int i, csum;
        char reply;

        switch(this->state) {
        case DS_IDLE:
            // detect the beginning of the packet-data part
            if (ch == '$') {
                this->line_buf_index = 0;
                this->state = DS_GETLINE;
            }
            break;
        case DS_GETLINE:
            if (ch == '#') {
                // end of the packet-data
                this->line_buf[this->line_buf_index] = '\0';
                // beginning of the checksum part
                this->state = DS_CHKSUM1;
            } else if (this->line_buf_index >= (sizeof(this->line_buf) - 1)) {
                // received packet-data longer than buffer size
                this->state = DS_IDLE;
            } else {
                // save the char
                this->line_buf[this->line_buf_index++] = ch;
            }
            break;
        case DS_CHKSUM1:
            // save checksum first byte
            this->line_csum = fromhex(ch) << 4;
            this->state = DS_CHKSUM2;
            break;
        case DS_CHKSUM2:
            // save checksum second byte
            this->line_csum |= fromhex(ch);
            // compute packet-data checksum
            csum = 0;
            for(i = 0; i < (int)this->line_buf_index; i++) {
                csum += this->line_buf[i];
            }
            // compare received csum and computed one
            if (this->line_csum != (csum & 0xff)) {
                // CRC not correct
                reply = '-';
                this->put_buffer(&reply, 1);
                this->state = DS_IDLE;
            } else {
                // CRC correct
                reply = '+';
                this->put_buffer(&reply, 1);
                // handle packet
                this->state = this->handle_packet();
            }
            break;
        default:
            abort();
        }
    }

    /** Send a formatted buffer to the GDB connection
     * @param[in] GDB serial formatted string
     * @param[in] len Length of the formatted string
     */
    void
    put_buffer(const char *buf, int len)
    {
        int ret;

        while (len > 0) {
            ret = send(this->clientfd, buf, len, 0);
            if (ret < 0) {
                if (errno != EINTR && errno != EAGAIN)
                    return;
            } else {
                buf += ret;
                len -= ret;
            }
        }
    }

    /** Send a packet to the remote GDB
     * @param[in] buf GDB serial command
     * @return -1 if error, 0 if OK
     */
    int
    put_packet(const char *buf)
    {
        int len, csum, i;
        char *p;

        #if GDBSERVERTCP_DEBUG_LEVEL
        printf("reply='%s'\n", buf);
        #endif

        for(;;) {
            p = last_packet;
            *(p++) = '$';
            len = strlen(buf);
            memcpy(p, buf, len);
            p += len;
            csum = 0;
            for(i = 0; i < len; i++) {
                csum += buf[i];
            }
            *(p++) = '#';
            *(p++) = tohex((csum >> 4) & 0xf);
            *(p++) = tohex((csum) & 0xf);

            last_packet_len = p - last_packet;
            this->put_buffer(last_packet, last_packet_len);

            i = this->get_char();
            if (i < 0)
                return -1;
            if (i == '+')
                break;
        }
        return 0;
    }

    /** Handle a signal from the ISS
     * This function sends the signaling packet to the remote GDB.
     * @param sig Signal id to send to the remote GDB (0 means stop until continue from gdb)
     * @return 0 if OK, different than 0 if error
     */
    int
    handlesig(int sig)
    {
        char buf[256];
        int n;

        // disable single step if it was enabled
        this->set_singlestep(false);

        // check if there is a signal to send
        if (sig != 0)
        {
            snprintf(buf, sizeof(buf), "S%02x", sig);
            this->put_packet(buf);
        }

        sig = 0;
        this->state = DS_IDLE;
        this->run = false;
        while (!this->run) {
            // read a chunk of bytes at once
            n = read(this->clientfd, buf, sizeof(buf));
            if (n > 0)
            {
                int i;
                for (i = 0; i < n; i++)
                    this->read_byte(buf[i]);
            }
            else if (n == 0 || errno != EAGAIN)
            {
                /// connection closed -> wait for a new connection
                this->clientfd = -1;
                return sig;
            }
        }
        return sig;
    }

    /// Port number on which the socket server should wait
    int port;
    /// Server socket connection file descriptor
    int serverfd;
    /// Client socket connection file descriptor
    int clientfd;
    /// Current serial decode state
    enum DecodeState state;
    /// Current run state (False, means can not run)
    bool run;
    /// Buffer used to format the buffer to send
    char last_packet[4100];
    /// Used length of the buffer
    int last_packet_len;
    /// Received packet-data
    char line_buf[4096];
    /// Received packet-data length
    uint32_t line_buf_index;
    /// Received checksum
    int line_csum;
    /// List of breakpoints
    std::set<uint64_t> brkpts;
    /// List of watchpoints
    std::set<uint64_t> wtcpts;
};

#endif /*GDBSERVERTCP_H_*/

