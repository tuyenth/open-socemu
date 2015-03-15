# Introduction #

This page stores a sample code to parse a I32HEX formatted file into a chained list of binary pieces.


# Details #
```
// standard includes
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
// for open
#include <fcntl.h>
// for read, write and close
#include <unistd.h>
// for strchr
#include <string.h>

void dump_hex_baseaddr(const uint8_t *buf, int size, uint32_t baseaddr)
{
    int i;
    for (i = 0; i < size; i++)
    {
        if ((i%16) == 0)
        {
            printf("0x%08X :", baseaddr+i);
        }
        printf(" %02X", buf[i]);
        if ((i%16) == 15)
        {
            fputs("\n", stdout);
        }
    }
    if ((size%16) != 15)
    {
        fputs("\n", stdout);
    }
}

void dump_hex(const uint8_t *buf, int size)
{
    dump_hex_baseaddr(buf, size, 0);
}

uint8_t read_hex_nibble(const uint8_t c)
{
    if ((c >='0') && (c <= '9'))
    {
        return c - '0';
    }
    else if ((c >='A') && (c <= 'F'))
    {
        return c - 'A' + 10;
    }
    else if ((c >='a') && (c <= 'f'))
    {
        return c - 'a' + 10;
    }
    else
    {
        puts("read_hex_nibble: Error char not in supported range");
        return 0;
    }
}

uint8_t read_hex_byte(const uint8_t *buf)
{
    uint8_t result;

    result = read_hex_nibble(buf[0]) << 4;
    result += read_hex_nibble(buf[1]);
    return result;
}

// this structure represents a contiguous area
struct i32hex
{
    // start address of the area
    uint32_t addr;
    // length of the area
    uint32_t length;
    // area content
    uint8_t *data;
    // pointer to the next contiguous area
    struct i32hex *next;
    // debug info
    int linenum;
};


void free_i32hex(struct i32hex *head)
{
    struct i32hex *current, *next;

    // free all the allocated blocks
    current = head;
    while (current != NULL)
    {
        // if the current contiguous allocation
        if (current->data != NULL)
        {
            free(current->data);
        }
        next = current->next;
        free(current);
        current = next;
    }
}

struct i32hex *parse_i32hexfile(const char *fname)
{
    int fid, linenum, ret, i;
    uint32_t baseaddr = 0;
    uint8_t c, buf[8];
    uint8_t *tmpbuf = NULL;
    struct i32hex *current = NULL;
    struct i32hex *head = NULL;

    fid = open(fname, O_RDONLY);
    if (fid < 0)
    {
        puts("open failed");
        perror("open");
        return NULL;
    }

    // initialize the variables
    linenum = 1;
    do
    {
        int eol_char;
        uint8_t chksum, len, type;
        uint16_t addr;

        // chomp data until the next colon
        do
        {
            // read 1 char
            ret = read(fid, &c, 1);
            if (ret == 0)
            {
                // end of file reached
                goto parse_i32hexfile_eof;
            }
            else if (ret < 0)
            {
                puts("read failed while looking for colon");
                goto parse_i32hexfile_error;
            }
            if (strchr("$: \t\r\n", c) == NULL)
            {
                printf("Unsupported char at line %d\n", linenum);
                goto parse_i32hexfile_error;
            }
            else
            {
                if (c == '\r')
                {
                    eol_char = 1;
                    linenum++;
                }
                else if (c == '\n')
                {
                    if (!eol_char)
                    {
                        linenum++;
                    }
                    eol_char = 0;
                }
                else
                {
                    eol_char = 0;
                }
            }
        } while (c != ':');

        // read the header
        ret = read(fid, buf, 8);
        if (ret <= 0)
        {
            puts("read failed while reading header");
            goto parse_i32hexfile_error;
        }
        len = read_hex_byte(buf);
        addr = (read_hex_byte(&buf[2]) << 8) + read_hex_byte(&buf[4]);
        type = read_hex_byte(&buf[6]);

        // initialize the checksum
        chksum  = read_hex_byte(buf);
        chksum += read_hex_byte(buf+2);
        chksum += read_hex_byte(buf+4);
        chksum += read_hex_byte(buf+6);

        // only allocate a buffer if the length is != 0
        if (len != 0)
        {
            // allocate the right number of bytes for the current record
            tmpbuf = malloc(len);
            if (tmpbuf == NULL)
            {
                puts("failed to allocate temporary buffer");
                goto parse_i32hexfile_error;
            }
            // read the data
            for (i = 0; i < len; i++)
            {
                // read the next byte
                ret = read(fid, buf, 2);
                if (ret <= 0)
                {
                    puts("read failed while reading data");
                    goto parse_i32hexfile_error;
                }
                tmpbuf[i] = read_hex_byte(buf);
                chksum += read_hex_byte(buf);
            }
        }
        // read the checksum
        ret = read(fid, buf, 2);
        if (ret <= 0)
        {
            puts("read failed while reading checksum");
            goto parse_i32hexfile_error;
        }
        chksum += read_hex_byte(buf);
        if (chksum)
        {
            printf("checksum error at line %d\n", linenum);
            goto parse_i32hexfile_error;
        }


        switch(type)
        {
        case 0:
            // if the length is equal to zero, do not do anything
            if (len != 0)
            {
                // check if this is the first element allocated
                if (current == NULL)
                {
                    current = malloc(sizeof(*current));
                    if (current == NULL)
                    {
                        puts("failed allocating first buffer");
                        goto parse_i32hexfile_error;
                    }
                    current->addr = baseaddr + addr;
                    current->length = len;
                    current->data = tmpbuf;
                    current->next = NULL;
                    current->linenum = linenum;

                    // save the head of list
                    head = current;
                }
                else
                {
                    // check if the record is right after last record (to just append)
                    if ((current->addr + current->length) == (baseaddr + addr))
                    {
                        uint8_t *newbuf;
                        newbuf = malloc(current->length + len);
                        if (newbuf == NULL)
                        {
                            puts("failed allocating new buffer");
                            goto parse_i32hexfile_error;
                        }
                        memcpy(newbuf, current->data, current->length);
                        memcpy(&newbuf[current->length], tmpbuf, len);
                        // free the previously allocated chunk
                        free(current->data);
                        current->data = newbuf;
                        current->length += len;
                        // free the data allocated for the current record
                        free(tmpbuf);
                    }
                    else
                    {
                        // allocate a new memory area
                        current->next = malloc(sizeof(*current));
                        if (current->next == NULL)
                        {
                            puts("failed allocating new record");
                            goto parse_i32hexfile_error;
                        }
                        current = current->next;
                        current->addr = baseaddr + addr;
                        current->length = len;
                        current->data = tmpbuf;
                        current->next = NULL;
                        current->linenum = linenum;
                    }
                }
                // allocated temporary buffer has been used
                tmpbuf = NULL;
            }
            // check if the address is located after the previous allocation
            break;
        case 1:
            if (tmpbuf != NULL)
            {
                free(tmpbuf);
                tmpbuf = NULL;
            }
            // end of file record, simply exit
            goto parse_i32hexfile_eof;
            break;
        case 2:
            if (tmpbuf != NULL)
            {
                free(tmpbuf);
                tmpbuf = NULL;
            }
            // not supported
            puts("Unsupported type 2 record");
            goto parse_i32hexfile_error;
            break;
        case 3:
            if (tmpbuf != NULL)
            {
                free(tmpbuf);
                tmpbuf = NULL;
            }
            // not supported
            puts("Unsupported type 3 record");
            goto parse_i32hexfile_error;
            break;
        case 4:
            if (len == 2)
            {
                baseaddr = (tmpbuf[0] << 24) + (tmpbuf[1] << 16);
            }
            if (tmpbuf != NULL)
            {
                free(tmpbuf);
                tmpbuf = NULL;
            }
            if (len != 2)
            {
                puts("type 4 does not have appropriate number of bytes");
                goto parse_i32hexfile_error;
            }
            break;
        case 5:
            if (tmpbuf != NULL)
            {
                free(tmpbuf);
                tmpbuf = NULL;
            }
            // not supported
            puts("Unsupported type 5 record");
            goto parse_i32hexfile_error;
            break;
        default:
            if (tmpbuf != NULL)
            {
                free(tmpbuf);
                tmpbuf = NULL;
            }
            printf("Unknown type at line %d\n", linenum);
            goto parse_i32hexfile_error;
            break;
        }
    } while (1);

parse_i32hexfile_eof:
    close(fid);
    // debug
    current = head;
    i = 0;
    while (current != NULL)
    {
        printf("Segment %d: @0x%08X (line %d)\n", i++, current->addr, current->linenum);
        dump_hex_baseaddr(current->data, current->length, current->addr);
        current = current->next;
    }
    return head;

parse_i32hexfile_error:
    // close the file
    close(fid);

    // free the current record if allocated
    if (tmpbuf != NULL)
    {
        free(tmpbuf);
        tmpbuf = NULL;
    }

    // in case of error, free all the allocated buffers
    free_i32hex(head);
    return NULL;
}



int main(int argc, const char* argv[])
{
    int status = 0;
    struct i32hex *flash;

    int fid;
    uint8_t buf[256];

    // parse the flash content hex file
    flash = parse_i32hexfile("testfiles/fw_and_config.hex");

    puts("exiting");

    // free all the allocated elements
    free_i32hex(flash);

    return status;
}

```