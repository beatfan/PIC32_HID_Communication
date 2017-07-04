#include "delay.h"

void delay_ms(uint32_t milliSeconds)
{
    uint32_t frequencyHz = milliSeconds * (SYS_CLK_SystemFrequencyGet()/2000);  //while occupy 2 period
    while(frequencyHz--);
}

void delay_us(uint32_t microSeconds)
{
    uint32_t frequencyHz = microSeconds * (SYS_CLK_SystemFrequencyGet()/2000000);  //while occupy 2 period
    while(frequencyHz--);
}