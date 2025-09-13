#include "main.h"
#include "usart.h"
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



// -------------------------- W25Q64 命令宏定义 --------------------------
// W25Q64是SPI Flash芯片，所有操作都通过发送“命令”实现（命令值由芯片手册规定）
#define W25Q64J_READ_ID       0x9F  // 读制造商ID和设备ID（验证芯片是否正常连接）
#define W25Q64J_WRITE_ENABLE  0x06  // 写使能（擦除/编程前必须发此命令，否则操作无效）
#define W25Q64J_WRITE_DISABLE 0x04  // 写禁用（禁止擦除/编程，用于保护数据）
#define W25Q64J_PAGE_PROGRAM  0x02  // 页编程（向Flash写入数据，一页最大256字节）
#define W25Q64J_READ_DATA     0x03  // 读数据（从Flash指定地址读取数据）
#define W25Q64J_SECTOR_ERASE  0x20  // 扇区擦除（擦除4KB数据，Flash写入前必须先擦除）
#define W25Q64J_CHIP_ERASE    0xC7  // 整片擦除（擦除整个芯片8MB数据，耗时较长）


// -------------------------- 硬件连接宏定义 --------------------------
// 1. SPI外设：使用SPI3（可根据实际硬件修改为SPI1/SPI2）
#define SPI_FLASH SPI3
// 2. 片选引脚（CS）：控制Flash是否被选中（低电平选中，高电平取消选中）
#define FLASH_CS_PORT GPIOD  // CS引脚所在端口：GPIOD
#define FLASH_CS_PIN  GPIO_PIN_3  // CS引脚编号：PD3

// 3. 片选控制宏（简化代码，直接调用即可控制CS电平）
#define FLASH_CS_LOW()  GPIO_ResetBits(FLASH_CS_PORT, FLASH_CS_PIN)  // CS拉低（选中Flash）
#define FLASH_CS_HIGH() GPIO_SetBits(FLASH_CS_PORT, FLASH_CS_PIN)    // CS拉高（取消选中）

/**
 * @brief  初始化W25Q64所需的SPI外设和GPIO引脚
 * @note   核心：SPI参数必须与W25Q64的要求匹配（如SPI模式0、8位数据、高位在前）
 */
void W25Q64_SPI_Init(void)
{
    
    SPI_InitType SPI_InitStructure;
    GPIO_InitType GPIO_InitStructure;

// -------------------------- 1. 使能外设时钟（给硬件上电） --------------------------
    // 使能SPI3时钟（SPI3挂载在APB1总线，故用RCC_EnableAPB1PeriphClk）
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_SPI3, ENABLE);
    // 使能GPIO时钟：GPIOC（SPI3的SCK/MOSI/MISO引脚）和GPIOD（CS引脚）
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOC | RCC_AHB_PERIPHEN_GPIOD, ENABLE);
   
// -------------------------- 2. 配置SPI引脚（SCK/MOSI/MISO） --------------------------
    // SPI3_SCK -> PC10, SPI3_MOSI -> PC12, SPI3_MISO -> PC11
    GPIO_InitStructure.Pin        = GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF1;  // 假设复用功能为 GPIO_AF1
    GPIO_InitStructure.GPIO_Current = GPIO_DC_8mA;  // 8mA 驱动能力，可选 2/4/8/12mA（见 GPIO 头文件）
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;  // 若需要高速 slew rate，补充此配置   

    // 将配置应用到GPIOC端口（使SPI引脚生效）
    GPIO_InitPeripheral(GPIOC, &GPIO_InitStructure);
// -------------------------- 3. 配置CS引脚（片选控制） --------------------------
    // 配置CS引脚（PD3）
    GPIO_InitStructure.Pin       = FLASH_CS_PIN;// 选中PD3引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;// 普通推挽输出（CS由软件直接控制，非复用）
    GPIO_InitStructure.GPIO_Current = GPIO_DC_8mA;  // 8mA驱动电流
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;  // 快速 slew rate
    GPIO_InitStructure.GPIO_Alternate = GPIO_NO_AF;  // 不使用复用功能（普通GPIO）
    // 将配置应用到GPIOD端口（使CS引脚生效）
    GPIO_InitPeripheral(FLASH_CS_PORT, &GPIO_InitStructure);
    FLASH_CS_HIGH(); // 初始置高，取消片选

