#ifndef PARAMETERS_H_
#define PARAMETERS_H_

struct Parameters
{
    bool gdb_enabled;
    bool gdb_wait;
    const char* elffile;
    const char* romfile;
    const char* flashfile;
    bool flash_preloaded;
};

#endif /*PARAMETERS_H_*/
