/** @file elfreader.h
 * @brief Definition of the API to read ELF files segments.
 * 
 * File containing an API to retrieve the segments from an ELF (Executable and Linking 
 * Format) file.  Typically, the user uses the class CElfReader and its methods to parse 
 * the ELF file and retrieve successively the pointers to the CSegment instances that 
 * represent all the segments of the ELF file.
 * 
 * An typical use case of this API, the ELF file containing 3 segments:
 * @msc
 *  Caller,ElfReader;
 *  Caller=>ElfReader [label="Open(ElfFileName)", URL="\ref CElfReader::Open()"];
 *  Caller=>ElfReader [label="GetNextSegment()", URL="\ref CElfReader::GetNextSegment()"];
 *  Caller<<ElfReader [label="CSegment*", URL="\ref CSegment", ID="1"];
 *  ---               [label="Use the segment to load the system memory"];
 *  Caller=>ElfReader [label="GetNextSegment()", URL="\ref CElfReader::GetNextSegment()"];
 *  Caller<<ElfReader [label="CSegment*", URL="\ref CSegment", ID="2"];
 *  ---               [label="Use the segment to load the system memory"];
 *  Caller=>ElfReader [label="GetNextSegment()", URL="\ref CElfReader::GetNextSegment()"];
 *  Caller<<ElfReader [label="CSegment*", URL="\ref CSegment", ID="3"];
 *  ---               [label="Use the segment to load the system memory"];
 *  Caller=>ElfReader [label="GetNextSegment()", URL="\ref CElfReader::GetNextSegment()"];
 *  Caller<<ElfReader [label="NULL"];
 *  ---               [label="No more segments"];
 * @endmsc
 * The equivalent code of this picture is:
 * @code
 * CElfReader ElfReader;
 * CSegment* Segment;
 * ElfReader.Open("ElfFileName");
 * while ((Segment = ElfReader.GetNextSegment()) != NULL)
 * {
 *     ... Segment->Data() copied at Segment->Size()
 * }
 * @endcode
 * 
 * @see CElfReader
 * @see CSegment
 * @see http://fr.wikipedia.org/wiki/Executable_and_Linking_Format
 * For more information about the ELF file format.
 */

#ifndef ELFREADER_H_
#define ELFREADER_H_

#include <stdint.h>

/// Class representing a loadable segment of an ELF file.
/** Contains all the relevant information concerning a loadable segment of an ELF file.  
 * This information is extracted from the program headers of the ELF file.
 * @see CElfReader
 */
class CSegment
{
private:
    /// Segment data pointer.
    char* m_Data;
    /// Segment data size.
    int m_Size;
    /// Segment data address in system memory.
    uint32_t m_Address;

public:
    /// Default constructor.
    CSegment(char* Data, int Size, uint32_t Address) : 
        m_Data(Data), m_Size(Size), m_Address(Address) {}
    
    /// Retrieve the segment data pointer.
    /** The pointer points to a location in the ELF file.  The ELF file content is memory 
     * mapped, this pointer contains a memory address and can be accessed immediately.
     * @see CElfReader::Open()
     */
    char* Data() { return m_Data; }
    
    /// Retrieve the segment data size.
    /** Indicates the size of the loadable segment.  In the ELF file format, this 
     * information can be retrieved from two different fields: Ehdr->Phdr->filesz or 
     * Ehdr->Phdr->memsz.  Usually the two values are identical.  For security reasons,
     * this value is the copy of Ehdr->Phdr->filesz, to prevent segmentation faults.
     */
    int Size() { return m_Size; }
    
    /// Retrieve the segment system address.
    /** Indicates at which location in the system the loadable section must be seen by the
     * processor.  Therefore this is a virtual address that must take into account any
     * system level translation that is not known at the time of the compilation or is not
     * indicated explicitely in the linker description file.
     */
    uint32_t Address() { return m_Address; }
};

/// Class representing an ELF file.
/** This class helps in parsing the loadable segments of an ELF file into a system memory.
 * It hides all the complexity of the ELF format behind a few method calls.
 * @see CSegment
 */
class CElfReader
{
private:
    /// File descriptor of the ELF file.
    int m_Fd;
    /// Pointer to the memory area where the ELF file content is mmap'd.
    void* m_MmapBuf;
    /// Size of the memory area where the ELF file is mmap'd.
    int m_MmapSize;
    /// Contains the number of segments in the ELF file.
    int m_SegmentsNum;
    /// Contains the number of segments in the ELF file.
    int m_CurrentSegment;
    /// Array of the segments in the ELF file.
    CSegment** m_Segments;
    
public:
    /// Default constructor.
    /** The constructor initializes the private members with the default values to make
     * sure that any misuse of the API can not lead to crash.  But no dynamic allocation
     * is performed during this phase.
     */
    CElfReader() : 
        m_Fd(-1), m_MmapBuf(NULL), m_MmapSize(0), m_SegmentsNum(0), m_CurrentSegment(0) {}

    /// Default destructor.
    /** The destructor takes care of freeing all the memory that was allocated during the
     * lifetime of the instance.
     */
    ~CElfReader();
    
    /// Open and parse the ELF file.
    /** The function fills the internal array with the ELF file loadable segments 
     * found in the ELF file.  The segments information can be retrieved after this 
     * function call.
     * @warning If the user omits to call this function, no segments can be retrieved but
     * this mistake is handled and can not lead to a crash.
     * @param file Name of the ELF file to parse.
     * @see CElfReader::GetNextSegment()
     */
    void Open(const char* file);
    
    /// Retrieve the next segment in the ELF file.
    /** @warning If the Open method has not been called or was not able to complete 
     * successfully, this function always returns NULL.
     * @return Pointer to a CSegment class instance or NULL if there are no more instances.
     * @see CElfReader::Open()
     */
    CSegment* GetNextSegment();
};

#endif /*ELFREADER_H_*/
