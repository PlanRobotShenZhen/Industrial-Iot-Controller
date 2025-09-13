#ifndef __SI2C_H__
#define __SI2C_H__

#include "init.h"

void SI2C1_Configuration(void);
uint8_t SI2C_ReadData(uint32_t x, uint8_t SlaveAddr, uint8_t RegAddr, uint8_t *Data, uint32_t Num);
uint8_t SI2C_WriteData(uint32_t x, uint8_t SlaveAddr, uint8_t RegAddr, uint8_t* Data, uint32_t Num);

#endif
