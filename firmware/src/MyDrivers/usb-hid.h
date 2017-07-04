#ifndef _USB_HID_H
#define _USB_HID_H

#define hidMaxBufferSize 64

#include "../common_include.h"
#include "uartScreen/uart.h"

typedef enum
{
    USB_HID_STATE_INIT,
            
    /* Application is waiting for configuration */
    USB_HID_STATE_WAIT_FOR_CONFIGURATION,

    /* Application is running the main tasks */
    USB_HID_STATE_MAIN_TASK,

    /* Application is in an error state */
    USB_HID_STATE_ERROR
}USB_HID_STATE;

typedef struct
{
    /* The application's current state */
    USB_HID_STATE state;

      /* Device layer handle returned by device layer open function */
    USB_DEVICE_HANDLE  usbDevHandle;

    /* Recieve data buffer */
    uint8_t * receiveDataBuffer;


    /* Device configured */
    bool deviceConfigured;

    /* Send report transfer handle*/
    USB_DEVICE_HID_TRANSFER_HANDLE txTransferHandle;

    /* Receive report transfer handle */
    USB_DEVICE_HID_TRANSFER_HANDLE rxTransferHandle;

    /* Configuration value selected by the host*/
    uint8_t configurationValue;

    /* HID data received flag*/
    bool hidDataReceived;

    /* HID data transmitted flag */
    bool hidDataTransmitted;

     /* USB HID current Idle */
    uint8_t idleRate;

} USB_HID_DATA;

void USB_HID_Tasks(void);

bool USB_HID_SendBytes(uint8_t *datas, uint8_t length);

void USB_HID_ReceiveBytes(uint8_t * datas);

bool USB_HID_StartNewReceiveBytes(void);

extern const USB_DEVICE_FUNCTION_REGISTRATION_TABLE funcRegistrationTable[1];
extern const USB_DEVICE_MASTER_DESCRIPTOR usbMasterDescriptor;

#endif