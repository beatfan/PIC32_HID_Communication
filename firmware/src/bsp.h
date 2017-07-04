#ifndef _BSP_H
#define _BSP_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "peripheral/ports/plib_ports.h"



typedef enum
{
    BSP_SWITCH_1 = 0,
    BSP_SWITCH_2 = 1,
    BSP_SWITCH_3 = 2
} BSP_SWITCH;



typedef enum
{
    /* Switch pressed */
    BSP_SWITCH_STATE_PRESSED =/*DOM-IGNORE-BEGIN*/ 0/*DOM-IGNORE-END*/,

   /* Switch not pressed */
    BSP_SWITCH_STATE_RELEASED =/*DOM-IGNORE-BEGIN*/ 1/*DOM-IGNORE-END*/

} BSP_SWITCH_STATE;


BSP_SWITCH_STATE BSP_SwitchStateGet(BSP_SWITCH bspSwitch);

typedef enum
{
    BSP_LED_1 = 0,
    BSP_LED_2 = 1,
    BSP_LED_3 = 2
} BSP_LED;



typedef enum
{
    /* LED State is on */
    BSP_LED_STATE_OFF = /*DOM-IGNORE-BEGIN*/0/*DOM-IGNORE-END*/,

   /* LED State is off */
    BSP_LED_STATE_ON = /*DOM-IGNORE-BEGIN*/1/*DOM-IGNORE-END*/

} BSP_LED_STATE;


typedef enum
{
    /* LED active level is low */
    BSP_LED_ACTIVE_LOW = /*DOM-IGNORE-BEGIN*/0/*DOM-IGNORE-END*/,

    /* LED active level is high */
    BSP_LED_ACTIVE_HIGH = /*DOM-IGNORE-BEGIN*/1/*DOM-IGNORE-END*/

} BSP_LED_ACTIVE_LEVEL;

void BSP_LEDStateSet(BSP_LED led, BSP_LED_STATE state);

BSP_LED_STATE BSP_LEDStateGet(BSP_LED led);


void BSP_LEDToggle(BSP_LED led);


void BSP_LEDOn(BSP_LED led);


void BSP_LEDOff(BSP_LED led);


typedef enum
{
    /* USB VBUS Switch disable */
    BSP_USB_VBUS_SWITCH_STATE_DISABLE = /*DOM-IGNORE-BEGIN*/0/*DOM-IGNORE-END*/,

    /* USB VBUS Switch enable */
    BSP_USB_VBUS_SWITCH_STATE_ENABLE = /*DOM-IGNORE-BEGIN*/1/*DOM-IGNORE-END*/

} BSP_USB_VBUS_SWITCH_STATE;



void BSP_USBVBUSSwitchStateSet(BSP_USB_VBUS_SWITCH_STATE state);

void BSP_USBVBUSPowerEnable(uint8_t port, bool enable);

bool BSP_USBVBUSSwitchOverCurrentDetect(uint8_t port);


void BSP_Initialize(void);

#endif // _BSP_H
