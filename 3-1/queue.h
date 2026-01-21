/*++

Copyright (c) 1990-2000  Microsoft Corporation

Module Name:

    queue.h

Abstract:

    Queue handling header for serial port driver.

--*/

NTSTATUS
SerioQueueInitialize(
    __in WDFDEVICE hDevice
    );

//
// Events from the IoQueue object
//
EVT_WDF_IO_QUEUE_IO_WRITE SerioEvtIoWrite;

