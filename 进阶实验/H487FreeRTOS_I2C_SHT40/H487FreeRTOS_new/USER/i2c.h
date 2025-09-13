#ifndef __SI2C_H__
#define __SI2C_H__

#include <stdint.h>
#include "init.h"
void SI2C1_Configuration(void);
uint8_t SI2C_ReadData(uint8_t x, uint8_t SlaveAddr, uint8_t RegAddr, uint8_t *Data, uint32_t Num);
uint8_t SI2C_WriteData(uint8_t x, uint8_t SlaveAddr, uint8_t RegAddr, uint8_t* Data, uint32_t Num);
uint8_t CRC8_Caculate(uint16_t data);
bool Read_TempHumi_I2C(float *pTemp, float *pHumi);
#endif
