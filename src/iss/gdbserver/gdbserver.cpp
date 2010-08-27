/** @file gdbserver.cpp
 * @brief GDB server class implementation
 *
 * This file contains the GDB server class implementation.
 *
 * @sa http://sourceware.org/gdb/current/onlinedocs/gdb_toc.html For information
 * about the GDB packet format.  Look at annexes.
 */

#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <assert.h>

#include "gdbserver.h"


// LCARON : used to make it compatible with our system
#define target_ulong unsigned long int

#define TARGET_FMT_lx "%08lX"
#define PRIx64 "016llX"

//#define DEBUG_GDB

enum {
    GDB_SYS_UNKNOWN,
    GDB_SYS_ENABLED,
    GDB_SYS_DISABLED,
} gdb_syscall_mode;

static inline int fromhex(int v)
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

static inline int tohex(int v)
{
    if (v < 10)
        return v + '0';
    else
        return v - 10 + 'a';
}

static void memtohex(char *buf, const uint8_t *mem, int len)
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

static void hextomem(uint8_t *mem, const char *buf, int len)
{
    int i;

    for(i = 0; i < len; i++) {
        mem[i] = (fromhex(buf[0]) << 4) | fromhex(buf[1]);
        buf += 2;
    }
}

bool gdbserver::use_gdb_syscalls(void)
{
    if (gdb_syscall_mode == GDB_SYS_UNKNOWN) {
        gdb_syscall_mode =  GDB_SYS_ENABLED;
    }
    return gdb_syscall_mode == GDB_SYS_ENABLED;
}

