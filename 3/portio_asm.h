/*++

Copyright (c) 2024 Serial Port Driver

Module Name:

    portio_asm.h

Abstract:

    Assembly language helpers for serial port I/O operations.
    This file provides inline assembly implementations for reading and
    writing to I/O ports using x86 IN/OUT instructions.

--*/

#ifndef __PORTIO_ASM_H__
#define __PORTIO_ASM_H__

//
// Platform-specific I/O port access functions
//

#if defined(_M_IX86)  // x86 (32-bit)

//
// Read a single byte from an I/O port using IN instruction
//
__forceinline UCHAR ReadByteFromPort(__in USHORT port)
{
    UCHAR value;
    
    __asm {
        mov dx, port
        in  al, dx
        mov value, al
    }
    
    return value;
}

//
// Write a single byte to an I/O port using OUT instruction
//
__forceinline void WriteByteToPort(__in USHORT port, __in UCHAR value)
{
    __asm {
        mov dx, port
        mov al, value
        out dx, al
    }
}

//
// Read a word (USHORT) from an I/O port using IN instruction
//
__forceinline USHORT ReadWordFromPort(__in USHORT port)
{
    USHORT value;
    
    __asm {
        mov dx, port
        in  ax, dx
        mov value, ax
    }
    
    return value;
}

//
// Write a word (USHORT) to an I/O port using OUT instruction
//
__forceinline void WriteWordToPort(__in USHORT port, __in USHORT value)
{
    __asm {
        mov dx, port
        mov ax, value
        out dx, ax
    }
}

//
// Read a dword (ULONG) from an I/O port using IN instruction
//
__forceinline ULONG ReadDwordFromPort(__in USHORT port)
{
    ULONG value;
    
    __asm {
        mov dx, port
        in  eax, dx
        mov value, eax
    }
    
    return value;
}

//
// Write a dword (ULONG) to an I/O port using OUT instruction
//
__forceinline void WriteDwordToPort(__in USHORT port, __in ULONG value)
{
    __asm {
        mov dx, port
        mov eax, value
        out dx, eax
    }
}

//
// Read Line Status Register (LSR) for serial port
// Returns LSR value with status bits
//
__forceinline UCHAR ReadSerialLSR(__in USHORT port_base)
{
    // LSR is at offset 5 from base address
    return ReadByteFromPort(port_base + 5);
}

//
// Write byte to Transmitter Holding Register (THR)
//
__forceinline void WriteByteToTHR(__in USHORT port_base, __in UCHAR byte)
{
    // THR is at offset 0 from base address
    WriteByteToPort(port_base, byte);
}

//
// Check if Transmitter Holding Register is empty (THRE bit in LSR)
//
__forceinline BOOLEAN IsTransmitterReady(__in USHORT port_base)
{
    UCHAR lsr = ReadSerialLSR(port_base);
    return (lsr & 0x20) ? TRUE : FALSE;  // THRE bit is bit 5
}

//
// Serial port polling transmission - try to transmit byte
// Returns: TRUE if transmitted, FALSE if not ready
//
__forceinline BOOLEAN TryTransmitByte(__in USHORT port_base, __in UCHAR byte)
{
    if (IsTransmitterReady(port_base)) {
        WriteByteToTHR(port_base, byte);
        return TRUE;
    }
    return FALSE;
}

#elif defined(_M_AMD64)  // x86-64 (64-bit)

//
// For x86-64, use the Windows DDK macros instead of inline asm
// (inline assembly is not supported in x64 code)
//

#define ReadByteFromPort(port) READ_PORT_UCHAR((PUCHAR)(ULONG_PTR)(port))
#define WriteByteToPort(port, value) WRITE_PORT_UCHAR((PUCHAR)(ULONG_PTR)(port), (value))
#define ReadWordFromPort(port) READ_PORT_USHORT((PUSHORT)(ULONG_PTR)(port))
#define WriteWordToPort(port, value) WRITE_PORT_USHORT((PUSHORT)(ULONG_PTR)(port), (value))
#define ReadDwordFromPort(port) READ_PORT_ULONG((PULONG)(ULONG_PTR)(port))
#define WriteDwordToPort(port, value) WRITE_PORT_ULONG((PULONG)(ULONG_PTR)(port), (value))

__forceinline UCHAR ReadSerialLSR(__in USHORT port_base)
{
    return READ_PORT_UCHAR((PUCHAR)(ULONG_PTR)(port_base + 5));
}

__forceinline void WriteByteToTHR(__in USHORT port_base, __in UCHAR byte)
{
    WRITE_PORT_UCHAR((PUCHAR)(ULONG_PTR)port_base, byte);
}

__forceinline BOOLEAN IsTransmitterReady(__in USHORT port_base)
{
    UCHAR lsr = ReadSerialLSR(port_base);
    return (lsr & 0x20) ? TRUE : FALSE;
}

__forceinline BOOLEAN TryTransmitByte(__in USHORT port_base, __in UCHAR byte)
{
    if (IsTransmitterReady(port_base)) {
        WriteByteToTHR(port_base, byte);
        return TRUE;
    }
    return FALSE;
}

#endif  // Platform selection

#endif  // __PORTIO_ASM_H__

