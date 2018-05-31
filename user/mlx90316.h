/*
 * @brief:MLX90316 driver
 * @author:zhumingxun
 * @note
 * Copyright(C) zhumingxun , 05.2018
 * All rights reserved.
 */
#ifndef _MLX90316_H
#define _MLX90316_H


#include "chip.h"

void Mlx90316_SPI_Config();
uint16_t  MLX90316_ReadData(uint8_t sseln);




#endif

















