#ifndef UTILS_H_
#define UTILS_H_

// necessary define for processes in simple_target_socket
#define SC_INCLUDE_DYNAMIC_PROCESSES

// obvious inclusion
#include "systemc"

/// Macro to configure debug the utils tools (could be configured for a single file)
#ifndef UTILS_DEBUG_LEVEL
    #define UTILS_DEBUG_LEVEL 0
#endif

/// Macro to get a register field value in a register r with mask m and at bit position p
/// @param[in] __r register value
/// @param[in] __m field mask BEFORE shifting
/// @param[in] __p position of the lowest bit of the field
/// @return the value of the register masked and shifted
#define GETF(__r, __m, __p)                                                 \
    (( (__r) & (__m) ) >> (__p))

/// Macro to set a value v in a register field at bit position p in register r with mask m
/// @param[in] __r register value
/// @param[in] __v value to put in field
/// @param[in] __m field mask BEFORE shifting
/// @param[in] __p position of the lowest bit of the field
#define SETF(__r, __v, __m, __p)                                            \
    do {                                                                    \
        assert( ( ( ( (__v) << (__p) ) & ( ~(__m) ) ) ) == 0 );             \
        __r &= ~(__m);                                                      \
        __r |= (__v) << (__p);                                              \
    } while (0)

/// Macro to print error and exit
/// @param[in] format print format
#define TLM_ERR(format, ...)                                                \
do {                                                                        \
    char __mystring[1024];                                                  \
    sprintf(__mystring, "%s: " format, this->name(), __VA_ARGS__);          \
    std::cerr<<__mystring <<" at "<<sc_core::sc_time_stamp()<<std::endl;    \
    exit(-1);                                                               \
} while (0)

/// Macro to print error and exit
/// @param[in] name name to print
/// @param[in] format print format
#define SYS_ERR(name, format, ...)                                          \
do {                                                                        \
    char __mystring[1024];                                                  \
    sprintf(__mystring, "%s: " format, name, __VA_ARGS__);                  \
    std::cerr<<__mystring <<" at "<<sc_core::sc_time_stamp()<<std::endl;    \
    exit(-1);                                                               \
} while (0)

/// Macro to print debug information from SC modules
/// @param[in] format print format
#define TLM_DBG(format, ...)                                                \
do {                                                                        \
    char __mystring[1024];                                                  \
    sprintf(__mystring, "%s: " format, this->name(), __VA_ARGS__);          \
    std::cout<<__mystring <<" at "<<sc_core::sc_time_stamp()<<std::endl;    \
} while (0)

/// Macro to print debug information
/// @param[in] name name to print
/// @param[in] format print format
#define SYS_DBG(name, format, ...)                                          \
do {                                                                        \
    char __mystring[1024];                                                  \
    sprintf(__mystring, "%s: " format, name, __VA_ARGS__);                  \
    std::cout<<__mystring <<" at "<<sc_core::sc_time_stamp()<<std::endl;    \
} while (0)

/// Macro that checks the sanity of an AHB transaction
/// @param[in] __t transaction to check
#define TLM_TRANS_SANITY(__t)                                               \
do {                                                                        \
    assert((__t).get_byte_enable_ptr() == 0);                               \
    assert((__t).get_streaming_width() == 4);                               \
    assert(((__t).get_command() == tlm::TLM_WRITE_COMMAND) ||               \
           ((__t).get_command() == tlm::TLM_READ_COMMAND));                 \
} while(0)

/// Macro that checks the sanity of a 4 byte word transaction
/// @param[in] __t transaction to check
#define TLM_WORD_SANITY(__t)                                                \
do {                                                                        \
    assert(((__t).get_address()&0x3) == 0);                                 \
    assert((__t).get_byte_enable_ptr() == 0);                               \
    assert((__t).get_data_length() == 4);                                   \
    assert((__t).get_streaming_width() == 4);                               \
    assert(((__t).get_command() == tlm::TLM_WRITE_COMMAND) ||               \
           ((__t).get_command() == tlm::TLM_READ_COMMAND));                 \
} while(0)

