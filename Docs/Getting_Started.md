# Getting Started

This software provides services to application code in the form of a set of functions that perform specific operations.

In this document, the reader will get an overall understanding of this software. It is assumed that the project setup (files and directories) is as described in other documents, and that a *C compiler* exists for the hardware platform that is in use. However, this document makes no assumptions about the tools, drivers or the hardware platform that is used.

## Initialization

First of all, include *&lt;mb.h&gt;* first:

```
#include <mb.h>
```

After the operating system initialized, you can call *MB_Initialize()* to initialize this software:

```
MB_ERROR  error;
MB_Initialize(&(error));
```

It is highly recommend to check the error code returned by every function:

```
if (error != MB_ERROR_NONE) {
    ... Error handling codes ...
}
```

Error checks would be omitted in next several sections in this document to keep the document simple . But you shall not omit them in your code.

## Register a device

After initialization, you have to register a device by calling *MB_RegisterDevice()* before opening and using it, then the function would return a Modbus device interface ID to your application:

```
MB_IFINDEX  iface;
iface = MB_RegisterDevice(
    &(MBDRV_USART_DRIVERDESC),  /*  Make a pointer to your driver.  */
    &(error)
);
```

## Open a device

After registered a device, you can open it by calling *MB_OpenDevice()*. In this example, the device is opened in RTU transmission mode, 9600 bps serial baudrate, 8 serial data bits, no serial parity bit and 1 serial stop bit.

Make a *MB_SERIAL_SETUP* object first:
```
MB_SERIAL_SETUP  serialsetup;
serialsetup.baudrate = MB_SERIAL_BAUDRATE_9600;
serialsetup.parity   = MB_SERIAL_PARITY_NONE;
serialsetup.dataBits = MB_SERIAL_DATABITS_8;
serialsetup.stopBits = MB_SERIAL_STOPBITS_1;
```

And then open the device:

```
MB_OpenDevice(
    iface,
    MB_TRMODE_RTU,    /*  Transmission mode.  */,
    &(serialsetup),
    &(error)
);
```

## Devices working as Slave

In this section, we would make a Modbus Slave node with supporting of following function codes:

 - Read Coils (0x01)
 - Write Multiple Coils (0x0F)

For simplicity, the Modbus data model is extremely simple. It has only one coil at address 0x0000 and it is not connected to any hardware port (like *GPIO*) in this example.

### Make callbacks

This software implements Slave nodes with a callback pattern, which you have to implement your Modbus data model through callbacks.

In this example, you need three callbacks. The first one is to validate whether a coil address is valid:

```
static CPU_BOOLEAN  MBApp_ValidateCoil(
    CPU_INT16U  address,
    void       *p_arg,
    MB_ERROR   *p_error
) {
    /*  No error by default.  */
    *p_error = MB_ERROR_NONE;

    /*  Check the address.  */
    if (address == (CPU_INT16U)0x0000U) {
        return DEF_YES;
    } else {
        return DEF_NO;
    }
}
```

The second one is to read a coil:

```
static CPU_BOOLEAN  g_MBApp_Coil0 = DEF_NO;

static CPU_BOOLEAN  MBApp_ReadCoil(
    CPU_INT16U  address,
    void       *p_arg,
    MB_ERROR   *p_error
) {
    /*  No error by default.  */
    *p_error = MB_ERROR_NONE;

    switch (address) {
        case (CPU_INT16U)0x0000:
            return g_MBApp_Coil0;
        default:
            *p_error = MB_ERROR_INVALIDPARAMETER;
            return DEF_NO;
    }
}
```

The last one is to write a coil:

```
static void MBApp_WriteCoil(
    CPU_INT16U   address,
    CPU_BOOLEAN  value,
    void        *p_arg,
    MB_ERROR    *p_error
) {
    /*  No error by default.  */
    *p_error = MB_ERROR_NONE;

    switch (address) {
        case (CPU_INT16U)0x0000:
            g_MBApp_Coil0 = value;
        default:
            *p_error = MB_ERROR_INVALIDPARAMETER;
            return;
    }
}
```

### Make the command table

Since all callbacks are ready, let's make a command table (*MBSLAVE_CMDTABLE*) object which is used by the Slave to lookup and process function codes by calling *MBSlave_CmdTable_Initialize()* function:

```
MBSLAVE_CMDTABLE   g_MBApp_SlaveCmdTable;   /*  The command table may be very large, so it's better to put it to global context.  */
...
MBSlave_CmdTable_Initialize(
    &(g_MBApp_SlaveCmdTable),
    &(error)
);
```

Register function codes supported on the device:

```
MBSLAVE_READCOILS_CTX      mbscmd_01;
mbscmd_01.cbValidateCoil = MBApp_ValidateCoil;
mbscmd_01.cbReadCoil     = MBApp_ReadCoil;
mbscmd_01.cbArg          = (void*)0;
MBSlave_CmdTable_Add(
    &(g_MBApp_SlaveCmdTable),
    MB_FNCODE_READCOILS,
    MBSlave_CmdLet_ReadCoils,         /*  This is the built-in 0x01 function code processor.  */
    &(mbscmd_01),
    DEF_YES,                          /*  Disable this function code for broadcast requests.  */
    DEF_YES,                          /*  Disable this function code for listen-only mode.  */
    &(error)
);
```

