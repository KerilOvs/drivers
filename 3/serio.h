/*++

Copyright (c) 2024 Serial Port Driver

Module Name:

    serio.h

Abstract:

    Common definitions for Serial Port I/O Driver

--*/

#if     !defined(__SERIO_H__)
#define __SERIO_H__

//
// Device type for serial port driver
//
#define SERIO_TYPE              40001

//
// Serial port base address (COM1 standard)
//
#define COM1_BASE_ADDRESS       0x3F8
#define COM_PORT_COUNT          8

//
// UART Register offsets
//
#define UART_THR                0   // Transmitter Holding Register (W)
#define UART_RBR                0   // Receiver Buffer Register (R)
#define UART_IER                1   // Interrupt Enable Register
#define UART_IIR                2   // Interrupt Identification Register
#define UART_FCR                2   // FIFO Control Register
#define UART_LCR                3   // Line Control Register
#define UART_MCR                4   // Modem Control Register
#define UART_LSR                5   // Line Status Register
#define UART_MSR                6   // Modem Status Register
#define UART_DLL                0   // Divisor Latch Low (when LCR.DLAB=1)
#define UART_DLH                1   // Divisor Latch High (when LCR.DLAB=1)

//
// Line Status Register (LSR) bit definitions
//
#define LSR_DR                  0x01    // Data Ready
#define LSR_OE                  0x02    // Overrun Error
#define LSR_PE                  0x04    // Parity Error
#define LSR_FE                  0x08    // Framing Error
#define LSR_BI                  0x10    // Break Interrupt
#define LSR_THRE                0x20    // Transmitter Holding Register Empty
#define LSR_TSRE                0x40    // Transmitter Shift Register Empty
#define LSR_FIFOE               0x80    // FIFO Error

//
// Line Control Register (LCR) bit definitions
//
#define LCR_DLAB                0x80    // Divisor Latch Access Bit
#define LCR_SB                  0x40    // Set Break
#define LCR_SP                  0x20    // Stick Parity
#define LCR_EPS                 0x10    // Even Parity Select
#define LCR_PEN                 0x08    // Parity Enable
#define LCR_STB                 0x04    // Stop Bits
#define LCR_WLS_5BITS           0x00    // 5 Data Bits
#define LCR_WLS_6BITS           0x01    // 6 Data Bits
#define LCR_WLS_7BITS           0x02    // 7 Data Bits
#define LCR_WLS_8BITS           0x03    // 8 Data Bits
#define LCR_STOP_1BIT           0x00    // 1 Stop Bit
#define LCR_STOP_2BITS          0x04    // 2 Stop Bits
#define LCR_PARITY_NONE         0x00    // No Parity
#define LCR_PARITY_ODD          0x08    // Odd Parity
#define LCR_PARITY_EVEN         0x18    // Even Parity

//
// Modem Control Register (MCR) bit definitions
//
#define MCR_DTR                 0x01    // Data Terminal Ready
#define MCR_RTS                 0x02    // Request To Send
#define MCR_OUT1                0x04    // Output 1
#define MCR_OUT2                0x08    // Output 2 (Interrupt enable)
#define MCR_LOOPBACK            0x10    // Loopback

//
// Interrupt Enable Register (IER) bit definitions
//
#define IER_ERDAI               0x01    // Enable Received Data Available Interrupt
#define IER_ETHREI              0x02    // Enable Transmitter Holding Register Empty Interrupt
#define IER_ELSI                0x04    // Enable Line Status Interrupt
#define IER_EMSI                0x08    // Enable Modem Status Interrupt

#endif // __SERIO_H__

