/*++

Copyright (c) 1990-2000  Microsoft Corporation

Module Name:

    queue.c

Abstract:

    Queue handling for serial port I/O driver.
    Processes WriteFile requests to transmit data via serial port.

--*/

#include "driver.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, SerioQueueInitialize)
#pragma alloc_text (PAGE, SerioEvtIoWrite)
#endif

// UART Line Status Register (LSR) bits
#define LSR_THRE    0x20    // Transmitter Holding Register Empty
#define LSR_TSRE    0x40    // Transmitter Shift Register Empty

// Maximum attempts for transmitter polling
#define MAX_TX_ATTEMPTS     100
#define TX_POLL_DELAY       1   // microseconds

NTSTATUS
SerioQueueInitialize(
    __in WDFDEVICE Device
    )
/*++

Routine Description:

    The I/O dispatch callbacks for the framework device object
    are configured in this function.

    A single default I/O Queue is configured for sequential request
    processing.

Arguments:

    Device - Handle to a framework device object.

Return Value:

    NTSTATUS

--*/
{
    WDFQUEUE queue;
    NTSTATUS status;
    WDF_IO_QUEUE_CONFIG queueConfig;

    PAGED_CODE();

    //
    // Configure a default queue for sequential request processing
    //
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(
        &queueConfig,
        WdfIoQueueDispatchSequential
        );

    //
    // Register WriteFile handler
    //
    queueConfig.EvtIoWrite = SerioEvtIoWrite;

    status = WdfIoQueueCreate(
                 Device,
                 &queueConfig,
                 WDF_NO_OBJECT_ATTRIBUTES,
                 &queue
                 );

    if (!NT_SUCCESS(status)) {
        KdPrint(("WdfIoQueueCreate failed 0x%x\n", status));
        return status;
    }

    return status;
}

VOID
SerioEvtIoWrite(
    __in WDFQUEUE     Queue,
    __in WDFREQUEST   Request,
    __in size_t       Length
    )
/*++

Routine Description:

    This event is invoked when the framework receives IRP_MJ_WRITE requests.
    This handler transmits bytes to the serial port with cyclic polling
    of transmitter readiness.

Arguments:

    Queue - Handle to the I/O queue object that is associated with the
            I/O request.

    Request - Handle to a framework request object.

    Length - The number of bytes to be written. The request will contain
            the user input buffer at Irp->AssociatedIrp.SystemBuffer.

Return Value:

    VOID

--*/
{
    PDEVICE_CONTEXT devContext = NULL;
    PUCHAR pBuffer = NULL;
    NTSTATUS status = STATUS_SUCCESS;
    WDFDEVICE device;
    size_t bytesWritten = 0;
    UCHAR lsr;
    UCHAR txByte;
    int attempts;

    device = WdfIoQueueGetDevice(Queue);
    devContext = SerioGetDeviceContext(device);

    PAGED_CODE();

    //
    // Get the input buffer
    //
    if (Length == 0) {
        status = STATUS_INVALID_PARAMETER;
        goto exit;
    }

    status = WdfRequestRetrieveInputBuffer(Request, Length, &pBuffer, NULL);
    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    //
    // Attempt to transmit the first byte of the buffer
    // using cyclic polling of transmitter readiness
    //
    txByte = pBuffer[0];
    attempts = 0;

    // Log the byte we're about to transmit
    KdPrint(("SerioEvtIoWrite: transmit start - byte=0x%02X, max_attempts=%d\n", txByte, MAX_TX_ATTEMPTS));

    while (attempts < MAX_TX_ATTEMPTS) {
        //
        // Read Line Status Register to check transmitter readiness
        // LSR at offset 5 from base address
        //
        lsr = READ_PORT_UCHAR(
            (PUCHAR)((ULONG_PTR)devContext->PortBase + 5)
        );

        // Log the attempt number and LSR value
        KdPrint(("SerioEvtIoWrite: attempt=%d, LSR=0x%02X\n", attempts + 1, lsr));

        //
        // Check Transmitter Holding Register Empty (THRE) bit
        //
        if (lsr & LSR_THRE) {
            //
            // Transmitter is ready - write the byte
            //
            WRITE_PORT_UCHAR(
                (PUCHAR)((ULONG_PTR)devContext->PortBase + 0),
                txByte
            );

            bytesWritten = 1;
            KdPrint(("SerioEvtIoWrite: Byte 0x%02X transmitted\n", txByte));
            break;
        }

        attempts++;
        
        //
        // Small delay before next attempt
        //
        KeStallExecutionProcessor(TX_POLL_DELAY);
    }

    if (attempts >= MAX_TX_ATTEMPTS) {
        //
        // Transmitter not ready after polling - return 0 bytes written
        //
        KdPrint(("SerioEvtIoWrite: Transmitter not ready (timeout)\n"));
        bytesWritten = 0;
    }

exit:
    //
    // Complete the request with number of bytes written
    //
    WdfRequestCompleteWithInformation(Request, status, bytesWritten);
}

