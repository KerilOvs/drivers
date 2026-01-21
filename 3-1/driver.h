/*++

Copyright (c) 1990-2000  Microsoft Corporation

Module Name:

    driver.h

Abstract:

    Serial Port I/O Driver Header. 
    This driver supports WriteFile API for transmitting data to serial port
    with cyclic polling of transmitter readiness.

--*/

#define INITGUID

#include <ntddk.h>
#include <wdf.h>

#include "device.h"
#include "queue.h"

#define SERIO_DEVICE_NAME       L"\\Device\\SerialPort0"
#define SERIO_TYPE              40001
#define SERIO_DOS_DEVICE_NAME   L"\\DosDevices\\SerialPort"

//
// WDFDRIVER Events
//

DRIVER_INITIALIZE DriverEntry;

EVT_WDF_DRIVER_DEVICE_ADD SerioEvtDeviceAdd;
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, SerioGetDeviceContext)
