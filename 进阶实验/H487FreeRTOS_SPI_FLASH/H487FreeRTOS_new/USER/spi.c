
#include <stddef.h>
#include "n32h47x_48x.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_usart.h"
#include "n32h47x_48x_rcc.h"
#include "n32h47x_48x_tim.h"
#include "n32h47x_48x_dma.h"
#include "n32h47x_48x_adc.h"
#include "misc.h"
#include "n32h47x_48x_spi.h"
#include "spi.h"



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

// 片选控制宏也要相应修改
#define FLASH_CS_LOW()  GPIO_ResetBits(FLASH_CS_PORT, FLASH_CS_PIN)
#define FLASH_CS_HIGH() GPIO_SetBits(FLASH_CS_PORT, FLASH_CS_PIN)


/**
 * @brief 初始化用于控制W25Q64JVSSQ的SPI外设和GPIO
 * @note 需要根据实际硬件连接配置SPI引脚复用、时钟等
 */
void W25Q64_SPI_Init(void)
{
    
    SPI_InitType SPI_InitStructure;
    GPIO_InitType GPIO_InitStructure;

    // 使能SPI和GPIO时钟
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_SPI3, ENABLE);
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOC | RCC_AHB_PERIPHEN_GPIOD, ENABLE);
   
    // 配置SPI引脚：SCK、MOSI、MISO（根据实际引脚修改，这里以SPI3默认引脚举例）
    // SPI3_SCK -> PC10, SPI3_MOSI -> PC12, SPI3_MISO -> PC11
    GPIO_InitStructure.Pin        = GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF1;  // 假设复用功能为 GPIO_AF1
    GPIO_InitStructure.GPIO_Current = GPIO_DC_8mA;  // 8mA 驱动能力，可选 2/4/8/12mA（见 GPIO 头文件）
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;  // 若需要高速 slew rate，补充此配置   

    GPIO_InitPeripheral(GPIOC, &GPIO_InitStructure);
    // 配置CS引脚为推挽输出
    // 配置CS引脚（PD3）
    GPIO_InitStructure.Pin       = FLASH_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Current = GPIO_DC_8mA;  // 8mA驱动电流
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;  // 快速 slew rate
    GPIO_InitStructure.GPIO_Alternate = GPIO_NO_AF; 
    GPIO_InitPeripheral(FLASH_CS_PORT, &GPIO_InitStructure);
    FLASH_CS_HIGH(); // 初始置高，取消片选


    // 配置SPI参数
    // 5. 配置SPI3参数（匹配W25Q64要求）
    SPI_InitStructure.DataDirection = SPI_DIR_DOUBLELINE_FULLDUPLEX;          // 全双工模式
    SPI_InitStructure.SpiMode       = SPI_MODE_MASTER;                        // 主机模式（MCU控制Flash）
    SPI_InitStructure.DataLen       = SPI_DATA_SIZE_8BITS;                    // 8位数据（W25Q64命令/数据均为8位）
    SPI_InitStructure.CLKPOL        = SPI_CLKPOL_LOW;                         // 时钟空闲低（SPI模式0，W25Q64唯一支持）
    SPI_InitStructure.CLKPHA        = SPI_CLKPHA_FIRST_EDGE;                  // 上升沿采样（SPI模式0）
    SPI_InitStructure.NSS           = SPI_NSS_SOFT;                           // 软件控制片选（用GPIO的CS引脚，不用硬件NSS）
    SPI_InitStructure.BaudRatePres  = SPI_BR_PRESCALER_16;                    // 波特率分频（假设APB1=42MHz，42/16≈2.6MHz，低速调试更稳定）
    SPI_InitStructure.FirstBit      = SPI_FB_MSB;                             // 高位在前（SPI默认，W25Q64支持）
    SPI_InitStructure.CRCPoly       = 7;                                      // CRC校验（默认7，可忽略）

    SPI_Init(SPI_FLASH, &SPI_InitStructure);
    SPI_Enable(SPI_FLASH, ENABLE);
}

/**
 * @brief 向SPI外设发送一个字节数据，并接收返回的一个字节数据
 * @param data 要发送的数据
 * @return 接收到的数据
 */
uint8_t W25Q64_SPI_SendReceiveByte(uint8_t data)
{
    while (SPI_I2S_GetStatus(SPI_FLASH, SPI_I2S_TE_FLAG) == RESET); // 等待发送缓冲区空
    SPI_I2S_TransmitData(SPI_FLASH, data);
    while (SPI_I2S_GetStatus(SPI_FLASH, SPI_I2S_RNE_FLAG) == RESET); // 等待接收缓冲区有数据
    return SPI_I2S_ReceiveData(SPI_FLASH);
}

/**
 * @brief 等待W25Q64JVSSQ忙标志位清除（擦除、编程操作时调用）
 */
