/*
 * @brief:calculate encoder value
 * @author:zhumingxun
 * @note
 * Copyright(C) zhumingxun , 05.2018
 * All rights reserved.
 */

#include "mlx90316.h"
#include "calc_value.h"
#include "ecd_flash.h"

EncoderValue_s sEncoderValue;
EncoderValue_s *psEncoderValue = &sEncoderValue;
static float RatioK = 4.0;


void SampleAndCalcEncoderValue()
{

    /* Sample MLX90316 Value */
    psEncoderValue->slaver_tmp = MLX90316_ReadData(1);
    psEncoderValue->master_tmp = MLX90316_ReadData(0);
    
    /* Calculate master slaver delta */
    psEncoderValue->slaver_delta = Calc_Slaver_Delta();
    psEncoderValue->master_delta = Calc_Master_Delta();
    
    /* Calculate delta */
    psEncoderValue->delta = (uint32_t)(psEncoderValue->slaver_delta / 16383.0f * RatioK)*16383 + psEncoderValue->master_delta;
     
    
}

void Detect_MS_Dir()
{
 
    
}




