```
MBSLAVE_WRITEMULTIPLECOILS_CTX  mbscmd_02;
mbscmd_02.cbValidateCoil      = MBApp_ValidateCoil;
mbscmd_02.cbWriteCoil         = MBApp_WriteCoil;
mbscmd_02.cbArg               = (void*)0;
MBSlave_CmdTable_Add(
    &(g_MBApp_SlaveCmdTable),
    MB_FNCODE_WRITEMULTIPLECOILS,
    MBSlave_CmdLet_WriteMultipleCoils,   /*  This is the built-in 0x0F function code processor.  */
    &(mbscmd_02),
    DEF_YES,                             /*  Disable this function code for broadcast requests.  */
    DEF_YES,                             /*  Disable this function code for listen-only mode.  */
    &(error)
);
```

Note that unlike Master nodes, all callbacks in *MBSLAVE_\*_CTX* must be implemented.

### Create a Slave object

Now you can create a Slave (*MBSLAVE*) object now by using *MBSlave_Initialize()* function:

```
CPU_INT08U   g_MBApp_SlaveRcvBuf[252];
CPU_INT08U   g_MBApp_SlaveSndBuf[252];
...
MBSLAVE   slave;
MBSlave_Initialize(
    &(slave),
    &(g_MBApp_SlaveCmdTable),
    iface,
    g_MBApp_SlaveRcvBuf,
    sizeof(g_MBApp_SlaveRcvBuf),
    g_MBApp_SlaveSndBuf,
    sizeof(g_MBApp_SlaveSndBuf),
    &(error)
);
```

The Slave object is created with slave address settled to 0 by default. It could only handle broadcast requests unless you set the slave address explicitly with *MBSlave_SetAddress()* function:

```
MBSlave_SetAddress(
    &(slave),
    (CPU_INT08U)1U,     /*  Set slave address to 1.  */,
    &(error)
);
```

### Poll requests

Now we have prepared everything, now the slave can poll requests from the master node:

```
while(1) {
    MBSlave_Poll(
        &(slave),
        (MB_TIMESPAN)0U,     /*  Wait infinitely.  */
        &(error)
    );
}
```

Note that the requests are handled automatically within *MBSlave_Poll()* function.

## Devices working as Master

In this section, we would make a Modbus Master node which polls the state of the coil at address 0x0000 in Slave whose address is 1.

### Make callbacks

This software implements Master nodes with a partial callback pattern, which you have to implement your response handling functions.

In this example, we just need one callback:

```
static void MBApp_MasterCoilValue(
    CPU_INT16U   address,
    CPU_BOOLEAN  value,
    void        *p_arg,
    MB_ERROR    *p_error
) {
    /*  No error by default.  */
    *p_error = MB_ERROR_NONE;

    switch (address) {
        case (CPU_INT16U)0x0000U:
            /*  ... Handle 'value' properly here ...  */
            break;
        default:
            *p_error = MB_ERROR_INVALIDPARAMETER;
            return;
    }
}
```

### Make a request object

Make a request object (*MBMASTER_CMDLET_READCOILS_REQUEST* in this example) and fill request parameters:

```
MBMASTER_CMDLET_READCOILS_REQUEST  request;
request.coilStartAddress = (CPU_INT16U)0x0000U;
request.coilQuantity     = (CPU_INT16U)1U;
```

### Make a response object

Make a response object (*MBMASTER_CMDLET_READCOILS_RESPONSE* in this example) and fill response callbacks:

```
MBMASTER_CMDLET_READCOILS_RESPONSE  response;
response.cbException       = (MBMASTER_EXCEPTION_CB)0;
response.cbCoilValueStart  = (MBMASTER_COILVALUE_START_CB)0;
response.cbCoilValueUpdate = MBApp_MasterCoilValue;
response.cbCoilValueEnd    = (MBMASTER_COILVALUE_END_CB)0;
```

Unlike the Slave, all callbacks are optional. If specific callback is not needed, it must be written to NULL to disable.

### Create a Master object

Create a *MBMASTER* object before posting the request:

```
CPU_INT08U   g_MBApp_MasterBuf[252];

MBMASTER   master;
MBMaster_Initialize(
    &(master),
    iface,
    g_MBApp_MasterBuf,
    sizeof(g_MBApp_MasterBuf),
    &(error)
);
```

### Post the request

Now let's post the request:

```
MBMaster_Post(
    &(master),
    (CPU_INT08U)1U,                        /*  Post to slave whose address is 1.  */
    MBMASTER_CMDLETDESCRIPTOR_READCOILS,   /*  This is the built-in 0x01 function code support.  */
    &(request),
    &(response),
    (void*)0,
    (MB_TIMESPAN)1000U,                    /*  Timeout time (unit: milliseconds).  */
    &(error)
);
```

## Close a device

If a device is not used any more, you may close it:

```
MB_CloseDevice(
    iface,
    &(error)
);
```

