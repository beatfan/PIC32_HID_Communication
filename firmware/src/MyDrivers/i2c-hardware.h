#ifndef _I2C_HARDWARE_H
#define _I2C_HARDWARE_H

#include "../common_include.h"

#include "delay.h"

typedef struct
{
    DRV_HANDLE i2c_drvHandle;
    DRV_I2C_BUFFER_HANDLE i2c_bufferHandle;
}IIC_DATA;
IIC_DATA i2cData;

bool IIC_Initial(void);

bool IIC_WriteByte(uint8_t slaveAddr, uint8_t IIC_Byte);

bool IIC_WriteBytes(uint8_t slaveAddr, uint8_t *IIC_Bytes, uint8_t length);

bool IIC_ReadBytes(uint8_t slaveAddr, uint8_t *IIC_Bytes, uint8_t readLength);

#endif