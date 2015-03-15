#summary Directions to build SystemC (for Cygwin specifically).

# Introduction #

Instructions are provided in the SystemC installation on how to build it for several environments:
  * several UNIX flavours
  * Visual C++ 7.0

As a Windows user trying to avoid dependency to tools, I wanted to try to compile it with Cygwin GCC distribution.

# Details #

It was tested with:
  * systemc-2.2.0.tgz
  * gcc 3.4.4 for Cygwin:
```
 gcc (GCC) 3.4.4 (cygming special, gdc 0.12, using dmd 0.125)
 Copyright (C) 2004 Free Software Foundation, Inc.
 This is free software; see the source for copying conditions.  There is NO
 warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

After going through the hassle of sorting out all the problems by myself, I found that someone else had already written a webpage on how to complete this (http://www.ht-lab.com/howto/sccygwin/sccygwin.html).

Anyway, here it goes, with my own comments about optimizing:
  * add gcc, g++ and make to your cygwin installation (from cygwin setup)
  * download systemc-2.2.0.tgz and untar it to a `<your_dir>` (be careful to use the cygwin tar and not a windows tool)
  * go to `<your_dir>/systemc-2.2.0`
  * create a subdirectory called "objdir" (`<your_dir>/systemc-2.2.0/objdir`)
  * edit the file `<your_dir>/systemc-2.2.0/src/sysc/kernel/sc_constants.h` and change:
```
 Line 57: const int SC_DEFAULT_STACK_SIZE   = 0x10000;
```
> to
```
 Line 57: const int SC_DEFAULT_STACK_SIZE   = 0x50000;
```

  * edit the file `<your_dir>/systemc-2.2.0/src/systemc.h` and change:
```
 Line 175:       using std::wctomb;
 Line 176:       using std::mbstowcs;
 Line 177:       using std::wcstombs;
```
> to
```
 Line 175:       //using std::wctomb;
 Line 176:       using std::mbstowcs;
 Line 177:       //using std::wcstombs;
```
  * run the following command:
```
 ../configure --prefix=<your_installdir>
```
> In my case, I never install SystemC globally but rather install a version for every project that requires it.  Therefore, my `<your_installdir>` corresponds to a project and when I need it, I add TLM in the same `<your_installdir>`.

  * finally, compile (not using pthreads as suggested by the other webpage, as it slows down by a factor 10 the simulation):
```
 make opt
```

  * run the tests
```
 make check
```