/// Macro that checks the sanity of an interrupt transaction
/// @param[in] __t transaction to check
#define TLM_INT_SANITY(__t)                                                 \
do {                                                                        \
    uint32_t* ptr = reinterpret_cast<uint32_t*>((__t).get_data_ptr());      \
    TLM_WORD_SANITY(__t);                                                   \
    assert((__t).get_command() == tlm::TLM_WRITE_COMMAND);                  \
    assert((__t).get_address() == 0);                                       \
    assert(*ptr < 2);                                                       \
} while(0)

/// Macro to make a blocking access
/// @param[in] __s socket to use to perform access
/// @param[in] __t transaction object to use to perform access
/// @param[in] __y time object to use to indicate delay
/// @param[in] __c command type
/// @param[in] __a address of the transaction
/// @param[in] __d data object of the transaction
/// @param[in] __l length of the request
#define TLM_B_TRANS(__s, __t, __y, __c, __a, __d, __l)                      \
do {                                                                        \
    (__t).set_command(__c);                                                 \
    (__t).set_address(__a);                                                 \
    (__t).set_data_ptr(reinterpret_cast<unsigned char*>(&(__d)));           \
    (__t).set_data_length(__l);                                             \
    (__t).set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);                \
    (__y) = sc_core::SC_ZERO_TIME;                                          \
    (__s)->b_transport(__t, __y);                                           \
    assert(!(__t).is_response_error());                                     \
} while(0)

/// Macro to make a blocking write 4 byte access.
/// @param[in] __s socket to use to perform access
/// @param[in] __t transaction object to use to perform access
/// @param[in] __y time object to use to indicate delay
/// @param[in] __a address of the transaction
/// @param[in] __d data object of the transaction
#define TLM_B_WR_WORD(__s, __t, __y, __a, __d)                              \
    TLM_B_TRANS(__s, __t, __y, tlm::TLM_WRITE_COMMAND, __a, __d, 4)

/// Macro to make a blocking write 2 byte access.
/// @param[in] __s socket to use to perform access
/// @param[in] __t transaction object to use to perform access
/// @param[in] __y time object to use to indicate delay
/// @param[in] __a address of the transaction
/// @param[in] __d data object of the transaction
#define TLM_B_WR_HALFWORD(__s, __t, __y, __a, __d)                          \
    TLM_B_TRANS(__s, __t, __y, tlm::TLM_WRITE_COMMAND, __a, __d, 2)

/// Macro to make a blocking write byte access.
/// @param[in] __s socket to use to perform access
/// @param[in] __t transaction object to use to perform access
/// @param[in] __y time object to use to indicate delay
/// @param[in] __a address of the transaction
/// @param[in] __d data object of the transaction
#define TLM_B_WR_BYTE(__s, __t, __y, __a, __d)                              \
    TLM_B_TRANS(__s, __t, __y, tlm::TLM_WRITE_COMMAND, __a, __d, 1)

/// Macro to make a blocking read 4 byte access.
/// @param[in] __s socket to use to perform access
/// @param[in] __t transaction object to use to perform access
/// @param[in] __y time object to use to indicate delay
/// @param[in] __a address of the transaction
/// @param[in, out] __d data object of the transaction
#define TLM_B_RD_WORD(__s, __t, __y, __a, __d)                              \
    TLM_B_TRANS(__s, __t, __y, tlm::TLM_READ_COMMAND, __a, __d, 4);         \
    if (UTILS_DEBUG_LEVEL > 0)                                              \
        TLM_DBG("read word @0x%08X = 0x%X", __a, __d)

/// Macro to make a blocking read 2 byte access.
/// @param[in] __s socket to use to perform access
/// @param[in] __t transaction object to use to perform access
/// @param[in] __y time object to use to indicate delay
/// @param[in] __a address of the transaction
/// @param[in, out] __d data object of the transaction
#define TLM_B_RD_HALFWORD(__s, __t, __y, __a, __d)                          \
    TLM_B_TRANS(__s, __t, __y, tlm::TLM_READ_COMMAND, __a, __d, 2);         \
    if (UTILS_DEBUG_LEVEL > 0)                                              \
        TLM_DBG("read half @0x%08X = 0x%X", __a, __d)

