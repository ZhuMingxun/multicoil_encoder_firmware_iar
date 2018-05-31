/*
* @brief multicoil encoder used two MLX90316s
* @author:zhumingxun
* @note
* Copyright(C) zhumingxun , 05.2018
* All rights reserved.
*/

#include "chip.h"
#include "mlx90316.h"
#include "spi_slaver.h"
#include "led.h"
#include "ecd_flash.h"
#include "calc_value.h"


/*****************************************************************************
* Private types/enumerations/variables
****************************************************************************/
static bool mlx_rd_flag;

/*****************************************************************************
* Public types/enumerations/variables
****************************************************************************/





/*****************************************************************************
* Private functions
****************************************************************************/
/* Initializes pin muxing for SPI1 interface - note that SystemInit() may
already setup your pin muxing at system startup */

/*****************************************************************************
* Public functions
*****************************************************************************/

/**
* @brief	Main routine for SPI example
* @return	Does not return
*/

void main()
{
    uint32_t u32temp;

    SystemCoreClockUpdate();
    
    LED_GPIO_Config();
    
    Mlx90316_SPI_Config();
    Slaver_SPI_Config();
    
    /* Enable SysTick Timer 100Hz */
	SysTick_Config(SystemCoreClock / 100);

    /* Read Flash to know wether start value was recorded or not */
    u32temp = ReadRecordFlag();
    if( u32temp == SAVE_FLAG )
    {
        psEncoderValue->slaver_start_value = ReadSlaverStartValue();
        psEncoderValue->master_start_value = ReadMasterStartValue();
        psEncoderValue->m_s_dir = ReadMSDir();
    }
    else
    {
        psEncoderValue->slaver_start_value = MLX90316_ReadData(1);
        psEncoderValue->master_start_value = MLX90316_ReadData(0);
        psEncoderValue->m_s_dir = 1;
        WriteSlaverStartValue();
        
    }
    
    
    while(1)
    {
        
        if(mlx_rd_flag)
        {
            mlx_rd_flag = false;
            
            LED_Toggle();
            
            SampleAndCalcEncoderValue();

        }
        
    }
}


void SysTick_Handler(void)
{
    static uint16_t mlx_rd_cnt;
    
    if(mlx_rd_cnt++>=2)
    {
        mlx_rd_cnt = 0;
        mlx_rd_flag = true;
    }
    
    
}





