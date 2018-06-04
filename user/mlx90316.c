/*
 * @brief:MLX90316 driver
 * @author:zhumingxun
 * @note
 * Copyright(C) zhumingxun , 05.2018
 * All rights reserved.
 */
#include "chip.h"

#define SPIMASTERIRQHANDLER                 SPI0_IRQHandler
#define LPC_SPIMASTERPORT                   LPC_SPI0
#define LPC_SPIMASTERIRQNUM                 SPI0_IRQn


#define LPCMASTERCLOCKRATE  (100000)

static void Init_SPI_PinMux(void)
{
	/* Enable the clock to the Switch Matrix */
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);

	/* Master Pins for SPI0 */
	Chip_SWM_MovablePinAssign(SWM_SPI0_SSEL0_IO, 13);
    Chip_SWM_MovablePinAssign(SWM_SPI0_SSEL1_IO, 9);
	Chip_SWM_MovablePinAssign(SWM_SPI0_SCK_IO, 1);
	Chip_SWM_MovablePinAssign(SWM_SPI0_MISO_IO, 15);
	Chip_SWM_MovablePinAssign(SWM_SPI0_MOSI_IO, 10);

    Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_IOCON);
    Chip_IOCON_PinSetI2CMode(LPC_IOCON,IOCON_PIO10,PIN_I2CMODE_GPIO);
    //Chip_IOCON_PinSetI2CMode(LPC_IOCON,IOCON_PIO11,PIN_I2CMODE_GPIO);
    Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_IOCON);
    
//	/* Slave Pins for SPI1 */
//	Chip_SWM_MovablePinAssign(SWM_SPI1_SSEL0_IO, 11);
//	Chip_SWM_MovablePinAssign(SWM_SPI1_SCK_IO, 17);
//	Chip_SWM_MovablePinAssign(SWM_SPI1_MISO_IO, 8);
//	Chip_SWM_MovablePinAssign(SWM_SPI1_MOSI_IO, 23);

	/* Disable the clock to the Switch Matrix to save power */
	Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SWM);
//#else
//	/* Configure your own SPI pin muxing here if needed */
//#warning "No SPI pin muxing defined"
//#endif
}

/* Setup master controller */
static void setupMaster(void)
{
    static uint32_t spi_clk_rate=0;
	SPIM_DELAY_CONFIG_T masterDelay;

	/* Initialize SPI controller */
	Chip_SPI_Init(LPC_SPIMASTERPORT);

	/* Call to initialize first SPI controller for mode0, master mode, MSB first */
	Chip_SPI_ConfigureSPI(LPC_SPIMASTERPORT, SPI_MODE_MASTER |	/* Enable master mode */
						  SPI_CLOCK_CPHA1_CPOL0 |	/* Set Clock polarity to 0 */
						  SPI_CFG_MSB_FIRST_EN |/* Enable MSB first option */
						  SPI_CFG_SPOL_LO);	/* Chipselect is active low */

	/* Setup master clock rate, slave clock doesn't need to be setup */
	spi_clk_rate = Chip_SPIM_SetClockRate(LPC_SPIMASTERPORT, LPCMASTERCLOCKRATE);

	/* Setup master delay (all chip selects) */
	masterDelay.PreDelay = 0x5;
	masterDelay.PostDelay = 0x5;
	masterDelay.FrameDelay = 0x5;
	masterDelay.TransferDelay = 0x5;
	Chip_SPIM_DelayConfig(LPC_SPIMASTERPORT, &masterDelay);
    
    Chip_SPI_SetXferSize(LPC_SPI0,8);

	/* For the SPI controller configured in master mode, enable SPI master interrupts
	   for interrupt service. Do not enable SPI_INTENSET_TXDYEN. */
//	Chip_SPI_EnableInts(LPC_SPIMASTERPORT, (SPI_INTENSET_RXDYEN |
//											SPI_INTENSET_RXOVEN | SPI_INTENSET_TXUREN | SPI_INTENSET_SSAEN |
//											SPI_INTENSET_SSDEN));
    
    Chip_SPI_Enable(LPC_SPIMASTERPORT);
Chip_SPI_ClearStatus(LPC_SPIMASTERPORT,SPI_STAT_SSA);
	/* Setup master transfer callbacks in the transfer descriptor */
//	spiMasterXfer.pCB = &spiMasterCallbacks;
}