// -------------------------- 4. 配置SPI参数（关键！必须匹配Flash要求） --------------------------
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

    // 将SPI配置应用到SPI3外设（使SPI参数生效）
    SPI_Init(SPI_FLASH, &SPI_InitStructure);
    // 使能SPI3（SPI配置完成后，必须使能才能工作）
    SPI_Enable(SPI_FLASH, ENABLE);
}

/**
 * @brief 向SPI外设发送一个字节数据，并接收返回的一个字节数据
 * @param data 要发送的数据
 * @return 接收到的数据
 */
uint8_t W25Q64_SPI_SendReceiveByte(uint8_t data)
{
    // 1. 等待SPI发送缓冲区为空（确保之前的数据已发完，才能发新数据）
    while (SPI_I2S_GetStatus(SPI_FLASH, SPI_I2S_TE_FLAG) == RESET); 
    // 2. 发送1个字节到Flash（将data写入SPI发送寄存器）
    SPI_I2S_TransmitData(SPI_FLASH, data);
    // 3. 等待SPI接收缓冲区有数据（确保Flash已返回数据）
    while (SPI_I2S_GetStatus(SPI_FLASH, SPI_I2S_RNE_FLAG) == RESET);
    // 4. 读取接收缓冲区的数据（返回Flash发来的字节）    
    return SPI_I2S_ReceiveData(SPI_FLASH);
}

/**
 * @brief 等待W25Q64JVSSQ忙标志位清除（擦除、编程操作时调用）
 */
void W25Q64_WaitForIdle(void)
{
      uint8_t status = 0x01;  // 初始值设为1（表示忙）
    FLASH_CS_LOW();  // 拉低CS，选中Flash（要读状态，必须先选中）
    
    // 发送“读状态寄存器1”命令（0x05，芯片手册规定）
    W25Q64_SPI_SendReceiveByte(0x05);
    
    // 循环读取状态寄存器：bit0是忙标志（1=忙，0=空闲）
    while (status & 0x01)
    {
        // 发送0xFF（空字节），接收状态寄存器的值
        status = W25Q64_SPI_SendReceiveByte(0xFF);
    }
    
    FLASH_CS_HIGH();  // 拉高CS，取消选中Flash
}

/**
 * @brief 读取W25Q64JVSSQ的制造商ID和设备ID
 * @param[out] mfg_id 存储制造商ID的变量指针
 * @param[out] dev_id 存储设备ID的变量指针
 */
void W25Q64_ReadID(uint8_t *mfg_id, uint16_t *dev_id)
{
    uint8_t id_buf[3];  // 存储3字节ID（1字节制造商ID + 2字节设备ID）
    FLASH_CS_LOW();  // 拉低CS，选中Flash
    
    // 发送“读ID”命令（0x9F）
    W25Q64_SPI_SendReceiveByte(W25Q64J_READ_ID);
    // 接收3字节ID（发送0xFF空字节，获取ID）
    id_buf[0] = W25Q64_SPI_SendReceiveByte(0xFF);  // 第1字节：制造商ID（0xEF）
    id_buf[1] = W25Q64_SPI_SendReceiveByte(0xFF);  // 第2字节：设备ID高位（0x40）
    id_buf[2] = W25Q64_SPI_SendReceiveByte(0xFF);  // 第3字节：设备ID低位（0x17）
    
    FLASH_CS_HIGH();  // 拉高CS，取消选中

    // 将ID赋值给外部变量（判断指针非空，避免崩溃）
    if (mfg_id) *mfg_id = id_buf[0];
    if (dev_id) *dev_id = (id_buf[1] << 8) | id_buf[2];  // 合并高低位，得到16位设备ID
}


/**
 * @brief 发送写使能命令，允许后续的编程、擦除操作
 */
void W25Q64_WriteEnable(void)
{
    FLASH_CS_LOW();  // 拉低CS，选中Flash
    W25Q64_SPI_SendReceiveByte(W25Q64J_WRITE_ENABLE);  // 发送写使能命令（0x06）
    FLASH_CS_HIGH();  // 拉高CS，取消选中（命令已发送完成）
}


