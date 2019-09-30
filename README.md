# XiaoJSoft Modbus Stack (over Serial Line) for uC/OS-III

## Overview

*XiaoJSoft Modbus Stack (over Serial Line)* for *uC/OS-III* (hereinafter referred to briefly as “*XMS for uC/OS-III*”, “*this software*”, or just “*XMS*”), a Modbus stack with maximized specification compatibility and customizable functionality, is developed by XiaoJSoft Studio for communicating with Modbus protocol in a wide range of embedded system applications (e.g. industrial automation, IoT).

This software is carefully designed and implemented to be compatible with the [Modbus specifications](http://www.modbus.org/specs.php) as much as possible. Also, all development was done in a “clean room” with no developer reads the document or source code of any other similar products (e.g. [uC/Modbus](https://www.micrium.com/rtos/modbus/), [FreeModbus](https://www.embedded-solutions.at/en/freemodbus/) and [LibModbus](https://libmodbus.org/)).

This software is built for [*uC/OS-III*](https://www.micrium.com/rtos/kernels/) platform and depends on *uC/CPU* and *uC/LIB* which are also products of [*Micrium*](https://www.micrium.com/). This software is generally distributed without these packages, you (or the developer) have to obtain these packages from *Micrium* to let everything work.

## Standard Compatibility

This software is designed to following specifications:

 - MODBUS over Serial Line Specification and Implementation Guide V1.02 ([*Modbus_over_serial_line_V1_02.pdf*](http://www.modbus.org/docs/Modbus_over_serial_line_V1_02.pdf))
 - MODBUS Application Protocol Specification V1.1b3 ([*Modbus_Application_Protocol_V1_1b3.pdf*](http://www.modbus.org/docs/Modbus_Application_Protocol_V1_1b3.pdf))

Currently, this software contains built-in support for following function codes on both Modbus Master and Slave:

 - Read Coils (0x01)
 - Read Discrete Inputs (0x02)
 - Read Holding Registers (0x03)
 - Read Input Registers (0x04)
 - Write Single Coil (0x05)
 - Write Single Register (0x06)
 - Write Multiple Coils (0x0F)
 - Write Multiple Registers (0x10)
 - Mask Write Register (0x16)
 - Read/Write Multiple Registers (0x17)

On Slave, it is allowed to customize supported function codes for each Modbus serial interface. And additional programming interfaces are provided for supporting other standard or non-standard function codes. Furthermore, this software accesses your Modbus data model through callbacks so that you can design and implement the data model freely as you want.

On Master, additional interfaces are also provided for supporting other function codes.

Following standardized counters are implemented in this software:

 - Bus Message Count (Slave)
 - Bus Communication Error Count (Slave)
 - Slave Exception Error Count (Slave)
 - Slave Message Count (Slave)
 - Slave No Response Count (Slave)

Following non-standardized counters are also implemented:
 - Serial Parity Error Count (Master + Slave)
 - Serial Data OverRun Error Count (Master + Slave)
 - Serial Frame Error Count (Master + Slave)

Following transmission modes are supported by this software:
 - RTU
 - ASCII

To support low-speed physical layer (such as [*Satellite communication*](https://en.wikipedia.org/wiki/Communications_satellite)), we implemented the “*Character Time Prescaler*” feature for RTU transmission mode to help you override the default 1.5 and 3.5 character time.

## Requirements

This software requires following software packages:

|    Name   |   Version  |  Vendor |    Description    |
|:---------:|:----------:|:-------:|:-----------------:|
| uC/OS-III | >= 3.03.00 | Micrium | uC/OS-III kernel. |
|   uC/LIB  | >= 1.38.02 | Micrium |   uC/LIB module.  |
|   uC/CPU  | >= 1.31.02 | Micrium |   uC/CPU module.  |

And this software would use following hardware resources:

|         Name         |                                                                     Amount                                                                    |                                                                                                                                                          Description                                                                                                                                                          |
|:--------------------:|:---------------------------------------------------------------------------------------------------------------------------------------------:|:-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------:|
| High-precision timer |                                                            1 per Modbus interface.                                                            | In RTU transmission mode, we need to use high-precision timer for monitoring 1.5 and 3.5 character time.<br> The minimum of the timer resolution should be lower than or equal to 250us and the maximum allowed timespan should not be lower than 20ms.                                                                       |
|       RAM space      | Slave (estimation):<br> (252 * 2 + 128) bytes per Modbus interface.<br> <br> Master (estimation):<br> (252 + 128) bytes per Modbus interface. | On Slave, RX/TX buffers are needed, the length of each of them should be 252 bytes. Other more 128 bytes are for other usages such as stack and static data.<br> On Master, only one buffer is needed and the length of it should also be 252 bytes. Other more 128 bytes are for other usages such as stack and static data. |
|       ROM space      |                                                                   1KB ~ 10KB                                                                  | The program size (which affects the ROM usage) may be slightly different in different MCUs. So it is hard to estimate. The value shown in left comes from our experience.                                                                                                                                                     |

## Documents

See *Docs* directory for the documents.

## Example Projects

We have an example project based on [*Arduino Mega2560 R3*](https://store.arduino.cc/usa/mega-2560-r3) board. But due to the copyright limitation, we could not distribute the project files freely. Feel free to contact us if you really need it.

 - If you are a student or a researcher, and you just want to learn this project or do some peaceful research, we would provide the example project to you.
 - If you are a geek and want to embed this software in your home-made non-commercial project, we would also provide the example project to you. 
 - If you want to embed this software in your commercial product, you must provide us a legal license authorized by *Micrium*. After that, we would provide the example project to you.

No purchase is needed for the example project.

## Commercial support

We provide commercial support for this software. You may contact us when:

 - Need any help.
 - Need to customize the software.
 - Need any other techical support.

## Contact us

Project Leader:
 - Ji WenCong &lt;[taiki_akita@163.com](mailto://taiki_akita@163.com)&gt;

