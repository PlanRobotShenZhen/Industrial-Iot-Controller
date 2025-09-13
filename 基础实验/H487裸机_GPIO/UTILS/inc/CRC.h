#ifndef __CRC_H__
#define __CRC_H__

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

uint16_t usMBCRC161(uint8_t* pucFrame, uint16_t usLen);
void ParseModbusMessage(uint8_t *message);
void ParseModbusRespond(uint8_t *ModbusMsg, uint16_t *DataBack, bool PrintOrNot);
	
#endif
