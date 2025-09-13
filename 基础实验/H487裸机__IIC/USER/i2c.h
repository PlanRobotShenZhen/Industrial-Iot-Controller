#ifndef __SI2C_H__
#define __SI2C_H__

#include <stdint.h>

void SI2C1_Configuration(void);
uint8_t SI2C_ReadData(uint8_t x, uint8_t SlaveAddr, uint8_t RegAddr, uint8_t *Data, uint32_t Num);
uint8_t SI2C_WriteData(uint8_t x, uint8_t SlaveAddr, uint8_t RegAddr, uint8_t* Data, uint32_t Num);

#endif
