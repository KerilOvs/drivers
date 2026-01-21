/*++

Copyright (c) 1990-2000  Microsoft Corporation

Module Name:

    device.h

Abstract:

    Device handling header for serial port driver.

--*/

//
// The device context holds driver specific information
//
typedef struct _DEVICE_CONTEXT
{
    PVOID PortBase;             // base port address for serial port
    ULONG PortCount;            // Count of I/O addresses used
    ULONG PortMemoryType;       // 0=Memory space, 1=I/O space
    BOOLEAN PortWasMapped;      // If TRUE, we must unmap on unload
    ULONG BaudRate;             // Baud rate setting
    UCHAR DataBits;             // Data bits (8)
    UCHAR StopBits;             // Stop bits (1)
    UCHAR Parity;               // Parity setting (0=none)
} DEVICE_CONTEXT, *PDEVICE_CONTEXT;

//
// Function to initialize the device and its callbacks
//
NTSTATUS
SerioDeviceCreate(
    PWDFDEVICE_INIT DeviceInit
    );

//
// Device events
//
EVT_WDF_DEVICE_PREPARE_HARDWARE SerioEvtDevicePrepareHardware;
EVT_WDF_DEVICE_RELEASE_HARDWARE SerioEvtDeviceReleaseHardware;

