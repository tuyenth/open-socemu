/** @file ElfReader.h
 * @brief Definition of the API to read ELF files segments
 * 
 * File containing an API to retrieve the segments from an ELF (Executable and Linking 
 * Format) file.  Typically, the user uses the class ElfReader and its methods to parse
 * the ELF file and retrieve successively the pointers to the Segment instances that
 * represent all the segments of the ELF file.
 * 
 * An typical use case of this API, the ELF file containing 3 segments:
 * @msc
 *  Caller,ElfReader;
 *  Caller=>ElfReader [label="Open(ElfFileName)", URL="\ref ElfReader::Open()"];
 *  Caller=>ElfReader [label="GetNextSegment()", URL="\ref ElfReader::GetNextSegment()"];
 *  Caller<<ElfReader [label="Segment*", URL="\ref Segment", ID="1"];
 *  ---               [label="Use the segment to load the system memory"];
 *  Caller=>ElfReader [label="GetNextSegment()", URL="\ref ElfReader::GetNextSegment()"];
 *  Caller<<ElfReader [label="Segment*", URL="\ref Segment", ID="2"];
 *  ---               [label="Use the segment to load the system memory"];
 *  Caller=>ElfReader [label="GetNextSegment()", URL="\ref ElfReader::GetNextSegment()"];
 *  Caller<<ElfReader [label="Segment*", URL="\ref Segment", ID="3"];
 *  ---               [label="Use the segment to load the system memory"];
 *  Caller=>ElfReader [label="GetNextSegment()", URL="\ref ElfReader::GetNextSegment()"];
 *  Caller<<ElfReader [label="NULL"];
 *  ---               [label="No more segments"];
 * @endmsc
 * The equivalent code of this picture is:
 * @code
 * ElfReader ElfReader;
 * Segment* Segment;
 * ElfReader.Open("ElfFileName");
 * while ((Segment = ElfReader.GetNextSegment()) != NULL)
 * {
 *     ... Segment->Data() copied at Segment->Size()
 * }
 * @endcode
 * 
 * @see ElfReader
 * @see Segment
 * @see http://fr.wikipedia.org/wiki/Executable_and_Linking_Format
 * For more information about the ELF file format.
 */

#ifndef ELFREADER_H_
#define ELFREADER_H_

// for C99 integer types
#include <stdint.h>
// file control options (O_RDONLY)
#include <fcntl.h>
// data returned by the stat() function
#include <sys/stat.h>
// memory management declarations
#include <sys/mman.h>
// verify program assertion
#include <assert.h>
// system error numbers
#include <errno.h>
// ELF declarations
#include <elf.h>


// for SYS_ERR
#include "utils.h"

/// Define this for debug purpose
//#define ELFREADER_DEBUG is_defined

/// Debug output if needed
#ifdef ELFREADER_DEBUG
#define elfreader_dbg printf
#else
#define elfreader_dbg(...)
#endif

/** @brief Class representing a loadable segment of an ELF file
 * Contains all the relevant information concerning a loadable segment of an ELF file.
 * This information is extracted from the program headers of the ELF file.
 * @see ElfReader
 */
class Segment
{
private:
    /// Segment data pointer
    char* m_Data;
    /// Segment data size
    int m_Size;
    /// Segment data address in system memory
    uint32_t m_Address;

public:
    /** Default constructor
     * @param[in, out] Data Pointer to the data of the segment to create
     * @param[in] Size Size of the Segment
     * @param[in] Address Address of the segment in memory
     */
    Segment(char* Data, int Size, uint32_t Address) :
        m_Data(Data), m_Size(Size), m_Address(Address)
    {
    }
    
    /** @brief Retrieve the segment data pointer
     * The pointer points to a location in the ELF file.  The ELF file content is memory
     * mapped, this pointer contains a memory address and can be accessed immediately.
     * @see ElfReader::Open()
     */
    char* Data(void)
    {
        return m_Data;
    }
    
