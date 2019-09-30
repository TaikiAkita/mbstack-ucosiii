# Porting Guide

This document describes how to port this software to different devices.

Currently, we provide a default implementation that works on all *C* compilers that supports *ANSI C* specification. But the default implementation might not be the best implementation on some hardware platforms.

We highly recommend that you should use the default porting implementation when debugging and use your own implementation in your final production.

## Directories and Files

Following table shows the name of porting implementation files and where (the directory) they should be placed. The file names in **bold** are files you will need to create or modify for your own hardware platform.

| File                | Directory                                         |
|---------------------|---------------------------------------------------|
| mbport_crc16.c      | /Port/Default/                                    |
| mbport_crc16.h      | /Port/Default/                                    |
| mbport_limits.h     | /Port/Default/                                    |
| mbport_lrc.c        | /Port/Default/                                    |
| mbport_lrc.h        | /Port/Default/                                    |
| **mbport_crc16.c**  | /Port/&lt;hardware-platform&gt;/&lt;compiler&gt;/ |
| **mbport_crc16.h**  | /Port/&lt;hardware-platform&gt;/&lt;compiler&gt;/ |
| **mbport_limits.h** | /Port/&lt;hardware-platform&gt;/&lt;compiler&gt;/ |
| **mbport_lrc.c**    | /Port/&lt;hardware-platform&gt;/&lt;compiler&gt;/ |
| **mbport_lrc.h**    | /Port/&lt;hardware-platform&gt;/&lt;compiler&gt;/ |
  
*&lt;hardware-platform&gt;* is the name of the hardware platform that the driver files apply to (e.g. "*MIMXRT1050-EVK*", "*Arduino/Mega2560*").

*&lt;compiler&gt;* is the name of the toolchain (compiler, assembler, linker/locator) used. Each has its own directory because they may have different features that makes them different from one another.

To implement your own port, copy the directory */Port/Default/* to */Port/&lt;hardware-platform&gt;/&lt;compiler&gt;/* and rewrite them.

