#include "usb-hid.h"

USB_HID_DATA usbhidData;

#define APP_MAKE_BUFFER_DMA_READY __attribute__ ((coherent, aligned(16)))

/* Recieve data buffer */
uint8_t receiveDataBuffer[64] APP_MAKE_BUFFER_DMA_READY;

/* Transmit data buffer */
uint8_t  transmitDataBuffer[64] APP_MAKE_BUFFER_DMA_READY;


USB_DEVICE_HID_EVENT_RESPONSE USB_HID_USBDeviceHIDEventHandler
(
    USB_DEVICE_HID_INDEX iHID,
    USB_DEVICE_HID_EVENT event,
    void * eventData,
    uintptr_t userData
)
{
    USB_DEVICE_HID_EVENT_DATA_REPORT_SENT * reportSent;
    USB_DEVICE_HID_EVENT_DATA_REPORT_RECEIVED * reportReceived;

    /* Check type of event */
    switch (event)
    {
        case USB_DEVICE_HID_EVENT_REPORT_SENT:

            /* The eventData parameter will be USB_DEVICE_HID_EVENT_REPORT_SENT
             * pointer type containing details about the report that was
             * sent. */
            reportSent = (USB_DEVICE_HID_EVENT_DATA_REPORT_SENT *) eventData;
            if(reportSent->handle == usbhidData.txTransferHandle )
            {
                // Transfer progressed.
                usbhidData.hidDataTransmitted = true;
            }
            
            break;

        case USB_DEVICE_HID_EVENT_REPORT_RECEIVED:

            /* The eventData parameter will be USB_DEVICE_HID_EVENT_REPORT_RECEIVED
             * pointer type containing details about the report that was
             * received. */

            reportReceived = (USB_DEVICE_HID_EVENT_DATA_REPORT_RECEIVED *) eventData;
            if(reportReceived->handle == usbhidData.rxTransferHandle )
            {
                // Transfer progressed.
                usbhidData.hidDataReceived = true;
            }
          
            break;

        case USB_DEVICE_HID_EVENT_SET_IDLE:

            /* For now we just accept this request as is. We acknowledge
             * this request using the USB_DEVICE_HID_ControlStatus()
             * function with a USB_DEVICE_CONTROL_STATUS_OK flag */

            USB_DEVICE_ControlStatus(usbhidData.usbDevHandle, USB_DEVICE_CONTROL_STATUS_OK);

            /* Save Idle rate recieved from Host */
            usbhidData.idleRate = ((USB_DEVICE_HID_EVENT_DATA_SET_IDLE*)eventData)->duration;
            break;

        case USB_DEVICE_HID_EVENT_GET_IDLE:

            /* Host is requesting for Idle rate. Now send the Idle rate */
            USB_DEVICE_ControlSend(usbhidData.usbDevHandle, & (usbhidData.idleRate),1);

            /* On successfully reciveing Idle rate, the Host would acknowledge back with a
               Zero Length packet. The HID function drvier returns an event
               USB_DEVICE_HID_EVENT_CONTROL_TRANSFER_DATA_SENT to the application upon
               receiving this Zero Length packet from Host.
               USB_DEVICE_HID_EVENT_CONTROL_TRANSFER_DATA_SENT event indicates this control transfer
               event is complete */

            break;
        default:
            // Nothing to do.
            break;
    }
    return USB_DEVICE_HID_EVENT_RESPONSE_NONE;
}

void USB_HID_USBDeviceEventHandler(USB_DEVICE_EVENT event, void * eventData, uintptr_t context)
{
    switch(event)
    {
        case USB_DEVICE_EVENT_RESET:
        case USB_DEVICE_EVENT_DECONFIGURED:

            /* Host has de configured the device or a bus reset has happened.
             * Device layer is going to de-initialize all function drivers.
             * Hence close handles to all function drivers (Only if they are
             * opened previously. */


            usbhidData.deviceConfigured = false;
            usbhidData.state = USB_HID_STATE_WAIT_FOR_CONFIGURATION;
            break;

        case USB_DEVICE_EVENT_CONFIGURED:
            /* Set the flag indicating device is configured. */
            usbhidData.deviceConfigured = true;

            /* Save the other details for later use. */
            usbhidData.configurationValue = ((USB_DEVICE_EVENT_DATA_CONFIGURED*)eventData)->configurationValue;

            /* Register application HID event handler */
            USB_DEVICE_HID_EventHandlerSet(USB_DEVICE_HID_INDEX_0, USB_HID_USBDeviceHIDEventHandler, (uintptr_t)&usbhidData);



            break;

        case USB_DEVICE_EVENT_SUSPENDED:


            break;

        case USB_DEVICE_EVENT_POWER_DETECTED:

            /* VBUS was detected. We can attach the device */

            USB_DEVICE_Attach (usbhidData.usbDevHandle);
            break;

        case USB_DEVICE_EVENT_POWER_REMOVED:

            /* VBUS is not available */
            USB_DEVICE_Detach(usbhidData.usbDevHandle);
            break;

        /* These events are not used in this demo */
        case USB_DEVICE_EVENT_RESUMED:
        case USB_DEVICE_EVENT_ERROR:
        default:
            break;
    }
}