void W25Q64_WaitForIdle(void)
{
    uint8_t status = 0x01;
    FLASH_CS_LOW();
    W25Q64_SPI_SendReceiveByte(0x05); // 发送读状态寄存器命令
    while (status & 0x01)
    {
        status = W25Q64_SPI_SendReceiveByte(0xFF);
    }
    FLASH_CS_HIGH();
}

/**
 * @brief 读取W25Q64JVSSQ的制造商ID和设备ID
 * @param[out] mfg_id 存储制造商ID的变量指针
 * @param[out] dev_id 存储设备ID的变量指针
 */
void W25Q64_ReadID(uint8_t *mfg_id, uint16_t *dev_id)
{
    uint8_t id_buf[3];
    FLASH_CS_LOW();
    W25Q64_SPI_SendReceiveByte(W25Q64J_READ_ID);
    id_buf[0] = W25Q64_SPI_SendReceiveByte(0xFF); // 制造商ID
    id_buf[1] = W25Q64_SPI_SendReceiveByte(0xFF); // 设备ID高位
    id_buf[2] = W25Q64_SPI_SendReceiveByte(0xFF); // 设备ID低位
    FLASH_CS_HIGH();

    if (mfg_id) *mfg_id = id_buf[0];
    if (dev_id) *dev_id = (id_buf[1] << 8) | id_buf[2];  // 合并高位+低位
}


/**
 * @brief 发送写使能命令，允许后续的编程、擦除操作
 */
void W25Q64_WriteEnable(void)
{
    FLASH_CS_LOW();
    W25Q64_SPI_SendReceiveByte(W25Q64J_WRITE_ENABLE);
    FLASH_CS_HIGH();
}


/**
 * @brief 向指定地址写入一页（最多256字节）数据
 * @param addr 要写入的起始地址
 * @param data 要写入的数据缓冲区指针
 * @param len  要写入的数据长度（不能超过256字节）
 */
void W25Q64_PageProgram(uint32_t addr, const uint8_t *data, uint16_t len)
{
    if (len > 256) len = 256; // 一页最大256字节

    W25Q64_WriteEnable();
    FLASH_CS_LOW();
    W25Q64_SPI_SendReceiveByte(W25Q64J_PAGE_PROGRAM);
    // 发送3字节地址
    W25Q64_SPI_SendReceiveByte((addr >> 16) & 0xFF);
    W25Q64_SPI_SendReceiveByte((addr >> 8) & 0xFF);
    W25Q64_SPI_SendReceiveByte(addr & 0xFF);
    // 发送数据
    for (uint16_t i = 0; i < len; i++)
    {
        W25Q64_SPI_SendReceiveByte(data[i]);
    }
    FLASH_CS_HIGH();
    W25Q64_WaitForIdle();
}


/**
 * @brief 向指定地址写入一页（最多256字节）数据
 * @param addr 要写入的起始地址
 * @param data 要写入的数据缓冲区指针
 * @param len  要写入的数据长度（不能超过256字节）
 */


/**
 * @brief 从指定地址读取数据
 * @param addr 要读取的起始地址
 * @param data 存储读取数据的缓冲区指针
 * @param len  要读取的数据长度
 */
void W25Q64_ReadData(uint32_t addr, uint8_t *data, uint16_t len)
{
    FLASH_CS_LOW();
    W25Q64_SPI_SendReceiveByte(W25Q64J_READ_DATA);
    // 发送3字节地址
    W25Q64_SPI_SendReceiveByte((addr >> 16) & 0xFF);
    W25Q64_SPI_SendReceiveByte((addr >> 8) & 0xFF);
    W25Q64_SPI_SendReceiveByte(addr & 0xFF);
    // 接收数据
    for (uint16_t i = 0; i < len; i++)
    {
        data[i] = W25Q64_SPI_SendReceiveByte(0xFF); // 发送0xFF获取数据
    }
    FLASH_CS_HIGH();
}

/**
 * @brief 擦除指定扇区（4KB大小，地址需按扇区对齐）
 * @param addr 要擦除扇区的起始地址
 */
void W25Q64_SectorErase(uint32_t addr)
{
    W25Q64_WriteEnable();
    FLASH_CS_LOW();
    W25Q64_SPI_SendReceiveByte(W25Q64J_SECTOR_ERASE);
    // 发送3字节地址
    W25Q64_SPI_SendReceiveByte((addr >> 16) & 0xFF);
    W25Q64_SPI_SendReceiveByte((addr >> 8) & 0xFF);
    W25Q64_SPI_SendReceiveByte(addr & 0xFF);
    FLASH_CS_HIGH();
    W25Q64_WaitForIdle();
}

/**
 * @brief 擦除整个W25Q64芯片（8MB空间，耗时较长）
 */
void W25Q64_ChipErase(void)
{
    W25Q64_WriteEnable();
    FLASH_CS_LOW();
    W25Q64_SPI_SendReceiveByte(W25Q64J_CHIP_ERASE);
    FLASH_CS_HIGH();
    W25Q64_WaitForIdle();
}

