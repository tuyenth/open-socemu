#include "Top.h"
#include "At91sam9261.h"
#include "Mc13224v.h"
#include "B2070.h"
#include "ticpp.h"

void usage(void)
{
    printf("\n"
            "Synopsis:\n"
            "    tlm_arm_nx [-d] (configfile)\n\n"
            "Parameters:\n"
            "    - -d : indicates that the connection to the debugger should\n"
            "        be polled on before starting the execution\n\n"
            "    - configfile : XML file containing the configuration of the\n"
            "        plateform.\n\n"
            "Config file description:\n"
            "    - element 'plateform':\n"
            "        + attribute 'name':\n"
            "            * example : plain ARM7TDMI with SRAM memory at address 0\n"
            "            * ref : reference plateform (at91sam9261) and 11nX model\n"
            "            * TBD\n"
            "        + attribute 'debug':\n"
            "            * 1 | true: indicates that the ISS must support debugger connection\n"
            "            * 0 | false: does not support debugger connection (faster execution)\n"
            "    - element 'elf':\n"
            "        + attribute 'name': path to the ELF file\n"
            "    - element 'rom':\n"
            "        + attribute 'name': path to the ROM binary content file\n"
            "    - element 'flash':\n"
            "        + attribute 'name': path to the FLASH binary content file\n"
            "        + attribute 'preloaded':\n"
            "            * 1 | true: FLASH content is preloaded in RAM\n"
            "            * 0 | false: FLASH content is not preloaded in RAM");

}

