#include "i2c-hardware.h"

bool IIC_Initial(void)
{
    i2cData.i2c_drvHandle = DRV_I2C_Open(0,DRV_IO_INTENT_READWRITE);
    
    if(i2cData.i2c_drvHandle==NULL)
        return false;
    else
        return true;
}


DRV_I2C_BUFFER_EVENT IIC_Check_Transfer_Status(DRV_HANDLE drvOpenHandle, DRV_I2C_BUFFER_HANDLE drvBufferHandle)
{
    return (DRV_I2C_TransferStatusGet (drvOpenHandle, drvBufferHandle));
}

bool IIC_WriteByte(uint8_t slaveAddr, uint8_t IIC_Byte)
{
    uint8_t tmp[] = {IIC_Byte};
    if ( (i2cData.i2c_bufferHandle == (DRV_I2C_BUFFER_HANDLE) NULL) || 
         (IIC_Check_Transfer_Status(i2cData.i2c_drvHandle,  i2cData.i2c_bufferHandle) == DRV_I2C_BUFFER_EVENT_COMPLETE) || 
         (IIC_Check_Transfer_Status(i2cData.i2c_drvHandle, i2cData.i2c_bufferHandle) == DRV_I2C_BUFFER_EVENT_ERROR)  )
    {
        i2cData.i2c_bufferHandle = DRV_I2C_Transmit(i2cData.i2c_drvHandle,slaveAddr,tmp,1,NULL);
        delay_ms(1);
        return true;
    }
    else
        return false;

}


bool IIC_WriteBytes(uint8_t slaveAddr, uint8_t *IIC_Bytes, uint8_t length)
{
    if ( (i2cData.i2c_bufferHandle == (DRV_I2C_BUFFER_HANDLE) NULL) || 
         (IIC_Check_Transfer_Status(i2cData.i2c_drvHandle,  i2cData.i2c_bufferHandle) == DRV_I2C_BUFFER_EVENT_COMPLETE) || 
         (IIC_Check_Transfer_Status(i2cData.i2c_drvHandle, i2cData.i2c_bufferHandle) == DRV_I2C_BUFFER_EVENT_ERROR)  )
    {
        i2cData.i2c_bufferHandle = DRV_I2C_Transmit(i2cData.i2c_drvHandle,slaveAddr,IIC_Bytes,length,NULL);
        delay_ms(1);
        return true;
    }
    else
        return false;

}

bool IIC_ReadBytes(uint8_t slaveAddr, uint8_t *IIC_Bytes, uint8_t readLength)
{
    if ( (i2cData.i2c_bufferHandle == (DRV_I2C_BUFFER_HANDLE) NULL) || 
    (IIC_Check_Transfer_Status(i2cData.i2c_drvHandle,  i2cData.i2c_bufferHandle) == DRV_I2C_BUFFER_EVENT_COMPLETE) || 
        (IIC_Check_Transfer_Status(i2cData.i2c_drvHandle, i2cData.i2c_bufferHandle) == DRV_I2C_BUFFER_EVENT_ERROR)  )
    {
        i2cData.i2c_bufferHandle = DRV_I2C_Receive(i2cData.i2c_drvHandle,
                                                                    slaveAddr,
                                                                    IIC_Bytes, readLength, NULL);
        delay_ms(1);
        return true;
    }
    else
        return false;
}