#define     SSEL0_EN         (0xe << 16)
#define     SSEL1_EN        (0xd << 16)
//#define     SSEL0_DIS        (1 << 16)
//#define     SSEL1_DIS        (1 << 16)
#define     EOT_EN          (1 << 20)
#define     EOT_DIS         (0 << 20)
#define     EOF_EN          (1 << 21)
#define     EOF_DIS         (0 << 21)
#define     RXIGNORE_EN     (1 << 22)
#define     RXIGNORE_DIS    (0 << 22)
#define     FLEN(n)         (((n) - 1) << 24)


static uint8_t SendRecv_Start (uint32_t ssel_num, uint8_t ucData)
{
    
    while (!(LPC_SPI0->STAT & (1 << 1)));  
    LPC_SPI0->TXDATCTL = FLEN(8) | EOF_EN | ssel_num | ucData;           /* 8 ??㏒?∩?那??a那?㏒????芍那?       */    
    
    while (!(LPC_SPI0->STAT & (1 << 0)));                               /* 米豕∩y?車那?那y?Y赤那3谷             */
    ucData = LPC_SPI0->RXDAT;                                           /* ?車那?那y?Y                     */
    
    return ucData;
}

static uint8_t Send_Byte (uint8_t ucData)
{
    while (!(LPC_SPI0->STAT & (1 << 1)));                               /* 米豕∩y﹞⊿?赤℅?㊣??赤D¯             */
    LPC_SPI0->TXDATCTL = FLEN(8) | EOF_EN | ucData;                     /* 8 ??㏒????芍那?                 */    
    
    while (!(LPC_SPI0->STAT & (1 << 0)));                               /* 米豕∩y?車那?那y?Y赤那3谷             */
    ucData = LPC_SPI0->RXDAT;                                           /* ?車那?那y?Y                     */
    
    return ucData;
}

static uint8_t SendRecv_Stop (uint8_t ucData)
{
    while (!(LPC_SPI0->STAT & (1 << 1)));                               /* 米豕∩y﹞⊿?赤℅?㊣??赤D¯             */
    LPC_SPI0->TXDATCTL = FLEN(8) | EOT_EN | ucData;                     /* 8 ??㏒?∩?那??芍那?               */    
    
    while (!(LPC_SPI0->STAT & (1 << 0)));                               /* 米豕∩y?車那?那y?Y赤那3谷             */
    ucData = LPC_SPI0->RXDAT;                                           /* ?車那?那y?Y                     */
    
    return ucData;
}


void Mlx90316_SPI_Config()
{
	/* SPI Pin initialization */
	Init_SPI_PinMux();
    
	/* Setup SPI controllers */
	setupMaster();
    
}




uint16_t  MLX90316_ReadData(uint8_t sseln)
{
    uint8_t temp;
    uint8_t startwordl;
    uint8_t startwordh;
    uint8_t framel;
    uint8_t frameh;
    uint8_t inverted_framel;
    uint8_t inverted_frameh;
    uint16_t startword = 0;
    uint16_t frame = 0;
    uint16_t inverted_frame = 0;
    uint16_t angle = 0;
    float anglef = 0.0;
    static uint16_t angle_14b;
    

    //黍��90316〞〞A ====================================================================

    if(sseln==0)
    {
        startwordh = SendRecv_Start(SSEL0_EN,0x55);//delay_us(50);
    }
    else
    {
        startwordh = SendRecv_Start(SSEL1_EN,0x55);//delay_us(50);
    }
    
    
    startwordl = Send_Byte(0x00);//delay_us(50);

    frameh = Send_Byte(0x00);//delay_us(50);
    framel = Send_Byte(0x00);//delay_us(50);

    inverted_frameh = Send_Byte(0x00);//delay_us(50);
    inverted_framel = Send_Byte(0x00);//delay_us(50);

    temp = Send_Byte(0x00);//delay_us(50);
    temp = Send_Byte(0x00);//delay_us(50);
    temp = Send_Byte(0x00);//delay_us(50);
    temp = SendRecv_Stop(0x00);//delay_us(50);
    
    //Chip_SPIM_DeAssertSSEL(LPC_SPI0,0);


    startword = ((uint16_t)startwordh<<8 & 0xFF00) | ((uint16_t)startwordl & 0x00FF);
    frame = ((uint16_t)frameh<<8 & 0xFF00) | ((uint16_t)framel & 0x00FF);
    if((frame & 0x0003) == 0x0001)
    {
        angle = frame>>2;
    }
    else angle = 0;
    anglef = (float)angle / (16383.0) * 360.0;

    inverted_frame = ((uint16_t)inverted_frameh<<8 & 0xFF00) | ((uint16_t)inverted_framel & 0x00FF);

    angle_14b = angle;
    return angle>>0;

}