void USB_HID_Initial(void)
{
    usbhidData.state = USB_HID_STATE_INIT;
    
    usbhidData.usbDevHandle = USB_DEVICE_HANDLE_INVALID;
    usbhidData.deviceConfigured = false;
    usbhidData.txTransferHandle = USB_DEVICE_HID_TRANSFER_HANDLE_INVALID;
    usbhidData.rxTransferHandle = USB_DEVICE_HID_TRANSFER_HANDLE_INVALID;
    usbhidData.hidDataReceived = false;
    usbhidData.hidDataTransmitted = true;
    usbhidData.receiveDataBuffer = &receiveDataBuffer[0];
    
}

void USB_HID_Tasks (void )
{

    /* Check if device is configured.  See if it is configured with correct
     * configuration value  */

    switch(usbhidData.state)
    {
        case USB_HID_STATE_INIT:
    
            /* Open the device layer */
            usbhidData.usbDevHandle = USB_DEVICE_Open( USB_DEVICE_INDEX_0, DRV_IO_INTENT_READWRITE );

            if(usbhidData.usbDevHandle != USB_DEVICE_HANDLE_INVALID)
            {
                /* Register a callback with device layer to get event notification (for end point 0) */
                USB_DEVICE_EventHandlerSet(usbhidData.usbDevHandle, USB_HID_USBDeviceEventHandler, 0);

                usbhidData.state = USB_HID_STATE_WAIT_FOR_CONFIGURATION;
            }
            else
            {
                /* The Device Layer is not ready to be opened. We should try
                 * again later. */
            }

            
            
            break;

        case USB_HID_STATE_WAIT_FOR_CONFIGURATION:

            if(usbhidData.deviceConfigured == true)
            {
                /* Device is ready to run the main task */
                usbhidData.hidDataReceived = false;
                usbhidData.hidDataTransmitted = true;
                usbhidData.state = USB_HID_STATE_MAIN_TASK;

                /* Place a new read request. */
                USB_DEVICE_HID_ReportReceive (USB_DEVICE_HID_INDEX_0,
                        &usbhidData.rxTransferHandle, usbhidData.receiveDataBuffer, 64);
            }
            break;

        case USB_HID_STATE_MAIN_TASK:

            if(!usbhidData.deviceConfigured)
            {
                /* Device is not configured */
                usbhidData.state = USB_HID_STATE_WAIT_FOR_CONFIGURATION;
            }
            else if( usbhidData.hidDataReceived )  //receive data 
            {

                USB_HID_ReceiveBytes(usbhidData.receiveDataBuffer);
                

            }
        case USB_HID_STATE_ERROR:
            break;
        default:
            break;
    }
}

bool USB_HID_SendBytes(uint8_t *datas, uint8_t length)
{
    if(usbhidData.hidDataTransmitted)
    {
        usbhidData.hidDataTransmitted = false;

        //uint8_t buffers[64];
        transmitDataBuffer[0] = length;

        uint8_t i=1;
        for(i=1;i<length+1;i++)
        {
            transmitDataBuffer[i]=datas[i-1];
        }

        /* Prepare the USB module to send the data packet to the host */
        USB_DEVICE_HID_ReportSend (USB_DEVICE_HID_INDEX_0,
                &usbhidData.txTransferHandle, transmitDataBuffer, 64 );
        
        return true;
    }
    else
        return false;
}

//receive data entry
//first byte is data length
void USB_HID_ReceiveBytes(uint8_t * datas)
{

    uint8_t length = datas[0];
    
    
    uint8_t i;
    for(i=0;i<length;i++)
    {
        receiveDataBuffer[i] = datas[i+1];
        Uart_Screen_Printf("Data %u:%02X",i,receiveDataBuffer[i]);
    }
    
    uint8_t tmpSend[] = {0xaa,0xbb};
    //output the receive data here
    USB_HID_SendBytes(tmpSend,2);
    
    if(receiveDataBuffer[0]==0x80)
    {
        SYS_PORTS_PinToggle(0,PORT_CHANNEL_H,0);
    }
    
    USB_HID_StartNewReceiveBytes();
}

//allow to recive new hid data
bool USB_HID_StartNewReceiveBytes(void)
{
    usbhidData.hidDataReceived = false;

    /* Place a new read request. */
    USB_DEVICE_HID_ReportReceive (USB_DEVICE_HID_INDEX_0,
            &usbhidData.rxTransferHandle, usbhidData.receiveDataBuffer, 64 );
    
    return true;
}