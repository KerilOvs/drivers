/*++

Copyright (c) 1990-2000  Microsoft Corporation

Module Name:

    device.c - Device handling events for serial port driver.

Abstract:

    Serial Port I/O Driver with WriteFile support for data transmission.

--*/

#include "driver.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, SerioDeviceCreate)
#pragma alloc_text (PAGE, SerioEvtDevicePrepareHardware)
#pragma alloc_text (PAGE, SerioEvtDeviceReleaseHardware)
#endif

// Serial port I/O base address for COM1 (standard)
#define COM1_BASE_ADDRESS       0x3F8
#define COM_PORT_COUNT          8

// Serial port register offsets
#define UART_THR                0   // Transmitter Holding Register
#define UART_RBR                0   // Receiver Buffer Register
#define UART_IER                1   // Interrupt Enable Register
#define UART_IIR                2   // Interrupt Identification Register
#define UART_FCR                2   // FIFO Control Register
#define UART_LCR                3   // Line Control Register
#define UART_MCR                4   // Modem Control Register
#define UART_LSR                5   // Line Status Register
#define UART_MSR                6   // Modem Status Register
#define UART_DLL                0   // Divisor Latch Low
#define UART_DLH                1   // Divisor Latch High

// Line Status Register (LSR) bits
#define LSR_THRE                0x20    // Transmitter Holding Register Empty

// Line Control Register (LCR) bits
#define LCR_DLAB                0x80    // Divisor Latch Access Bit
#define LCR_WLS_8BITS           0x03    // 8 data bits
#define LCR_STOP_1BIT           0x00    // 1 stop bit
#define LCR_PARITY_NONE         0x00    // No parity


NTSTATUS
SerioDeviceCreate(
    PWDFDEVICE_INIT DeviceInit
    )
/*++

Routine Description:

    Worker routine called to create a device and its software resources.

Arguments:

    DeviceInit - Pointer to an opaque init structure. Memory for this
        structure will be freed by the framework when the WdfDeviceCreate
        succeeds.

Return Value:

    NTSTATUS

--*/
{
    WDF_OBJECT_ATTRIBUTES           deviceAttributes;
    PDEVICE_CONTEXT                 deviceContext;
    WDF_PNPPOWER_EVENT_CALLBACKS    pnpPowerCallbacks;
    WDFDEVICE                       device;
    NTSTATUS                        status;
    UNICODE_STRING                  ntDeviceName;
    UNICODE_STRING                  win32DeviceName;
    WDF_FILEOBJECT_CONFIG           fileConfig;
    
    PAGED_CODE();
    
    WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&pnpPowerCallbacks);

    //
    // Register pnp/power callbacks for hardware initialization and cleanup
    //
    pnpPowerCallbacks.EvtDevicePrepareHardware = SerioEvtDevicePrepareHardware;
    pnpPowerCallbacks.EvtDeviceReleaseHardware = SerioEvtDeviceReleaseHardware;
    
    //
    // Register the PnP and power callbacks
    //
    WdfDeviceInitSetPnpPowerEventCallbacks(DeviceInit, &pnpPowerCallbacks);

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&deviceAttributes, DEVICE_CONTEXT);    

    WDF_FILEOBJECT_CONFIG_INIT(
                    &fileConfig,
                    WDF_NO_EVENT_CALLBACK, 
                    WDF_NO_EVENT_CALLBACK, 
                    WDF_NO_EVENT_CALLBACK
                    );
    
    fileConfig.AutoForwardCleanupClose = WdfFalse;
    
    WdfDeviceInitSetFileObjectConfig(DeviceInit,
                                     &fileConfig,
                                     WDF_NO_OBJECT_ATTRIBUTES);
    //
    // Create a named device object
    //
    RtlInitUnicodeString(&ntDeviceName, SERIO_DEVICE_NAME);
    
    status = WdfDeviceInitAssignName(DeviceInit, &ntDeviceName);
    if (!NT_SUCCESS(status)) {
        return status;
    }
    
    WdfDeviceInitSetDeviceType(DeviceInit, SERIO_TYPE);

    //
    // Device is not holding paging file
    //
    WdfDeviceInitSetPowerPageable(DeviceInit);

    status = WdfDeviceCreate(&DeviceInit, &deviceAttributes, &device);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    //
    // Get the device context and initialize it
    //
    deviceContext = SerioGetDeviceContext(device);

    //
    // Serial port address is in I/O space
    //
    deviceContext->PortMemoryType = 1;
    deviceContext->PortCount = COM_PORT_COUNT;
    deviceContext->BaudRate = 9600;
    deviceContext->DataBits = 8;
    deviceContext->StopBits = 1;
    deviceContext->Parity = 0;

    //
    // Create symbolic link for user-mode access
    //
    RtlInitUnicodeString(&win32DeviceName, SERIO_DOS_DEVICE_NAME);
    
    status = WdfDeviceCreateSymbolicLink(
                device,
                &win32DeviceName);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    //
    // Initialize the I/O Package and Queues
    //
    status = SerioQueueInitialize(device);

    return status;
}