/**
 * @brief  向Flash指定地址写入一页数据（最大256字节）
 * @param  addr：写入的起始地址（W25Q64地址是3字节，范围0x000000~0x7FFFFF）
 * @param  data：要写入的数据缓冲区指针
 * @param  len：写入的数据长度（不能超过256字节，否则只写前256字节）
 * @note   关键：Flash写入前必须先擦除（擦除后所有字节为0xFF），且地址不能跨页
 */
void W25Q64_PageProgram(uint32_t addr, const uint8_t *data, uint16_t len)
{
    // 边界保护：一页最大256字节，超过则截断
    if (len > 256) len = 256;

    // 1. 发送写使能（允许编程操作）
    W25Q64_WriteEnable();
    // 2. 拉低CS，选中Flash
    FLASH_CS_LOW();
    // 3. 发送“页编程”命令（0x02）
    W25Q64_SPI_SendReceiveByte(W25Q64J_PAGE_PROGRAM);
    // 4. 发送3字节起始地址（Flash地址是24位，分3次发送：高位→中位→低位）
    W25Q64_SPI_SendReceiveByte((addr >> 16) & 0xFF);  // 地址高位（第23~16位）
    W25Q64_SPI_SendReceiveByte((addr >> 8) & 0xFF);   // 地址中位（第15~8位）
    W25Q64_SPI_SendReceiveByte(addr & 0xFF);          // 地址低位（第7~0位）
    // 5. 循环发送要写入的数据（每次1字节）
    for (uint16_t i = 0; i < len; i++)
    {
        W25Q64_SPI_SendReceiveByte(data[i]);
    }
    // 6. 拉高CS，取消选中（触发编程操作开始）
    FLASH_CS_HIGH();
    // 7. 等待编程完成（编程耗时，必须等空闲）
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
    FLASH_CS_LOW();  // 拉低CS，选中Flash
    // 1. 发送“读数据”命令（0x03）
    W25Q64_SPI_SendReceiveByte(W25Q64J_READ_DATA);
    // 2. 发送3字节起始地址（高位→中位→低位）
    W25Q64_SPI_SendReceiveByte((addr >> 16) & 0xFF);
    W25Q64_SPI_SendReceiveByte((addr >> 8) & 0xFF);
    W25Q64_SPI_SendReceiveByte(addr & 0xFF);
    // 3. 循环读取数据（发送0xFF空字节，接收Flash返回的数据）
    for (uint16_t i = 0; i < len; i++)
    {
        data[i] = W25Q64_SPI_SendReceiveByte(0xFF);
    }
    FLASH_CS_HIGH();  // 拉高CS，取消选中
}

/**
 * @brief 擦除指定扇区（4KB大小，地址需按扇区对齐）
 * @param addr 要擦除扇区的起始地址
 */
void W25Q64_SectorErase(uint32_t addr)
{
    // 1. 发送写使能（允许擦除操作）
    W25Q64_WriteEnable();
    FLASH_CS_LOW();  // 拉低CS，选中Flash
    // 2. 发送“扇区擦除”命令（0x20）
    W25Q64_SPI_SendReceiveByte(W25Q64J_SECTOR_ERASE);
    // 3. 发送3字节扇区地址（只要是扇区内的任意地址，Flash都会擦除整个扇区）
    W25Q64_SPI_SendReceiveByte((addr >> 16) & 0xFF);
    W25Q64_SPI_SendReceiveByte((addr >> 8) & 0xFF);
    W25Q64_SPI_SendReceiveByte(addr & 0xFF);
    FLASH_CS_HIGH();  // 拉高CS，取消选中（触发擦除操作开始）
    W25Q64_WaitForIdle();  // 等待擦除完成（扇区擦除耗时约几十毫秒）
}

/**
 * @brief 擦除整个W25Q64芯片（8MB空间，耗时较长）
 */
void W25Q64_ChipErase(void)
{
    W25Q64_WriteEnable();  // 发送写使能
    FLASH_CS_LOW();
    W25Q64_SPI_SendReceiveByte(W25Q64J_CHIP_ERASE);  // 发送“整片擦除”命令（0xC7）
    FLASH_CS_HIGH();
    W25Q64_WaitForIdle();  // 等待整片擦除完成（耗时久，耐心等）
}