    /** @brief Retrieve the segment data size
     * Indicates the size of the loadable segment.  In the ELF file format, this
     * information can be retrieved from two different fields: Ehdr->Phdr->filesz or 
     * Ehdr->Phdr->memsz.  Usually the two values are identical.  For security reasons,
     * this value is the copy of Ehdr->Phdr->filesz, to prevent segmentation faults.
     */
    int Size(void)
    {
        return m_Size;
    }
    
    /** @brief Retrieve the segment system address
     * Indicates at which location in the system the loadable section must be seen by the
     * processor.  Therefore this is a virtual address that must take into account any
     * system level translation that is not known at the time of the compilation or is not
     * indicated explicitly in the linker description file.
     */
    uint32_t Address(void)
    {
        return m_Address;
    }
};

/** @brief Class representing an ELF file
 * This class helps in parsing the loadable segments of an ELF file into a system memory.
 * It hides all the complexity of the ELF format behind a few method calls.
 * @see Segment
 */
class ElfReader
{
private:
    /// File descriptor of the ELF file
    int m_Fd;
    /// Pointer to the memory area where the ELF file content is mmap'd
    void* m_MmapBuf;
    /// Size of the memory area where the ELF file is mmap'd
    int m_MmapSize;
    /// Contains the number of segments in the ELF file
    int m_SegmentsNum;
    /// Contains the number of segments in the ELF file
    int m_CurrentSegment;
    /// Array of the segments in the ELF file
    Segment** m_Segments;
    
public:
    /** @brief Default constructor
     * The constructor initializes the private members with the default values to make
     * sure that any misuse of the API can not lead to crash.  But no dynamic allocation
     * is performed during this phase.
     */
    ElfReader() :
        m_Fd(-1), m_MmapBuf(NULL), m_MmapSize(0), m_SegmentsNum(0), m_CurrentSegment(0)
    {
    }

    /** @brief Default destructor
     * The destructor takes care of freeing all the memory that was allocated during the
     * lifetime of the instance.
     */
    ~ElfReader()
    {
        int i;

        // check if the file was successfully opened
        if (m_Fd != -1)
        {
            // check if the file content was successfully mapped
            if (m_MmapBuf != NULL)
            {
                // unmap the file content
                munmap(m_MmapBuf, m_MmapSize);
            }
            // close the file
            close(m_Fd);


            // destroy the Segment instances
            for (i = 0; i < m_SegmentsNum; i++)
            {
                delete m_Segments[i];
            }

            // destroy the array of Segments
            delete [] m_Segments;
        }
    }

