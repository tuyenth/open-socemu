#ifndef GDBSERVERTCP_H_
#define GDBSERVERTCP_H_
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
    virtual void end(int code)
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
        return true;
    }

    /** Handle a signal from the ISS
     * This function sends the signaling packet to the remote GDB.
     * @param sig Signal id to send to the remote GDB (0 means stop until continue from gdb)
     * @return 0 if OK, different than 0 if error
     */
    virtual int handlesig(int sig)
    {
        return 0;
    }

private:
    /** Convert an hex digit into its scalar value
     * @param[in] v Hex digit
     * @return The scalar value of the digit
     */
    int fromhex(int v)
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
    int tohex(int v)
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
    void memtohex(char *buf, const char *mem, int len)
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
    void hextomem(char *mem, const char *buf, int len)
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
    int get_char()
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

    /** Send a formatted buffer to the GDB connection
     * @param[in] GDB serial formatted string
     * @param[in] len Length of the formatted string
     */
    void put_buffer(const char *buf, int len)
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
    int put_packet(const char *buf)
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

    /// Port number on which the socket server should wait
    int port;
    /// Server socket connection file descriptor
    int serverfd;
    /// Client socket connection file descriptor
    int clientfd;
    /// Buffer used to format the buffer to send
    char last_packet[4100];
    /// Used length of the buffer
    int last_packet_len;
};

#endif /*GDBSERVERTCP_H_*/