int main(void)
{
    // 测试用变量（串口打印演示）
    int a = 123;
    char str[] = "Hello USART!";

    // -------------------------- 1. 初始化串口（用于打印调试信息） --------------------------
    USART1_init();

    // 2. 串口打印测试（验证串口是否正常）
    printf("a = %d, str = %s\r\n", a, str);
    USART_Print(USART1, "USART_Print Test: num = %d, hex = 0x%X\r\n", a, a);
    LOG("USART\r\n");
    LOG("%s\r\n",str);
   
   
// -------------------------- 2. Flash测试核心变量 --------------------------
    uint8_t mfg_id;          // 制造商ID
    uint16_t dev_id;         // 设备ID
    const char *write_str = "hello flash";  // 要写入Flash的字符串（含终止符'\0'）
    uint16_t str_len = strlen(write_str);   // 字符串长度（含'\0'，方便后续读取后打印）
    char read_str[32] = {0};                // 存储从Flash读取的字符串（初始化全0）           // 存储读取的字符串

    // -------------------------- 3. Flash测试流程 --------------------------
    // 3.1 初始化SPI Flash（硬件准备）
    W25Q64_SPI_Init();

    // 3.2 读ID验证通信（第一步必须做，确认Flash是否连好）
    W25Q64_ReadID(&mfg_id, &dev_id);
    LOG("1. Flash ID 验证：\n");
    LOG("   MFG ID: 0x%02X, Dev ID: 0x%04X\n", mfg_id, dev_id);
    // 验证ID是否正确（W25Q64的标准ID：MFG=0xEF，Dev=0x4017）
    if (mfg_id != 0xEF || dev_id != 0x4017) {
        LOG("   错误：Flash通信失败，停止测试！\n");
        while(1);  // ID错，说明通信有问题，卡住等待调试
    }

    // 3.3 擦除目标扇区（写入前必须擦除，地址0x000000所在扇区）
    LOG("\n2. 擦除地址0x000000所在扇区...\n");
    W25Q64_SectorErase(0x000000);  // 擦除0x000000~0x000FFF的4KB扇区
    LOG("   扇区擦除完成\n");

    // 3.4 打印要写入的字符串信息
    LOG("\n3. 准备写入的字符串：\n");
    LOG("   内容：\"%s\"\n", write_str);
    LOG("   长度：%d字节（含终止符）\n", str_len);
    LOG("   十六进制：");
    for (uint16_t i = 0; i < str_len; i++) {
        LOG("0x%02X ", write_str[i]);  // 打印每个字符的十六进制值（如'h'是0x68）
    }
    LOG("\n");

    // 3.5 向Flash写入字符串（地址0x000000）
    LOG("\n4. 向地址0x000000写入数据...\n");
    W25Q64_PageProgram(0x000000, (uint8_t*)write_str, str_len);  // 强制转换为uint8_t*
    LOG("   写入完成\n");

    // 3.6 从Flash读取字符串（地址0x000000，读str_len字节）
    LOG("\n5. 从地址0x000000读取数据...\n");
    W25Q64_ReadData(0x000000, (uint8_t*)read_str, str_len);
    LOG("   读取完成\n");

    // 3.7 打印读取到的字符串信息
    LOG("\n6. 读取到的字符串：\n");
    LOG("   内容：\"%s\"\n", read_str);  // 若读取正确，会打印"hello flash"
    LOG("   长度：%d字节（含终止符）\n", strlen(read_str));
    LOG("   十六进制：");
    for (uint16_t i = 0; i < str_len; i++) {
    LOG("0x%02X ", read_str[i]);  // 与写入的十六进制值对比，应完全一致
    }
    LOG("\n");

    // 3.8 验证写入与读取是否一致（用strcmp比较两个字符串）
    if (strcmp(write_str, read_str) == 0) {
        LOG("\n7. 验证结果：【成功】写入与读取完全一致！\n");
    } else {
        LOG("\n7. 验证结果：【失败】数据不匹配！\n");
    }  
    
    // 主循环（测试完成后，停在这里）
    while (1)
    {
        // 可添加后续功能，如循环读写、数据存储等
    }
}
