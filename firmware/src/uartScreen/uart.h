/***********************************************
 * This is uart command file
 ************************************************/

#ifndef _UART_H
#define _UART_H

#define uartMaxBufferSize 50


#include "common_include.h"
#include <stdio.h>
#include <stdarg.h>



typedef enum
{
    Uart_Init,
    Uart_Receive
}UART_STATE;

typedef struct
{
    /*** uart ******/
    DRV_HANDLE drvUartHandle;
    UART_STATE uartState;
    
    uint8_t uartBuffer[uartMaxBufferSize];
    uint8_t uart_BufferSize;
    
    bool uart_ByteReady;  // one command block receive ready flag

}UART_DATA;

UART_DATA uartData;

bool Uart_Initial();

void Uart_Task(void);

void Uart_WriteByte(uint8_t c);

void Uart_WriteBytes(uint8_t arrayBytes[uartMaxBufferSize],int length);

void Uart_WriteString(uint8_t *string);

void Uart_printf(const uint8_t *fmt,...);

void Uart_Screen_Printf(const uint8_t *fmt,...);

void Uart_Screen_PrintfArray(uint8_t *data, uint8_t length);

#endif