NTSTATUS
SerioEvtDevicePrepareHardware(
    __in  WDFDEVICE    Device,
    __in  WDFCMRESLIST ResourceList,
    __in  WDFCMRESLIST ResourceListTranslated
    )
/*++

Routine Description:

    In this callback, the driver does whatever is necessary to make the
    hardware ready to use.  In the case of a USB device, this involves
    reading and selecting descriptors.

Arguments:

    Device - handle to a device

    ResourceList - handle to a resource list containing the raw resources
        found by the PnP manager for this device

    ResourceListTranslated - handle to a resource list containing the
        translated resources found by the PnP manager for this device

Return Value:

    NTSTATUS

--*/
{
    PDEVICE_CONTEXT deviceContext = NULL;
    NTSTATUS status = STATUS_SUCCESS;

    UNREFERENCED_PARAMETER(ResourceList);
    UNREFERENCED_PARAMETER(ResourceListTranslated);

    PAGED_CODE();

    deviceContext = SerioGetDeviceContext(Device);

    //
    // Set up serial port base address
    // For real hardware, resource mapping would be done here
    // For emulation/testing, use standard COM1 address
    //
    deviceContext->PortBase = (PVOID)(ULONG_PTR)COM1_BASE_ADDRESS;
    deviceContext->PortWasMapped = FALSE;

    KdPrint(("SerioEvtDevicePrepareHardware: Serial port at 0x%p\n", 
             deviceContext->PortBase));

    //
    // Initialize serial port parameters
    // Set 9600 baud, 8 data bits, 1 stop bit, no parity
    //
    // In a real scenario, this would involve:
    // 1. Setting divisor latch for baud rate
    // 2. Configuring line control register
    // 3. Setting up FIFO and modem control
    //

    return status;
}

NTSTATUS
SerioEvtDeviceReleaseHardware(
    __in  WDFDEVICE    Device,
    __in  WDFCMRESLIST ResourceListTranslated
    )
/*++

Routine Description:

    EvtDeviceReleaseHardware is called by the framework when the device is
    being removed or stopped.

Arguments:

    Device - handle to a device

    ResourceListTranslated - handle to a resource list containing the
        translated resources found by the PnP manager for this device

Return Value:

    NTSTATUS

--*/
{
    PDEVICE_CONTEXT deviceContext = NULL;

    UNREFERENCED_PARAMETER(ResourceListTranslated);

    PAGED_CODE();

    deviceContext = SerioGetDeviceContext(Device);

    if (deviceContext->PortWasMapped) {
        // If port was mapped to memory space, unmap it here
        // MmUnmapIoSpace(deviceContext->PortBase, deviceContext->PortCount);
    }

    KdPrint(("SerioEvtDeviceReleaseHardware: Cleaning up serial port\n"));

    return STATUS_SUCCESS;
}