    /** @brief Open and parse the ELF file
     * The function fills the internal array with the ELF file loadable segments
     * found in the ELF file.  The segments information can be retrieved after this 
     * function call.
     * @warning If the user omits to call this function, no segments can be retrieved but
     * this mistake is handled and can not lead to a crash
     * @param[in] file Name of the ELF file to parse
     * @see ElfReader::GetNextSegment()
     */
    void Open(const char* file)
    {
        int i;
        struct stat stat;
        Elf32_Ehdr* Ehdr;
        Elf32_Phdr* Phdr;
        Elf32_Shdr* Shdr;

        // print information
        elfreader_dbg("elfreader:\n");
        elfreader_dbg("  - file '%s'\n", file);

        // open the file needed
        m_Fd = open(file, O_RDONLY);
        if (m_Fd == -1)
        {
            SYS_ERR("ElfReader.Open", "ERROR: open %s (%s)", file, strerror(errno));
        }

        // get the file information
        fstat(m_Fd, &stat);

        // map structure elf Ehdr
        m_MmapSize = stat.st_size;
        m_MmapBuf = mmap(NULL, m_MmapSize, PROT_READ, MAP_PRIVATE, m_Fd, 0);
        Ehdr = (Elf32_Ehdr*)m_MmapBuf;
        // sanity check, this can not happen
        assert(Ehdr != NULL);
        if (Ehdr == MAP_FAILED)
        {
            SYS_ERR("ElfReader.Open", "ERROR: mmap (%s)", strerror(errno));
        }
        // check that the specified file is elf
        if ((Ehdr->e_ident[EI_MAG0]!=ELFMAG0)||(Ehdr->e_ident[EI_MAG1]!=ELFMAG1)||
            (Ehdr->e_ident[EI_MAG2]!=ELFMAG2)||(Ehdr->e_ident[EI_MAG3]!=ELFMAG3))
        {
            SYS_ERR("ElfReader.Open", "ERROR: magic number not correct (byte compare) (0x%02X)", Ehdr->e_ident[EI_MAG0]);
        }
        // check that the specified file is elf
        if (memcmp(&(Ehdr->e_ident[EI_MAG0]), ELFMAG, SELFMAG))
        {
            SYS_ERR("ElfReader.Open", "ERROR: magic number not correct (string compare) (0x%02X)", Ehdr->e_ident[EI_MAG0]);
        }
        // check that the elf file is 32 bit class
        if (Ehdr->e_ident[EI_CLASS] != ELFCLASS32)
        {
            SYS_ERR("ElfReader.Open", "ERROR: class not 32 bit, therefore not supported (%d)", Ehdr->e_ident[EI_CLASS]);
        }
        else
        {
            elfreader_dbg("  - word format: 32 bit\n");
        }
        // check the elf file little or big endianness
        if (Ehdr->e_ident[EI_DATA] == ELFDATA2LSB)
        {
            elfreader_dbg("  - data format: little endian\n");
        }
        else
        {
            elfreader_dbg("  - data format: big endian\n");
        }
        // check the elf file version
        if (Ehdr->e_ident[EI_VERSION] == EV_NONE)
        {
            elfreader_dbg("  - version: not specified\n");
        }
        else
        {
            elfreader_dbg("  - version: current\n");
        }
        // check the elf file ABI (Application Binary Interface)
        if (Ehdr->e_ident[EI_OSABI] == ELFOSABI_ARM)
        {
            elfreader_dbg("  - ABI: ARM\n");
        }
        else
        {
            elfreader_dbg("  - ABI: not ARM\n");
        }
        // check the elf file type
        elfreader_dbg("  - type: ");
        switch(Ehdr->e_type)
        {
        case ET_NONE:
            elfreader_dbg("none\n");
            break;
        case ET_REL:
            elfreader_dbg("relocatable\n");
            break;
        case ET_EXEC:
            elfreader_dbg("executable\n");
            break;
        case ET_DYN:
            elfreader_dbg("shared object\n");
            break;
        case ET_CORE:
            elfreader_dbg("core\n");
            break;
        }
        // check the elf file machine
        if (Ehdr->e_machine == EM_ARM)
        {
            elfreader_dbg("  - machine: ARM\n");
        }
        else
        {
            elfreader_dbg("  - machine: other than ARM\n");
        }
        elfreader_dbg("  - flags: %d\n", Ehdr->e_flags);
        // the number of program headers
        elfreader_dbg("  - program headers: %d\n", Ehdr->e_phnum);
        elfreader_dbg("  - section headers: %d\n", Ehdr->e_shnum);
        elfreader_dbg("  - string section index: %d\n", Ehdr->e_shstrndx);

        // check supported types only
        if ((Ehdr->e_type != ET_EXEC) || (Ehdr->e_machine != EM_ARM))
        {
            SYS_ERR("ElfReader.Open", "ERROR: only EXEC files for ARM are supported (%d)", Ehdr->e_machine);
        }
        // check that the data format is little endian only
        if (Ehdr->e_ident[EI_DATA] != ELFDATA2LSB)
        {
            SYS_ERR("ElfReader.Open", "ERROR: only LITTLE ENDIAN data formats are supported (%d)", Ehdr->e_ident[EI_DATA]);
        }

        // check that the file has at least one program header element
        if (Ehdr->e_phnum == 0)
        {
            SYS_ERR("ElfReader.Open", "ERROR: only ELF file with program headers are supported (%d)", Ehdr->e_phnum);
        }
        assert(Ehdr->e_phoff!=0);

        // parse the program headers
        for (i = 0; i < Ehdr->e_phnum; i++)
        {
            elfreader_dbg("  - segment %d:\n", i);
            // map the program header
            Phdr = (Elf32_Phdr*)((uint32_t)Ehdr + Ehdr->e_phoff + (Ehdr->e_phentsize * i));

            // print the program header
            elfreader_dbg("    + offset: 0x%x\n", Phdr->p_offset);
            elfreader_dbg("    + type: %d\n", Phdr->p_type);
            elfreader_dbg("    + vaddr: 0x%08X\n", Phdr->p_vaddr);
            elfreader_dbg("    + paddr: 0x%08X\n", Phdr->p_paddr);
            elfreader_dbg("    + fsize: 0x%x\n", Phdr->p_filesz);
            elfreader_dbg("    + msize: 0x%x\n", Phdr->p_memsz);
            elfreader_dbg("    + flags: 0x%08X\n", Phdr->p_flags);
            elfreader_dbg("    + align: %d\n", Phdr->p_align);

        }

        // parse section headers
        for (i = 0; i < Ehdr->e_shnum; i++)
        {
            elfreader_dbg("  - section %d:\n", i);
            // map the section header
            Shdr = (Elf32_Shdr*)((uint32_t)Ehdr + Ehdr->e_shoff + (Ehdr->e_shentsize * i));

            // print the section header
            elfreader_dbg("    + name index: %d\n", Shdr->sh_name);
            elfreader_dbg("    + type: %d\n", Shdr->sh_type);
            elfreader_dbg("    + flags: 0x%08X\n", Shdr->sh_flags);
            elfreader_dbg("    + addr: 0x%08X\n", Shdr->sh_addr);
            elfreader_dbg("    + offset: 0x%x\n", Shdr->sh_offset);
            elfreader_dbg("    + size: 0x%x\n", Shdr->sh_size);
            elfreader_dbg("    + index: %d\n", Shdr->sh_link);
            elfreader_dbg("    + info: %d\n", Shdr->sh_info);
            elfreader_dbg("    + align: %d\n", Shdr->sh_addralign);
            elfreader_dbg("    + entry size: %d\n", Shdr->sh_entsize);
        }

        // initialize the number of segments
        m_SegmentsNum = Ehdr->e_phnum;

        // allocate the array of Segment pointers
        m_Segments = new Segment * [m_SegmentsNum];

        // fill the arrays
        for (i=0; i < Ehdr->e_phnum; i++)
        {
            // map the program header
            Phdr = (Elf32_Phdr*)((uint32_t)Ehdr + Ehdr->e_phoff + (Ehdr->e_phentsize * i));

            m_Segments[i] = new Segment((char*)Ehdr + Phdr->p_offset, Phdr->p_filesz, Phdr->p_vaddr);

            elfreader_dbg("  - segment %d: %d bytes at 0x%08X\n", i, m_Segments[i]->Size(),
                    (int)m_Segments[i]->Address());
        }

        elfreader_dbg("  - number of segments: %d\n", m_SegmentsNum);
    }

    /** Retrieve the next segment in the ELF file
     * @warning If the Open method has not been called or was not able to complete
     * successfully, this function always returns NULL.
     * @return Pointer to a Segment class instance or NULL if there are no more instances
     * @see ElfReader::Open()
     */
    Segment* GetNextSegment()
    {
        // check if the current segment is within boundaries
        if (m_CurrentSegment < m_SegmentsNum)
        {
            // return the current segment pointer and increment the index
            return m_Segments[m_CurrentSegment++];
        }
        else
        {
            // return NULL
            return NULL;
        }
    }
};

#endif /*ELFREADER_H_*/