/// Macro to make a blocking read byte access.
/// @param[in] __s socket to use to perform access
/// @param[in] __t transaction object to use to perform access
/// @param[in] __y time object to use to indicate delay
/// @param[in] __a address of the transaction
/// @param[in, out] __d data object of the transaction
#define TLM_B_RD_BYTE(__s, __t, __y, __a, __d)                              \
    TLM_B_TRANS(__s, __t, __y, tlm::TLM_READ_COMMAND, __a, __d, 1);         \
    if (UTILS_DEBUG_LEVEL > 0)                                              \
        TLM_DBG("read byte @0x%08X = 0x%X", __a, __d)

/// Macro to make an unknown size copy from APP buffer to EMB buffer
/// @param[in] __s socket for the EMB access
/// @param[in] __t transaction object to use to perform access
/// @param[in] __y time object to use to indicate delay
/// @param[in] __a pointer to the APP buffer
/// @param[in] __e next address in the EMB buffer
/// @param[in] __r remaining bytes to copy
/// @param[out] __n number of bytes copied
#define TLM_B_A2E_COPY(__s, __t, __y, __a, __e, __r, __n)                   \
    do {                                                                    \
        uint32_t __d;                                                       \
        __d = *((uint32_t*)(__a));                                          \
        if ((((__e) & 3) == 0) && ((__r) >= 4)) {                           \
            (__n) = 4;                                                      \
            TLM_B_WR_WORD((__s), (__t), (__y), (__e), __d);                 \
        }                                                                   \
        else if ((((__e) & 1) == 0) && ((__r) >= 2)) {                      \
            (__n) = 2;                                                      \
            __d &= 0xFFFF;                                                  \
            TLM_B_WR_HALFWORD((__s), (__t), (__y), (__e), __d);             \
        }                                                                   \
        else {                                                              \
            (__n) = 1;                                                      \
            __d &= 0xFF;                                                    \
            TLM_B_WR_BYTE((__s), (__t), (__y), (__e), __d);                 \
        }                                                                   \
    } while(false)

/// Macro to make an unknown size copy from EMB buffer to APP buffer
/// @param[in] __s socket for the EMB access
/// @param[in] __t transaction object to use to perform access
/// @param[in] __y time object to use to indicate delay
/// @param[in] __e next address in the EMB buffer
/// @param[in, out] __a pointer to the APP buffer
/// @param[in] __r remaining bytes to copy
/// @param[out] __n number of bytes copied
#define TLM_B_E2A_COPY(__s, __t, __y, __e, __a, __r, __n)                   \
    do {                                                                    \
        uint32_t __d;                                                       \
        if ((((__e) & 3) == 0) && ((__r) >= 4)) {                           \
            (__n) = 4;                                                      \
            TLM_B_RD_WORD((__s), (__t), (__y), (__e), __d);                 \
            *((uint32_t*)(__a)) = __d;                                      \
        }                                                                   \
        else if ((((__e) & 1) == 0) && ((__r) >= 2)) {                      \
            (__n) = 2;                                                      \
            TLM_B_RD_HALFWORD((__s), (__t), (__y), (__e), __d);             \
            *((uint16_t*)(__a)) = __d;                                      \
        }                                                                   \
        else {                                                              \
            (__n) = 1;                                                      \
            TLM_B_RD_BYTE((__s), (__t), (__y), (__e), __d);                 \
            *((uint8_t*)(__a)) = __d;                                       \
        }                                                                   \
    } while(false)

/// Macro to set an interrupt.
/// @param[in] __s socket for the interrupt access
/// @param[in] __t transaction object to use to perform access
/// @param[in] __y time object to use to indicate delay
#define TLM_INT_SET(__s, __t, __y)                                          \
do {                                                                        \
    uint32_t __da = 1;                                                      \
    TLM_B_WR_WORD(__s, __t, __y, 0, __da);                                  \
} while(0)

