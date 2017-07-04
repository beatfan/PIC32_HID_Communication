#include "uart.h"

uint8_t uartCount;     
uint8_t uartTmp;


/**************** uart ***************************/
void Uart_interrupt(const SYS_MODULE_INDEX index)
{

    //Uart_WriteByte(UartCommandComeReady);
    
    //if(DRV_USART_ReceiverBufferIsEmpty(uartData.drvUartHandle)) return;

    uartTmp = DRV_USART_ReadByte(uartData.drvUartHandle);
    
    
}

void Uart_Screen_Clear(void);
bool Uart_Initial()
{
    uartData.uart_ByteReady = false;
    uartData.uart_BufferSize = 0;
    uartData.uartState = Uart_Init;
    
    /*********** UART *******************/
   uartData.drvUartHandle = DRV_USART_Open( DRV_USART_INDEX_0,DRV_IO_INTENT_EXCLUSIVE );  
    /* Check the USART1 driver handler */  
   if (uartData.drvUartHandle == DRV_HANDLE_INVALID )  
   {   
       return false;  
   }  
   DRV_USART_ByteReceiveCallbackSet(DRV_USART_INDEX_0, (DRV_USART_BYTE_EVENT_HANDLER)Uart_interrupt); 
   
   //clear uart screen
   Uart_Screen_Clear();
   
   return true;
}

void Uart_Task(void)
{
    switch(uartData.uartState)
    {
        case Uart_Init:
            //Uart_Initial();
            uartData.uartState = Uart_Receive;
        break;
        
        case Uart_Receive:
            if(!uartData.uart_ByteReady) return;
            
            uartData.uart_ByteReady = false;
            
            ///MainBD_ReceiveCommandBytes(uartData.uartBuffer,uartData.uart_BufferSize);
            
            
        break;
        
        default:
        break;
    }
}

void Uart_WriteByte(uint8_t c)
{
    while ((DRV_USART_TRANSFER_STATUS_TRANSMIT_FULL & DRV_USART_TransferStatus(uartData.drvUartHandle)) ) ;
        DRV_USART_WriteByte(uartData.drvUartHandle,c);
}

void Uart_WriteBytes(uint8_t arrayBytes[uartMaxBufferSize],int length)
{
    int i =0 ;
    for(i=0;i<length;i++)
    {
        Uart_WriteByte(arrayBytes[i]);
    }
}

void Uart_WriteString(uint8_t *string)
{
    while(*string!='\0')
    {  
        if(*string=='\n')
        {
            Uart_WriteByte(0x0D);
            Uart_WriteByte(0x0A);
        }
        else
            Uart_WriteByte(*string);  
        string++;  
    }  
}

void Uart_printf(const uint8_t *fmt,...)
{
    va_list ap;  
    char string[1024];  
      
    va_start(ap,fmt);  
    vsprintf(string,fmt,ap);
    Uart_WriteString(string);  
    va_end(ap); 
}

void Uart_Screen_WriteString(uint8_t *string)
{
    while(*string!='\0')
    {  
        Uart_WriteByte(*string);  
        string++;  
    }  
}

void Uart_Screen_Clear(void)
{
    uint8_t tmpColor[] = {0xEE ,0x42 ,0x73 ,0xAE ,0xFF ,0xFC ,0xFF ,0xFF };
    Uart_WriteBytes(tmpColor,8);
    
    uint8_t tmp[] = {0xEE ,0x01 ,0xFF ,0xFC ,0xFF ,0xFF };
    Uart_WriteBytes(tmp,6);
}

