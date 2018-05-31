/*
 * @brief:led driver
 * @author:zhumingxun
 * @note
 * Copyright(C) zhumingxun , 05.2018
 * All rights reserved.
 */

#include "chip.h"
#include "led.h"


#define LED_PORT   0
#define LED_PIN    14

void LED_GPIO_Config()
{
    Chip_GPIO_Init(LPC_GPIO_PORT);
    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,LED_PORT,LED_PIN);
    
    
}


void LED_ON()
{
    Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,LED_PORT,LED_PIN);
    
}

void LED_OFF()
{
    Chip_GPIO_SetPortOutLow(LPC_GPIO_PORT,LED_PORT,LED_PIN);
    
}

void LED_Toggle()
{
    Chip_GPIO_SetPinToggle(LPC_GPIO_PORT,LED_PORT,LED_PIN);
}

