/*
 * @brief:spi slaver 
 * @author:zhumingxun
 * @note
 * Copyright(C) zhumingxun , 05.2018
 * All rights reserved.
 */

#include "chip.h"
#include "calc_value.h"

#define SPISLAVEIRQHANDLER                  SPI1_IRQHandler
#define LPC_SPISLAVEPORT                    LPC_SPI1
#define LPC_SPISLAVEIRQNUM                  SPI1_IRQn

bool SPI1_WaitTXRDY()
{
    uint16_t cnt=0;
    while (!(LPC_SPI1->STAT & (1 << 1)))
    {
        if(cnt++ > 500)
            return 0;
    }
    return 1;
    
}

bool SPI1_WaitRXRDY()
{
    uint16_t cnt=0;
    while (!(LPC_SPI1->STAT & (1 << 0)))
    {
        if(cnt++ > 500)
            return 0;
    }
    return 1;
    
}

uint8_t Slaver_Send_Byte (uint8_t ucData)
{
    if(SPI1_WaitTXRDY())                             /* 米豕∩y﹞⊿?赤℅?㊣??赤D¯             */
      LPC_SPI1->TXDAT = ucData;                     /* 8 ??㏒????芍那?                 */    

    if(SPI1_WaitRXRDY())                               /* 米豕∩y?車那?那y?Y赤那3谷             */
      ucData = LPC_SPI1->RXDAT;
    else
        ucData = 0x01;
/* ?車那?那y?Y                     */
    
    return ucData;
}

static uint8_t Slaver_Send_LastByte (uint8_t ucData)
{
    if(SPI1_WaitTXRDY())                             /* 米豕∩y﹞⊿?赤℅?㊣??赤D¯             */
      LPC_SPI1->TXDATCTL = (0<<22)|ucData;                     /* 8 ??㏒????芍那?                 */    
    
//    if(SPI1_WaitRXRDY())                               /* 米豕∩y?車那?那y?Y赤那3谷             */
//      ucData = LPC_SPI1->RXDAT;                                           /* ?車那?那y?Y                     */
    
    return ucData;
}


static void Init_SPIS_PinMux(void)
{
	/* Enable the clock to the Switch Matrix */
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);

    Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_IOCON);
    //Chip_IOCON_PinSetI2CMode(LPC_IOCON,IOCON_PIO10,PIN_I2CMODE_GPIO);
    Chip_IOCON_PinSetI2CMode(LPC_IOCON,IOCON_PIO11,PIN_I2CMODE_GPIO);
    Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_IOCON);
    
	/* Slave Pins for SPI1 */
	Chip_SWM_MovablePinAssign(SWM_SPI1_SSEL0_IO, 11);
	Chip_SWM_MovablePinAssign(SWM_SPI1_SCK_IO, 17);
	Chip_SWM_MovablePinAssign(SWM_SPI1_MISO_IO, 8);
	Chip_SWM_MovablePinAssign(SWM_SPI1_MOSI_IO, 23);

	/* Disable the clock to the Switch Matrix to save power */
	Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SWM);

}
/* Setup slave controller */
static void setupSlave(void)
{
	/* Initialize SPI controller */
	Chip_SPI_Init(LPC_SPISLAVEPORT);
    
	/* Call to initialize first SPI controller for mode0, master mode, MSB first */
	Chip_SPI_ConfigureSPI(LPC_SPISLAVEPORT, 
                          SPI_MODE_SLAVE |/* Enable slave mode */
						  SPI_CLOCK_CPHA1_CPOL0 |	/* Set Clock polarity to 0 */
                          SPI_CFG_MSB_FIRST_EN |/* Enable MSB first option */
                          SPI_CFG_SPOL_LO);	/* Chipselect is active low */
    
	/* Setup slave controller for 8-bit transfer. Sizes can be altered later
    for each slave select in the slave select assertion callback once the
    transfer starts if needed. The master slave transfer size is setup as
    part of the master transfer description in the options field. */
	Chip_SPI_SetXferSize(LPC_SPISLAVEPORT, 8);
    
	/* For the SPI controller configured in slave mode, enable SPI slave interrupts
    for interrupt service. Do not enable SPI_INTENSET_TXDYEN. */
	Chip_SPI_EnableInts(LPC_SPISLAVEPORT, (SPI_STAT_SSA));
    
    Chip_SPI_Enable(LPC_SPISLAVEPORT);
    
    NVIC_EnableIRQ(LPC_SPISLAVEIRQNUM);
    Chip_SPI_ClearStatus(LPC_SPISLAVEPORT,SPI_STAT_SSA);
    Chip_SPI_ClearStatus(LPC_SPISLAVEPORT,SPI_STAT_SSD);
    
}


