/*++

Copyright (c) 2024 Serial Port Driver

Module Name:

    write_serial.c

Abstract:

    User-mode application for transmitting data through serial port driver
    using WriteFile API. Implements cyclic polling for transmitter readiness.

--*/

#include <windows.h>
#include <stdio.h>
#include <string.h>

#define DEVICE_PATH "\\\\.\\SerialPort"
#define MAX_TX_ATTEMPTS 100
#define TX_POLL_DELAY 10  // milliseconds

int __cdecl main(int argc, char *argv[])
{
    HANDLE hDevice = INVALID_HANDLE_VALUE;
    DWORD dwBytesWritten = 0;
    DWORD i = 0;
    CHAR strData[256] = "Hello, Serial Port!";
    DWORD dwDataLen = 0;
    CHAR cByte;
    int nAttempts = 0;

    if (argc > 1) {
        strcpy_s(strData, sizeof(strData), argv[1]);
    }

    dwDataLen = (DWORD)strlen(strData);

    //
    // Open the device
    //
    hDevice = CreateFile(
        DEVICE_PATH,
        GENERIC_WRITE,
        FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hDevice == INVALID_HANDLE_VALUE) {
        printf("Error: Cannot open device %ws (error: 0x%x)\n", DEVICE_PATH, GetLastError());
        return 1;
    }

    printf("Device opened successfully\n");

    //
    // Transmit each byte of the string
    //
    for (i = 0; i < dwDataLen; i++) {
        cByte = strData[i];
        nAttempts = 0;

        //
        // Retry until byte is transmitted or max attempts reached
        //
        while (nAttempts < MAX_TX_ATTEMPTS) {
            if (WriteFile(
                hDevice,
                &cByte,
                1,
                &dwBytesWritten,
                NULL
            )) {
                if (dwBytesWritten == 1) {
                    printf("Byte %d (0x%02X '%c') transmitted successfully\n", i, cByte, (cByte >= 32 && cByte < 127) ? cByte : '?');
                    break;
                } else {
                    //
                    // Transmitter not ready, retry after delay
                    //
                    nAttempts++;
                    if (nAttempts >= MAX_TX_ATTEMPTS) {
                        printf("Byte %d (0x%02X '%c') transmission timeout (transmitter not ready)\n", i, cByte, (cByte >= 32 && cByte < 127) ? cByte : '?');
                    } else {
                        Sleep(TX_POLL_DELAY);
                    }
                }
            } else {
                printf("Error: WriteFile failed for byte %d (error: 0x%x)\n", i, GetLastError());
                break;
            }
        }
    }

    printf("Transmission complete\n");

    //
    // Close the device
    //
    if (hDevice != INVALID_HANDLE_VALUE) {
        CloseHandle(hDevice);
    }

    return 0;
}