/// Main SystemC entry point to our environment
int sc_main(int argc, char* argv[])
{
    struct Parameters Parameters;
    char* configfile = NULL;
    std::string configpath, plateform, elffile, romfile, flashfile;
    uint8_t i;
    size_t pos;

    // initialize the parameters
    Parameters.gdb_enabled = false;
    Parameters.gdb_wait = false;
    Parameters.elffile = NULL;
    Parameters.romfile = NULL;
    Parameters.flashfile = NULL;
    Parameters.flash_preloaded = false;


    // checks arguments number
    if ((argc < 2) || (argc > 3))
    {
        usage();
        return -1;
    }

    for (i=1; i<argc; i++)
    {
        if (!memcmp(argv[i], "-d", 2))
        {
            Parameters.gdb_wait = true;
            Parameters.gdb_enabled = true;
        }
        else
        {
            configfile = argv[i];
        }
    }

    // check that at least a configuration file was provided
    if (configfile == NULL)
    {
        printf("ERROR: configuration file not provided\n");
        return -1;
    }

    // extract the path from the configuration file
    configpath.assign(configfile);
    // find the last occurrence of / or \ in the string
    pos = ((int)configpath.rfind('/') > (int)configpath.rfind('\\'))?configpath.rfind('/'):configpath.rfind('\\');
//    printf("%d,%d => %d", configpath.rfind('/'),configpath.rfind('\\'), pos);
    if (pos != std::string::npos)
        configpath.erase(pos+1);
    else
        configpath.clear();

    try
    {
        ticpp::Document xmldoc(configfile);
        xmldoc.LoadFile();

        ticpp::Iterator< ticpp::Element > element;


        for ( element = element.begin( &xmldoc ); element != element.end(); element++ )
        {
            if (element->Value() == "plateform")
            {
                ticpp::Iterator< ticpp::Attribute > attribute;
                for ( attribute = attribute.begin( &*element ); attribute != attribute.end(); attribute++ )
                {
                    if (attribute->Name() == "name")
                    {
                        plateform = attribute->Value();
                    }
                    else if ((attribute->Name() == "debug") && !Parameters.gdb_wait)
                    {
                        if ((attribute->Value() == "0") || (attribute->Value() == "false"))
                        {
                            Parameters.gdb_enabled = false;
                        }
                        else if ((attribute->Value() == "1") || (attribute->Value() == "true"))
                        {
                            Parameters.gdb_enabled = true;
                        }
                        else
                        {
                            printf("ERROR: unsupported value (%s) in XML %s.%s\n",
                                    attribute->Value().c_str(), element->Value().c_str(),
                                    attribute->Name().c_str());
                            return -1;
                        }
                    }
                }
            }
            else if (element->Value() == "elf")
            {
                ticpp::Iterator< ticpp::Attribute > attribute("name");
                for ( attribute = attribute.begin( &*element ); attribute != attribute.end(); attribute++ )
                {
                    elffile = attribute->Value();
                    // prepend with the configuration file path if not absolute path
                    if (elffile[0] != '/' && elffile[0] != '\\' &&
                        elffile[1] != ':')
                        elffile.insert(0, configpath);
                    Parameters.elffile = elffile.c_str();
                }
            }
            else if (element->Value() == "rom")
            {
                ticpp::Iterator< ticpp::Attribute > attribute("name");
                for ( attribute = attribute.begin( &*element ); attribute != attribute.end(); attribute++ )
                {
                    romfile = attribute->Value();
                    if (romfile[0] != '/' && romfile[0] != '\\' &&
                        romfile[1] != ':')
                        romfile.insert(0, configpath);
                    Parameters.romfile = romfile.c_str();
                }
            }
            else if (element->Value() == "flash")
            {
                ticpp::Iterator< ticpp::Attribute > attribute;
                for ( attribute = attribute.begin( &*element ); attribute != attribute.end(); attribute++ )
                {
                    if (attribute->Name() == "name")
                    {
                        flashfile = attribute->Value();
                        if (flashfile[0] != '/' && flashfile[0] != '\\' &&
                            flashfile[1] != ':')
                            flashfile.insert(0, configpath);
                        Parameters.flashfile = flashfile.c_str();
                    }
                    else if (attribute->Name() == "preloaded")
                    {
                        if ((attribute->Value() == "0") || (attribute->Value() == "false"))
                        {
                            Parameters.flash_preloaded = false;
                        }
                        else if ((attribute->Value() == "1") || (attribute->Value() == "true"))
                        {
                            Parameters.flash_preloaded = true;
                        }
                        else
                        {
                            printf("ERROR: unsupported value (%s) in XML %s.%s\n",
                                    attribute->Value().c_str(), element->Value().c_str(),
                                    attribute->Name().c_str());
                        }
                    }
                }
            }
        }
    }
    catch( ticpp::Exception& ex )
    {
        std::cout << ex.what();
        exit(-1);
    }

    if (plateform == "")
    {
        printf("ERROR: configuration file does not contain plateform type\n");
        return -1;
    }

    printf("  - Config file name %s\n", configfile);
    printf("  - Config file path %s (%d)\n", configpath.c_str(), pos);
    printf("  - Plateform name %s\n", plateform.c_str());
    printf("  - GDB server %s\n", Parameters.gdb_enabled?"ON":"OFF");
    printf("  - GDB wait at start %s\n", Parameters.gdb_wait?"ON":"OFF");
    if (Parameters.elffile != NULL)
        printf("  - ELF file name %s\n", Parameters.elffile);
    if (Parameters.romfile != NULL)
        printf("  - ROM file name %s\n", Parameters.romfile);
    if (Parameters.flashfile != NULL)
        printf("  - FLASH file name %s\n", Parameters.flashfile);


    // check if it is an AT91SAM9261 plateform that is requested
    if (plateform == "ref")
    {
        At91sam9261 at91sam9261("at91sam9261", Parameters);
    }
    else if (plateform == "mc13224v")
    {
        Mc13224v mc13224v("mc13224v", Parameters);
    }
    else if (plateform == "b2070")
    {
        B2070 b2070("b2070", Parameters);
    }
    else
    {
        // create the Top level system
        Top top("top", Parameters);
    }

    sc_core::sc_start();
    return 0;
}

