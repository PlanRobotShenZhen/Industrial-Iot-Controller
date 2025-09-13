
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



// ����W25Q64JVSSQ�������
#define W25Q64J_READ_ID       0x9F
#define W25Q64J_WRITE_ENABLE  0x06
#define W25Q64J_WRITE_DISABLE 0x04
#define W25Q64J_PAGE_PROGRAM  0x02
#define W25Q64J_READ_DATA     0x03
#define W25Q64J_SECTOR_ERASE  0x20
#define W25Q64J_CHIP_ERASE    0xC7

// ����SPI���裨����ʹ��SPI3������ʵ��Ӳ�������޸ģ�
#define SPI_FLASH SPI3
// ���¶���Ƭѡ����
#define FLASH_CS_PORT GPIOD
#define FLASH_CS_PIN  GPIO_PIN_3

// Ƭѡ���ƺ�ҲҪ��Ӧ�޸�
#define FLASH_CS_LOW()  GPIO_ResetBits(FLASH_CS_PORT, FLASH_CS_PIN)
#define FLASH_CS_HIGH() GPIO_SetBits(FLASH_CS_PORT, FLASH_CS_PIN)


/**
 * @brief ��ʼ�����ڿ���W25Q64JVSSQ��SPI�����GPIO
 * @note ��Ҫ����ʵ��Ӳ����������SPI���Ÿ��á�ʱ�ӵ�
 */
void W25Q64_SPI_Init(void)
{
    
    SPI_InitType SPI_InitStructure;
    GPIO_InitType GPIO_InitStructure;

    // ʹ��SPI��GPIOʱ��
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_SPI3, ENABLE);
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOC | RCC_AHB_PERIPHEN_GPIOD, ENABLE);
   
    // ����SPI���ţ�SCK��MOSI��MISO������ʵ�������޸ģ�������SPI3Ĭ�����ž�����
    // SPI3_SCK -> PC10, SPI3_MOSI -> PC12, SPI3_MISO -> PC11
    GPIO_InitStructure.Pin        = GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF1;  // ���踴�ù���Ϊ GPIO_AF1
    GPIO_InitStructure.GPIO_Current = GPIO_DC_8mA;  // 8mA ������������ѡ 2/4/8/12mA���� GPIO ͷ�ļ���
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;  // ����Ҫ���� slew rate�����������   

    GPIO_InitPeripheral(GPIOC, &GPIO_InitStructure);
    // ����CS����Ϊ�������
    // ����CS���ţ�PD3��
    GPIO_InitStructure.Pin       = FLASH_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Current = GPIO_DC_8mA;  // 8mA��������
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;  // ���� slew rate
    GPIO_InitStructure.GPIO_Alternate = GPIO_NO_AF; 
    GPIO_InitPeripheral(FLASH_CS_PORT, &GPIO_InitStructure);
    FLASH_CS_HIGH(); // ��ʼ�øߣ�ȡ��Ƭѡ


    // ����SPI����
    // 5. ����SPI3������ƥ��W25Q64Ҫ��
    SPI_InitStructure.DataDirection = SPI_DIR_DOUBLELINE_FULLDUPLEX;          // ȫ˫��ģʽ
    SPI_InitStructure.SpiMode       = SPI_MODE_MASTER;                        // ����ģʽ��MCU����Flash��
    SPI_InitStructure.DataLen       = SPI_DATA_SIZE_8BITS;                    // 8λ���ݣ�W25Q64����/���ݾ�Ϊ8λ��
    SPI_InitStructure.CLKPOL        = SPI_CLKPOL_LOW;                         // ʱ�ӿ��еͣ�SPIģʽ0��W25Q64Ψһ֧�֣�
    SPI_InitStructure.CLKPHA        = SPI_CLKPHA_FIRST_EDGE;                  // �����ز�����SPIģʽ0��
    SPI_InitStructure.NSS           = SPI_NSS_SOFT;                           // �������Ƭѡ����GPIO��CS���ţ�����Ӳ��NSS��
    SPI_InitStructure.BaudRatePres  = SPI_BR_PRESCALER_16;                    // �����ʷ�Ƶ������APB1=42MHz��42/16��2.6MHz�����ٵ��Ը��ȶ���
    SPI_InitStructure.FirstBit      = SPI_FB_MSB;                             // ��λ��ǰ��SPIĬ�ϣ�W25Q64֧�֣�
    SPI_InitStructure.CRCPoly       = 7;                                      // CRCУ�飨Ĭ��7���ɺ��ԣ�

    SPI_Init(SPI_FLASH, &SPI_InitStructure);
    SPI_Enable(SPI_FLASH, ENABLE);
}

/**
 * @brief ��SPI���跢��һ���ֽ����ݣ������շ��ص�һ���ֽ�����
 * @param data Ҫ���͵�����
 * @return ���յ�������
 */
uint8_t W25Q64_SPI_SendReceiveByte(uint8_t data)
{
    while (SPI_I2S_GetStatus(SPI_FLASH, SPI_I2S_TE_FLAG) == RESET); // �ȴ����ͻ�������
    SPI_I2S_TransmitData(SPI_FLASH, data);
    while (SPI_I2S_GetStatus(SPI_FLASH, SPI_I2S_RNE_FLAG) == RESET); // �ȴ����ջ�����������
    return SPI_I2S_ReceiveData(SPI_FLASH);
}

