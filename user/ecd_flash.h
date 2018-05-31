/*
 * @brief:encoder start value save
 * @author:zhumingxun
 * @note
 * Copyright(C) zhumingxun , 05.2018
 * All rights reserved.
 */



#ifndef _ECD_FLASH_H
#define _ECD_FLASH_H

#define SAVE_FLAG       (0xAAAA5555)
#define SAVE_PAGE_NUM   (240)
#define SAVE_SECTOR     (15)

#define SAVE_START_ADDR             (0x00003C00)
#define SLAVER_STARTVALUE_ADDR      (0x00003C04)
#define MASTER_STARTVALUE_ADDR      (0x00003C08)
#define MSATER_SLAVER_DIR_ADDR      (0x00003C0C)



STATIC INLINE uint32_t ReadRecordFlag()
{
    return *(volatile uint32_t*)SAVE_START_ADDR;    
}

STATIC INLINE uint32_t ReadSlaverStartValue()
{
    
    return *(volatile uint32_t*)SLAVER_STARTVALUE_ADDR;
}

STATIC INLINE uint32_t ReadMasterStartValue()
{
    return *(volatile uint32_t*)MASTER_STARTVALUE_ADDR;
    
}

STATIC INLINE uint32_t ReadMSDir()
{
    return *(volatile uint32_t*)MSATER_SLAVER_DIR_ADDR;
    
}

void WriteSlaverStartValue();







#endif

