#ifndef W25Q64_SPI_H
#define W25Q64_SPI_H

#include <stddef.h>
// 引入芯片相关的头文件，根据实际使用的芯片系列和开发环境调整
#include "n32h47x_48x.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_rcc.h"
#include "n32h47x_48x_spi.h"

// 定义W25Q64JVSSQ相关命令
#define W25Q64J_READ_ID       0x9F
#define W25Q64J_WRITE_ENABLE  0x06
#define W25Q64J_WRITE_DISABLE 0x04
#define W25Q64J_PAGE_PROGRAM  0x02
#define W25Q64J_READ_DATA     0x03
#define W25Q64J_SECTOR_ERASE  0x20
#define W25Q64J_CHIP_ERASE    0xC7

// 定义SPI外设（假设使用SPI3，根据实际硬件连接修改）
#define SPI_FLASH SPI3
// 重新定义片选引脚
#define FLASH_CS_PORT GPIOD
#define FLASH_CS_PIN  GPIO_PIN_3

// 片选控制宏
#define FLASH_CS_LOW()  GPIO_ResetBits(FLASH_CS_PORT, FLASH_CS_PIN)
#define FLASH_CS_HIGH() GPIO_SetBits(FLASH_CS_PORT, FLASH_CS_PIN)

// 函数声明，供外部调用
void W25Q64_SPI_Init(void);
uint8_t W25Q64_SPI_SendReceiveByte(uint8_t data);
void W25Q64_WaitForIdle(void);
void W25Q64_ReadID(uint8_t *mfg_id, uint16_t *dev_id);
void W25Q64_WriteEnable(void);
void W25Q64_PageProgram(uint32_t addr, const uint8_t *data, uint16_t len);
void W25Q64_ReadData(uint32_t addr, uint8_t *data, uint16_t len);
void W25Q64_SectorErase(uint32_t addr);
void W25Q64_ChipErase(void);

#endif /* W25Q64_SPI_H */