void Uart_Screen_ClearLine(uint8_t rowIndex1)
{
    uint8_t tmpcolor[] = {0xEE ,0x41 ,0x73 ,0xAE ,0xFF ,0xFC ,0xFF ,0xFF };
    Uart_WriteBytes(tmpcolor,8);
    
    uint8_t tmpClear[] = {0xEE ,0x55 ,0x00 ,0x00 ,0x00 ,0x06 ,0x02 ,0x57 ,0x00 ,0x15 ,0xFF ,0xFC ,0xFF ,0xFF };
    tmpClear[4]=rowIndex1*16/256;
    tmpClear[5]=rowIndex1*16%256;
    tmpClear[9]=rowIndex1*16+0x0f;
    
    Uart_WriteBytes(tmpClear,14);
    
    
    //front color
    uint8_t tmpColorNew[] = {0xEE ,0x41 ,0x07 ,0xE0 ,0xFF ,0xFC ,0xFF ,0xFF };
    Uart_WriteBytes(tmpColorNew,8);
}
/*****************************/
/**** this is for uart screen ***/
uint8_t rowIndex=0;
uint8_t columnIndex=0;
void Uart_Screen_Printf(const uint8_t *fmt,...)
{
    //uint8_t tmp[] = {0xEE, 0x20, 0x00, 0x64, 0x00, 0x64, 0x01, 0x00, 0x74, 0x65, 0x73, 0x74, 0xFF, 0xFC, 0xFF, 0xFF };
    va_list ap;  
    char string[1024];  
      
    va_start(ap,fmt);  
    vsprintf(string,fmt,ap);
    
    //clear line
    Uart_Screen_ClearLine(rowIndex);
    
    //write start
    Uart_WriteByte(0xEE);
    Uart_WriteByte(0x20);
    
       
    //xIndex = columnIndex
//    uint8_t xHigh = columnIndex*10/256;
//    uint8_t xLow = columnIndex*10%256;
    Uart_WriteByte(0);
    Uart_WriteByte(0);
//    columnIndex ++;
    
    //yIndex = rowIndex
    uint8_t yHigh = rowIndex*16/256;
    uint8_t yLow = rowIndex*16%256;
    Uart_WriteByte(yHigh);
    Uart_WriteByte(yLow);
    rowIndex++;
    rowIndex=rowIndex==16?0:rowIndex;
    
    
    //write size
    Uart_WriteByte(0x01);
    Uart_WriteByte(0x05);
    
    //content
    Uart_Screen_WriteString(string);
    
    //write end
    Uart_WriteByte(0xFF);
    Uart_WriteByte(0xFC);
    Uart_WriteByte(0xFF);
    Uart_WriteByte(0xFF);
    
    va_end(ap); 
    
    Uart_Screen_ClearLine(rowIndex);
}


//?????????????  
void ByteToHexStr(const uint8_t* source, char* dest, int sourceLen)  
{  
    short i;  
    unsigned char highByte, lowByte;  
  
    for (i = 0; i < sourceLen; i++)  
    {  
        highByte = source[i] >> 4;  
        lowByte = source[i] & 0x0f ;  
  
        highByte += 0x30;  
  
        if (highByte > 0x39)  
                dest[i * 2] = highByte + 0x07;  
        else  
                dest[i * 2] = highByte;  
  
        lowByte += 0x30;  
        if (lowByte > 0x39)  
            dest[i * 2 + 1] = lowByte + 0x07;  
        else  
            dest[i * 2 + 1] = lowByte;  
    }  
    return ;  
} 


void Uart_Screen_PrintfArray(uint8_t *data, uint8_t length)
{
    //clear line
    Uart_Screen_ClearLine(rowIndex);
    
    //write start
    Uart_WriteByte(0xEE);
    Uart_WriteByte(0x20);
    
       
    //xIndex = columnIndex
//    uint8_t xHigh = columnIndex*10/256;
//    uint8_t xLow = columnIndex*10%256;
    Uart_WriteByte(0);
    Uart_WriteByte(0);
//    columnIndex ++;
    
    //yIndex = rowIndex
    uint8_t yHigh = rowIndex*16/256;
    uint8_t yLow = rowIndex*16%256;
    Uart_WriteByte(yHigh);
    Uart_WriteByte(yLow);
    rowIndex++;
    rowIndex=rowIndex==16?0:rowIndex;
    
    
    //write size
    Uart_WriteByte(0x01);
    Uart_WriteByte(0x05);
    
    //content
    char* string;
    ByteToHexStr(data,string,length);
    
    //write end
    Uart_WriteByte(0xFF);
    Uart_WriteByte(0xFC);
    Uart_WriteByte(0xFF);
    Uart_WriteByte(0xFF);
    
    
}