/// Macro to clear an interrupt.
/// @param[in] __s socket for the interrupt access
/// @param[in] __t transaction object to use to perform access
/// @param[in] __y time object to use to indicate delay
#define TLM_INT_CLR(__s, __t, __y)                                          \
do {                                                                        \
    uint32_t __da = 0;                                                      \
    TLM_B_WR_WORD(__s, __t, __y, 0, __da);                                  \
} while(0)

/// Macro to make a debug transport 4 byte word access
/// @param[in] __s socket for the interrupt access
/// @param[in] __t transaction object to use to perform access
/// @param[in] __c command type
/// @param[in] __a address of the transaction
/// @param[in] __p data pointer of the transaction
/// @param[in] __l length of the request
#define TLM_DBG_TRANS(__s, __t, __c, __a, __p, __l)                         \
unsigned int n_bytes;                                                       \
do {                                                                        \
    (__t).set_command(__c);                                                 \
    (__t).set_address(__a);                                                 \
    (__t).set_data_ptr(reinterpret_cast<unsigned char*>(__p));              \
    (__t).set_data_length(__l);                                             \
    (__t).set_streaming_width(4);                                           \
    (__t).set_byte_enable_ptr(0);                                           \
    (__t).set_dmi_allowed(false);                                           \
    n_bytes =(__s)->transport_dbg(__t);                                     \
} while(0)

/// Macro to make a debug transport write 4 byte word access.
/// @param[in] __s socket for the interrupt access
/// @param[in] __t transaction object to use to perform access
/// @param[in] __a address of the transaction
/// @param[in] __p data pointer of the transaction
/// @param[in] __l length of the request
#define TLM_DBG_WR(__s, __t, __a, __p, __l)                                 \
    TLM_DBG_TRANS(__s, __t, tlm::TLM_WRITE_COMMAND, __a, __p, __l)

/// Macro to make a debug transport read 4 byte word access.
/// @param[in] __s socket for the interrupt access
/// @param[in] __t transaction object to use to perform access
/// @param[in] __a address of the transaction
/// @param[in] __p data pointer of the transaction
/// @param[in] __l length of the request
#define TLM_DBG_RD(__s, __t, __a, __p, __l)                                 \
    TLM_DBG_TRANS(__s, __t, tlm::TLM_READ_COMMAND, __a, __p, __l)

/// Macro to retrieve and execute the information from a DBG transaction
/// @param[in] __t transaction object to perform
/// @param[in] __p pointer to the object data container start
/// @param[in] __m maximum number of bytes in the data container
#define TLM_DBG_EXEC_COPY_NORETURN(__t, __p, __m)                           \
    sc_dt::uint64 __a = (__t).get_address();                                \
    uint32_t __l = (__t).get_data_length();                                 \
    uint8_t* __pin = (__t).get_data_ptr();                                  \
    uint8_t* __pout = (uint8_t*)__p;                                        \
    uint32_t __s = ((__a + __l) <= (__m))?__l:((__m) - __a);                \
    assert(__a < __m);                                                      \
    if ((__t).get_command() == tlm::TLM_READ_COMMAND)                       \
        memcpy(__pin, __pout + __a, __s);                                   \
    else                                                                    \
        memcpy(__pout + __a, __pin, __s)

/// Macro to retrieve and execute the information from a DBG transaction
/// @param[in] __t transaction object to perform
/// @param[in] __p pointer to the object data container start
/// @param[in] __m maximum number of bytes in the data container
/// @return number of bytes copied
#define TLM_DBG_EXEC(__t, __p, __m)                                         \
    TLM_DBG_EXEC_COPY_NORETURN(__t, __p, __m);                              \
    return __s

/// Macro to read an unsigned 32 bit value at the address of an other object
/// @param[in] __a 32 bit object to read
#define U32(__o) (*((uint32_t*)(&(__o))))

/// Macro to read a signed 32 bit value at the address of an other object
/// @param[in] __a 32 bit object to read
#define S32(__o) (*((int32_t*)(&(__o))))

/// Macro to read an unsigned 16 bit value at the address of an other object
/// @param[in] __a 16 bit object to read
#define U16(__o) (*((uint16_t*)(&(__o))))

extern int rand_ps();

#endif /*UTILS_H_*/
