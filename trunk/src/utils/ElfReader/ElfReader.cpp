/** @file ElfReader.cpp
 * @brief Implementation of the API to read ELF files segments
 */

#include <iostream>

#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <assert.h>
#include <errno.h>
#include <elf.h>

#include "ElfReader/ElfReader.h"

/// Define this for debug purpose
//#define ELFREADER_DEBUG is_defined

/// Debug output if needed
#ifdef ELFREADER_DEBUG
# define elfreader_dbg printf
#else
# define elfreader_dbg(...)
#endif

using namespace std;

ElfReader::~ElfReader()
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
        for (i=0; i < m_SegmentsNum; i++)
        {
            delete m_Segments[i];
        }
        
        // destroy the array of Segments
        delete [] m_Segments;
    }
}

void
ElfReader::Open(const char* file)
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
        fprintf (stderr, "ERROR: open %s (%s)\n", file, strerror(errno));
        goto elfreader_exit;
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
        fprintf(stderr, "ERROR: mmap (%s)\n", strerror(errno));
        // reinit the address
        m_MmapBuf = NULL;
        goto elfreader_exit;
    }
    // check that the specified file is elf
    if ((Ehdr->e_ident[EI_MAG0]!=ELFMAG0)||(Ehdr->e_ident[EI_MAG1]!=ELFMAG1)||
        (Ehdr->e_ident[EI_MAG2]!=ELFMAG2)||(Ehdr->e_ident[EI_MAG3]!=ELFMAG3))
    {
        fprintf(stderr, "ERROR: magic number not correct (byte compare)\n");
        goto elfreader_exit;
    }
    // check that the specified file is elf
    if (memcmp(&(Ehdr->e_ident[EI_MAG0]), ELFMAG, SELFMAG))
    {
        fprintf(stderr, "ERROR: magic number not correct (string compare)\n");
        goto elfreader_exit;
    }
    // check that the elf file is 32 bit class
    if (Ehdr->e_ident[EI_CLASS] != ELFCLASS32)
    {
        fprintf(stderr, "ERROR: class not 32 bit, therefore not supported\n");
        goto elfreader_exit;
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
        fprintf(stderr, "ERROR: only EXEC files for ARM are supported\n");
        goto elfreader_exit;
    }
    // check that the data format is little endian only
    if (Ehdr->e_ident[EI_DATA] != ELFDATA2LSB)
    {
        fprintf(stderr, "ERROR: only LITTLE ENDIAN data formats are supported\n");
        goto elfreader_exit;
    }

    // check that the file has at least one program header element
    if (Ehdr->e_phnum == 0)
    {
        fprintf(stderr, "ERROR: only ELF file with program headers are supported\n");
        goto elfreader_exit;
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

elfreader_exit:
    return;
}

Segment*
ElfReader::GetNextSegment()
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




