/*
 * @brief:encoder start value save
 * @author:zhumingxun
 * @note
 * Copyright(C) zhumingxun , 05.2018
 * All rights reserved.
 */

#include "chip.h"
#include "ecd_flash.h"
#include "calc_value.h"

uint32_t ecdval[16];

void WriteSlaverStartValue()
{
    uint8_t retcode;
            
    ecdval[0] = SAVE_FLAG;       
    ecdval[1] = psEncoderValue->slaver_start_value;
    ecdval[2] = psEncoderValue->master_start_value;
    ecdval[3] = psEncoderValue->m_s_dir;
    
    __disable_irq();
    
    retcode = Chip_IAP_PreSectorForReadWrite(SAVE_SECTOR, SAVE_SECTOR);
    retcode = Chip_IAP_ErasePage(SAVE_PAGE_NUM,SAVE_PAGE_NUM);
    
    retcode = Chip_IAP_PreSectorForReadWrite(SAVE_SECTOR, SAVE_SECTOR);
    retcode = Chip_IAP_CopyRamToFlash(SAVE_START_ADDR, ecdval, 64);
    
    __enable_irq();
}