/**
 * @brief �ȴ�W25Q64JVSSQæ��־λ�������������̲���ʱ���ã�
 */
void W25Q64_WaitForIdle(void)
{
    uint8_t status = 0x01;
    FLASH_CS_LOW();
    W25Q64_SPI_SendReceiveByte(0x05); // ���Ͷ�״̬�Ĵ�������
    while (status & 0x01)
    {
        status = W25Q64_SPI_SendReceiveByte(0xFF);
    }
    FLASH_CS_HIGH();
}

/**
 * @brief ��ȡW25Q64JVSSQ��������ID���豸ID
 * @param[out] mfg_id �洢������ID�ı���ָ��
 * @param[out] dev_id �洢�豸ID�ı���ָ��
 */
void W25Q64_ReadID(uint8_t *mfg_id, uint16_t *dev_id)
{
    uint8_t id_buf[3];
    FLASH_CS_LOW();
    W25Q64_SPI_SendReceiveByte(W25Q64J_READ_ID);
    id_buf[0] = W25Q64_SPI_SendReceiveByte(0xFF); // ������ID
    id_buf[1] = W25Q64_SPI_SendReceiveByte(0xFF); // �豸ID��λ
    id_buf[2] = W25Q64_SPI_SendReceiveByte(0xFF); // �豸ID��λ
    FLASH_CS_HIGH();

    if (mfg_id) *mfg_id = id_buf[0];
    if (dev_id) *dev_id = (id_buf[1] << 8) | id_buf[2];  // �ϲ���λ+��λ
}


/**
 * @brief ����дʹ�������������ı�̡���������
 */
void W25Q64_WriteEnable(void)
{
    FLASH_CS_LOW();
    W25Q64_SPI_SendReceiveByte(W25Q64J_WRITE_ENABLE);
    FLASH_CS_HIGH();
}


/**
 * @brief ��ָ����ַд��һҳ�����256�ֽڣ�����
 * @param addr Ҫд�����ʼ��ַ
 * @param data Ҫд������ݻ�����ָ��
 * @param len  Ҫд������ݳ��ȣ����ܳ���256�ֽڣ�
 */
void W25Q64_PageProgram(uint32_t addr, const uint8_t *data, uint16_t len)
{
    if (len > 256) len = 256; // һҳ���256�ֽ�

    W25Q64_WriteEnable();
    FLASH_CS_LOW();
    W25Q64_SPI_SendReceiveByte(W25Q64J_PAGE_PROGRAM);
    // ����3�ֽڵ�ַ
    W25Q64_SPI_SendReceiveByte((addr >> 16) & 0xFF);
    W25Q64_SPI_SendReceiveByte((addr >> 8) & 0xFF);
    W25Q64_SPI_SendReceiveByte(addr & 0xFF);
    // ��������
    for (uint16_t i = 0; i < len; i++)
    {
        W25Q64_SPI_SendReceiveByte(data[i]);
    }
    FLASH_CS_HIGH();
    W25Q64_WaitForIdle();
}


/**
 * @brief ��ָ����ַд��һҳ�����256�ֽڣ�����
 * @param addr Ҫд�����ʼ��ַ
 * @param data Ҫд������ݻ�����ָ��
 * @param len  Ҫд������ݳ��ȣ����ܳ���256�ֽڣ�
 */


/**
 * @brief ��ָ����ַ��ȡ����
 * @param addr Ҫ��ȡ����ʼ��ַ
 * @param data �洢��ȡ���ݵĻ�����ָ��
 * @param len  Ҫ��ȡ�����ݳ���
 */
void W25Q64_ReadData(uint32_t addr, uint8_t *data, uint16_t len)
{
    FLASH_CS_LOW();
    W25Q64_SPI_SendReceiveByte(W25Q64J_READ_DATA);
    // ����3�ֽڵ�ַ
    W25Q64_SPI_SendReceiveByte((addr >> 16) & 0xFF);
    W25Q64_SPI_SendReceiveByte((addr >> 8) & 0xFF);
    W25Q64_SPI_SendReceiveByte(addr & 0xFF);
    // ��������
    for (uint16_t i = 0; i < len; i++)
    {
        data[i] = W25Q64_SPI_SendReceiveByte(0xFF); // ����0xFF��ȡ����
    }
    FLASH_CS_HIGH();
}

/**
 * @brief ����ָ��������4KB��С����ַ�谴�������룩
 * @param addr Ҫ������������ʼ��ַ
 */
void W25Q64_SectorErase(uint32_t addr)
{
    W25Q64_WriteEnable();
    FLASH_CS_LOW();
    W25Q64_SPI_SendReceiveByte(W25Q64J_SECTOR_ERASE);
    // ����3�ֽڵ�ַ
    W25Q64_SPI_SendReceiveByte((addr >> 16) & 0xFF);
    W25Q64_SPI_SendReceiveByte((addr >> 8) & 0xFF);
    W25Q64_SPI_SendReceiveByte(addr & 0xFF);
    FLASH_CS_HIGH();
    W25Q64_WaitForIdle();
}

/**
 * @brief ��������W25Q64оƬ��8MB�ռ䣬��ʱ�ϳ���
 */
void W25Q64_ChipErase(void)
{
    W25Q64_WriteEnable();
    FLASH_CS_LOW();
    W25Q64_SPI_SendReceiveByte(W25Q64J_CHIP_ERASE);
    FLASH_CS_HIGH();
    W25Q64_WaitForIdle();
}