void Slaver_SPI_Config()
{
     Init_SPIS_PinMux();
    
     setupSlave();
}




/**
 * @brief	SPI1 interrupt handler (slave)
 * @return	Nothing
 */
void SPISLAVEIRQHANDLER(void)
{
	uint32_t ints = Chip_SPI_GetStatus(LPC_SPISLAVEPORT);
    static uint8_t rcvdat[20];
    static uint8_t rcvtmp=0;
    static uint8_t rcv[10];
    static uint8_t i=0;
    uint8_t waitcnt=0;
    static uint8_t dummy_datah;
    static uint8_t dummy_datal;
    static uint8_t dummy_datahn;
    static uint8_t dummy_dataln;
    
    static uint16_t dummy_value;
    uint16_t dummy_data;
    
    dummy_value = psEncoderValue->delta/4;
    
    dummy_data = (dummy_value<<2) | 0x0001;
    dummy_datah = (uint8_t)(dummy_data>>8);
    dummy_datal = (uint8_t)(dummy_data>>0);
    dummy_datahn = ~dummy_datah;
    dummy_dataln = ~dummy_datal;

    if(dummy_value>16383)
        dummy_value=0;
    
	/* Handle SPI slave interrupts only */
	if ((ints & (SPI_STAT_SSA)) != 0) 
    {
        Chip_SPI_ClearStatus(LPC_SPISLAVEPORT,SPI_STAT_SSA);
        if(SPI1_WaitRXRDY())
        {
          if(i>19)
              i=0;
          rcvtmp = rcvdat[i++] = LPC_SPI1->RXDAT;
          
          if( rcvtmp == 0xAA )
          {
              rcv[0]=Slaver_Send_Byte(0x00);//0xFF
              rcv[1]=Slaver_Send_Byte(~dummy_datah);//frameh
              rcv[2]=Slaver_Send_Byte(~dummy_datal);//framel
              rcv[3]=Slaver_Send_Byte(~dummy_datahn);//~frameh
              rcv[4]=Slaver_Send_Byte(~dummy_dataln);//~framel
              
//              rcv[1]=Slaver_Send_Byte((uint8_t)(psEncoderValue->delta>>24));//frameh
//              rcv[2]=Slaver_Send_Byte((uint8_t)(psEncoderValue->delta>>16));//framel
//              
//              rcv[3]=Slaver_Send_Byte((uint8_t)(psEncoderValue->delta>>8));//~frameh
//              rcv[4]=Slaver_Send_Byte((uint8_t)(psEncoderValue->delta>>0));//~framel
              
              if(SPI1_WaitTXRDY())                            
                  LPC_SPI1->TXDAT = 0;   
              
              /* 4 0xFF */
              //rcv[5]=Slaver_Send_Byte(0x55);
//              rcv[6]=Slaver_Send_Byte(0x00);
//              rcv[7]=Slaver_Send_Byte(0x00);
//              rcv[8]=Slaver_Send_Byte(0x00);
//              while((Chip_SPI_GetStatus(LPC_SPISLAVEPORT) & SPI_STAT_SSD) == 0);
//                Chip_SPI_ClearStatus(LPC_SPISLAVEPORT, SPI_STAT_SSD);
          }
        }
	
	}
    
  //Chip_SPI_EnableInts(LPC_SPISLAVEPORT,SPI_INTENSET_RXDYEN);
    
}











