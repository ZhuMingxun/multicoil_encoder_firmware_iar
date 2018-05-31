/*
 * @brief:calculate encoder value
 * @author:zhumingxun
 * @note
 * Copyright(C) zhumingxun , 05.2018
 * All rights reserved.
 */


#ifndef _CALC_VALUE_H
#define _CALC_VALUE_H


typedef struct
{
    uint32_t m_s_dir;/* 0:reverse;1:syn */
    uint32_t slaver_tmp;
    uint32_t slaver_start_value;
    uint32_t slaver_delta;
    uint32_t master_tmp;
    uint32_t master_start_value;
    uint32_t master_delta;
    uint32_t delta;

}EncoderValue_s;
extern EncoderValue_s *psEncoderValue;

STATIC INLINE uint32_t Calc_Slaver_Delta()
{
    if(psEncoderValue->slaver_tmp >= psEncoderValue->slaver_start_value)
    {
        return (psEncoderValue->slaver_tmp - psEncoderValue->slaver_start_value);
    }
    else
    {
        return (16383-(psEncoderValue->slaver_start_value - psEncoderValue->slaver_tmp));
    }
    
}

STATIC INLINE uint32_t Calc_Master_Delta()
{
    uint32_t ret_delta;
      
    if( psEncoderValue->m_s_dir )/* Master Slaver syn */
    {
        if(psEncoderValue->master_tmp >= psEncoderValue->master_start_value)
        {
            ret_delta = psEncoderValue->master_tmp - psEncoderValue->master_start_value;
        }
        else
        {
            ret_delta = 16383 - ( psEncoderValue->master_start_value - psEncoderValue->master_tmp );
        }
    }
    else
    {
        if(psEncoderValue->master_tmp > psEncoderValue->master_start_value)
        {
            ret_delta = 16383 - (psEncoderValue->master_tmp - psEncoderValue->master_start_value);
        }
        else
        {
            ret_delta = psEncoderValue->master_start_value - psEncoderValue->master_tmp;
        }   
    }
    
    return ret_delta;
}

void SampleAndCalcEncoderValue();







#endif