int gdbserver::get_char()
{
    uint8_t ch;
    int ret;

    for(;;) {
        ret = recv(m_clientfd, &ch, 1, 0);
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

void gdbserver::put_buffer(const uint8_t *buf, int len)
{
    int ret;

    while (len > 0) {
        ret = send(m_clientfd, buf, len, 0);
        if (ret < 0) {
            if (errno != EINTR && errno != EAGAIN)
                return;
        } else {
            buf += ret;
            len -= ret;
        }
    }
}

/* return -1 if error, 0 if OK */
int gdbserver::put_packet(char *buf)
{
    int len, csum, i;
    uint8_t *p;

#ifdef DEBUG_GDB
    printf("reply='%s'\n", buf);
#endif

    for(;;) {
        p = m_last_packet;
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

        m_last_packet_len = p - m_last_packet;
        this->put_buffer((uint8_t *)m_last_packet, m_last_packet_len);

        i = this->get_char();
        if (i < 0)
            return -1;
        if (i == '+')
            break;
    }
    return 0;
}

bool gdbserver::cpu_gdb_rw_memory(uint32_t addr, uint8_t* buf, uint32_t len, uint8_t rw)
{
    int num_bytes;

    if (rw == 0)
    {
        // read the bytes
        num_bytes = gdb_read(addr, buf, len);

        if (num_bytes == -1)
        {
            return true;
        }
        // sanity check
        assert (num_bytes == (int)len);
        return false;
    }
    else
    {
        // write the bytes
        num_bytes = gdb_write(addr, buf, len);

        if (num_bytes == -1)
        {
            return true;
        }
        // sanity check
        assert (num_bytes == (int)len);
        return false;
    }
}


gdbserver::RSState gdbserver::handle_packet(const char *line_buf)
{
    const char *p;
    int ch, reg_size, type;
    char buf[4096];
    uint8_t mem_buf[4096];
    uint32_t *registers;
    target_ulong addr, len;

#ifdef DEBUG_GDB
    printf("command='%s'\n", line_buf);
#endif
    p = line_buf;
    ch = *p++;
    switch(ch) {
    case '?':
        snprintf(buf, sizeof(buf), "S%02x", SIGTRAP);
        this->put_packet(buf);
        break;
    case 'c':
        if (*p != '\0') {
            addr = strtoull(p, (char **)&p, 16);
            gdb_set_pc(addr);
        }
        m_running_state = 1;
        return RS_IDLE;
    case 's':
        if (*p != '\0') {
            addr = strtoull(p, (char **)&p, 16);
            gdb_set_pc(addr);
        }
        gdb_single_step(1);
        m_running_state = 1;
	return RS_IDLE;
    case 'F':
        {
            target_ulong ret;
            target_ulong err;

            ret = strtoull(p, (char **)&p, 16);
            if (*p == ',') {
                p++;
                err = strtoull(p, (char **)&p, 16);
            } else {
                err = 0;
            }
            if (*p == ',')
                p++;
            type = *p;
            if (m_current_syscall_cb)
                m_current_syscall_cb(ret, err);
            if (type == 'C') {
                this->put_packet("T02");
            } else {
                m_running_state = 1;
            }
        }
        break;
    case 'g':
        reg_size = gdb_read_registers(mem_buf);
        memtohex(buf, mem_buf, reg_size);
        this->put_packet(buf);
        break;
    case 'G':
        registers = (uint32_t *)mem_buf;
        len = strlen(p) / 2;
        hextomem((uint8_t *)registers, p, len);
        gdb_write_registers(mem_buf, len);
        this->put_packet("OK");
        break;
    case 'm':
        addr = strtoull(p, (char **)&p, 16);
        if (*p == ',')
            p++;
        len = strtoull(p, NULL, 16);

        if (cpu_gdb_rw_memory(addr, mem_buf, len, 0) != 0) {
            this->put_packet ("E14");
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
        if (cpu_gdb_rw_memory(addr, mem_buf, len, 1) != 0)
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
            if (gdb_breakpoint_insert(addr) < 0)
                goto breakpoint_error;
            this->put_packet("OK");
        } else if (type == 2) {
            if (gdb_watchpoint_insert(addr) < 0)
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
            gdb_breakpoint_remove(addr);
            this->put_packet("OK");
        } else if (type == 2) {
            gdb_watchpoint_remove(addr);
            this->put_packet("OK");
        } else {
            goto breakpoint_error;
        }
        break;
    default:
        //        unknown_command:
        /* put empty packet */
        buf[0] = '\0';
        this->put_packet(buf);
        break;
    }
    return RS_IDLE;
}


void gdbserver::do_syscall(syscall_complete_cb cb, char *fmt, ...)
{
    va_list va;
    char buf[256];
    char *p;
    target_ulong addr;
    uint64_t i64;

    m_current_syscall_cb = cb;
    m_state = RS_SYSCALL;
    m_state = RS_IDLE;
    va_start(va, fmt);
    p = buf;
    *(p++) = 'F';
    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt++) {
            case 'x':
                addr = va_arg(va, target_ulong);
                p += sprintf(p, TARGET_FMT_lx, addr);
                break;
            case 'l':
                if (*(fmt++) != 'x')
                    goto bad_format;
                i64 = va_arg(va, uint64_t);
                p += sprintf(p, "%" PRIx64, i64);
                break;
            case 's':
                addr = va_arg(va, target_ulong);
                p += sprintf(p, TARGET_FMT_lx "/%x", addr, va_arg(va, int));
                break;
            default:
            bad_format:
                fprintf(stderr, "gdbstub: Bad syscall format string '%s'\n",
                        fmt - 1);
                break;
            }
        } else {
            *(p++) = *(fmt++);
        }
    }
    *p = 0;
    va_end(va);
    this->put_packet(buf);
    this->handlesig(0);
}

void gdbserver::read_byte(int ch)
{
    int i, csum;
    uint8_t reply;

    switch(m_state) {
    case RS_IDLE:
        if (ch == '$') {
            m_line_buf_index = 0;
            m_state = RS_GETLINE;
        }
        break;
    case RS_GETLINE:
        if (ch == '#') {
            m_state = RS_CHKSUM1;
        } else if (m_line_buf_index >= (int)(sizeof(m_line_buf) - 1)) {
            m_state = RS_IDLE;
        } else {
            m_line_buf[m_line_buf_index++] = ch;
        }
        break;
    case RS_CHKSUM1:
        m_line_buf[m_line_buf_index] = '\0';
        m_line_csum = fromhex(ch) << 4;
        m_state = RS_CHKSUM2;
        break;
    case RS_CHKSUM2:
        m_line_csum |= fromhex(ch);
        csum = 0;
        for(i = 0; i < m_line_buf_index; i++) {
            csum += m_line_buf[i];
        }
        if (m_line_csum != (csum & 0xff)) {
            reply = '-';
            this->put_buffer(&reply, 1);
            m_state = RS_IDLE;
        } else {
            reply = '+';
            this->put_buffer(&reply, 1);
            m_state = this->handle_packet(m_line_buf);
        }
        break;
    default:
        abort();
    }
}

