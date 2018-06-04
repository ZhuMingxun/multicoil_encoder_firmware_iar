/*
 * @brief:uart init
 * @author:zhumingxun
 * @note
 * Copyright(C) zhumingxun , 05.2018
 * All rights reserved.
 */

#include "chip.h"
#include "stdio.h"

#define DEBUG_UART LPC_USART0

/* Board Debug UART Initialisation function */
STATIC void UART_SWM_Init(void)
{
	/* Enable the clock to the Switch Matrix */
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);
    
	/* Connect the U0_TXD_O and U0_RXD_I signals to port pins(P0.17, P0.18) */
	Chip_SWM_DisableFixedPin(SWM_FIXED_ADC11);
	Chip_SWM_DisableFixedPin(SWM_FIXED_ACMP_I1);
    
	/* Enable UART Divider clock, divided by 1 */
	Chip_Clock_SetUARTClockDiv(1);
    
	/* Divided by 1 */
    
    Chip_SWM_MovablePinAssign(SWM_U0_TXD_O, 4);
    Chip_SWM_MovablePinAssign(SWM_U0_RXD_I, 0);
    
    
	/* Disable the clock to the Switch Matrix to save power */
	Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SWM);
}




/* Initialize debug output via UART for board */
void Debug_UART_Init(void)
{

	UART_SWM_Init();
	Chip_UART_Init(DEBUG_UART);
	Chip_UART_ConfigData(DEBUG_UART, UART_CFG_DATALEN_8 | UART_CFG_PARITY_NONE | UART_CFG_STOPLEN_1);
	Chip_Clock_SetUSARTNBaseClockRate((115200 * 16), true);
	Chip_UART_SetBaud(DEBUG_UART, 115200);
	Chip_UART_Enable(DEBUG_UART);
	Chip_UART_TXEnable(DEBUG_UART);

}


int fputc(int ch,FILE *f)
{
    Chip_UART_SendByte(LPC_USART0,(uint8_t)ch); 
    while((Chip_UART_GetStatus(LPC_USART0) >> 0x3u & 0x1u) == 0x0u );
    return ch;                 
}



