int gdbserver::handlesig(int sig)
{
    char buf[256];
    int n;

    if (m_serverfd < 0)
        return sig;

    /* disable single step if it was enabled */
    gdb_single_step(0);

    if (sig != 0)
    {
        snprintf(buf, sizeof(buf), "S%02x", sig);
        this->put_packet(buf);
    }

    sig = 0;
    m_state = RS_IDLE;
    m_running_state = 0;
    while (m_running_state == 0) {
        n = read(m_clientfd, buf, 256);
        if (n > 0)
        {
            int i;

            for (i = 0; i < n; i++)
                this->read_byte(buf[i]);
        }
        else if (n == 0 || errno != EAGAIN)
        {
            /* XXX: Connection closed.  Should probably wait for another
             connection before continuing.  */
            return sig;
        }
    }
    return sig;
}

bool gdbserver::checkctrlc(void)
{
    char buf[256];
    int n;

    n = read(m_clientfd, buf, 256);
    // check if ctrl-c was received
    if ((n == 1) && buf[0] == 3)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool gdbserver::checkremote(bool forced)
{
    // if the gdbserver is disabled and not enforced by caller
    if (!this->m_enabled && !forced)
        return false;

    // if no client already accepted
    if (this->m_clientfd == -1)
    {
        struct sockaddr_in sockaddr;
        socklen_t len;
        int val;

        // if the connection is configured as blocking, print message
        if (this->m_blocking)
        {
            std::cout << "GDB Server TCP: waiting for connection on port " << this->port << std::endl;
        }

        len = sizeof(sockaddr);
        this->m_clientfd = accept(this->m_serverfd, (struct sockaddr *)&sockaddr, &len);
        if (this->m_clientfd < 0)
        {
            return false;
        }

        /* set short latency */
        val = 1;
        setsockopt(this->m_clientfd, IPPROTO_TCP, TCP_NODELAY, (char *)&val, sizeof(val));

        fcntl(this->m_clientfd, F_SETFL, O_NONBLOCK);

        // this is a first connection -> handle messages from debugger
        handlesig(0);

        return true;
    }
    else
    {
        // client already accepted
        return true;
    }
}
int gdbserver::open()
{
    struct sockaddr_in sockaddr;
    int fd, val, ret;

    fd = socket(PF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        perror("socket");
        return -1;
    }

    // allow fast reuse
    val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&val, sizeof(val));

    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(this->port);
    sockaddr.sin_addr.s_addr = 0;
    ret = bind(fd, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
    if (ret < 0)
    {
        perror("bind");
        return -1;
    }
    ret = listen(fd, 0);
    if (ret < 0)
    {
        perror("listen");
        return -1;
    }
    return fd;
}

void gdbserver::start(int port, bool enabled, bool blocking)
{
    // init the member variables the information
    this->port = port;
    this->m_enabled = enabled;
    this->m_blocking = blocking;
    // by default, no remote debugger connected
    this->m_clientfd = -1;

    // open the server port
    this->m_serverfd = this->open();

    // sanity check: server socket could not be opened
    assert(this->m_serverfd >= 0);

    // if non blocking mode was requested, set the socket non blocking
    if (!this->m_blocking)
    {
        // by default configure the server socket in non blocking
        fcntl(this->m_serverfd, F_SETFL, O_NONBLOCK);
    }
}

void gdbserver::end(int code)
{
    char buf[4];

    if (m_serverfd < 0)
    {
        return;
    }

    snprintf(buf, sizeof(buf), "W%02x", code);
    put_packet(buf);
}


