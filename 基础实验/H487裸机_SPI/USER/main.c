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



// -------------------------- W25Q64 ����궨�� --------------------------
// W25Q64��SPI FlashоƬ�����в�����ͨ�����͡����ʵ�֣�����ֵ��оƬ�ֲ�涨��
#define W25Q64J_READ_ID       0x9F  // ��������ID���豸ID����֤оƬ�Ƿ��������ӣ�
#define W25Q64J_WRITE_ENABLE  0x06  // дʹ�ܣ�����/���ǰ���뷢��������������Ч��
#define W25Q64J_WRITE_DISABLE 0x04  // д���ã���ֹ����/��̣����ڱ������ݣ�
#define W25Q64J_PAGE_PROGRAM  0x02  // ҳ��̣���Flashд�����ݣ�һҳ���256�ֽڣ�
#define W25Q64J_READ_DATA     0x03  // �����ݣ���Flashָ����ַ��ȡ���ݣ�
#define W25Q64J_SECTOR_ERASE  0x20  // ��������������4KB���ݣ�Flashд��ǰ�����Ȳ�����
#define W25Q64J_CHIP_ERASE    0xC7  // ��Ƭ��������������оƬ8MB���ݣ���ʱ�ϳ���


// -------------------------- Ӳ�����Ӻ궨�� --------------------------
// 1. SPI���裺ʹ��SPI3���ɸ���ʵ��Ӳ���޸�ΪSPI1/SPI2��
#define SPI_FLASH SPI3
// 2. Ƭѡ���ţ�CS��������Flash�Ƿ�ѡ�У��͵�ƽѡ�У��ߵ�ƽȡ��ѡ�У�
#define FLASH_CS_PORT GPIOD  // CS�������ڶ˿ڣ�GPIOD
#define FLASH_CS_PIN  GPIO_PIN_3  // CS���ű�ţ�PD3

// 3. Ƭѡ���ƺ꣨�򻯴��룬ֱ�ӵ��ü��ɿ���CS��ƽ��
#define FLASH_CS_LOW()  GPIO_ResetBits(FLASH_CS_PORT, FLASH_CS_PIN)  // CS���ͣ�ѡ��Flash��
#define FLASH_CS_HIGH() GPIO_SetBits(FLASH_CS_PORT, FLASH_CS_PIN)    // CS���ߣ�ȡ��ѡ�У�

/**
 * @brief  ��ʼ��W25Q64�����SPI�����GPIO����
 * @note   ���ģ�SPI����������W25Q64��Ҫ��ƥ�䣨��SPIģʽ0��8λ���ݡ���λ��ǰ��
 */
void W25Q64_SPI_Init(void)
{
    
    SPI_InitType SPI_InitStructure;
    GPIO_InitType GPIO_InitStructure;

// -------------------------- 1. ʹ������ʱ�ӣ���Ӳ���ϵ磩 --------------------------
    // ʹ��SPI3ʱ�ӣ�SPI3������APB1���ߣ�����RCC_EnableAPB1PeriphClk��
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_SPI3, ENABLE);
    // ʹ��GPIOʱ�ӣ�GPIOC��SPI3��SCK/MOSI/MISO���ţ���GPIOD��CS���ţ�
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOC | RCC_AHB_PERIPHEN_GPIOD, ENABLE);
   
// -------------------------- 2. ����SPI���ţ�SCK/MOSI/MISO�� --------------------------
    // SPI3_SCK -> PC10, SPI3_MOSI -> PC12, SPI3_MISO -> PC11
    GPIO_InitStructure.Pin        = GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF1;  // ���踴�ù���Ϊ GPIO_AF1
    GPIO_InitStructure.GPIO_Current = GPIO_DC_8mA;  // 8mA ������������ѡ 2/4/8/12mA���� GPIO ͷ�ļ���
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;  // ����Ҫ���� slew rate�����������   

    // ������Ӧ�õ�GPIOC�˿ڣ�ʹSPI������Ч��
    GPIO_InitPeripheral(GPIOC, &GPIO_InitStructure);
// -------------------------- 3. ����CS���ţ�Ƭѡ���ƣ� --------------------------
    // ����CS���ţ�PD3��
    GPIO_InitStructure.Pin       = FLASH_CS_PIN;// ѡ��PD3����
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;// ��ͨ���������CS�����ֱ�ӿ��ƣ��Ǹ��ã�
    GPIO_InitStructure.GPIO_Current = GPIO_DC_8mA;  // 8mA��������
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;  // ���� slew rate
    GPIO_InitStructure.GPIO_Alternate = GPIO_NO_AF;  // ��ʹ�ø��ù��ܣ���ͨGPIO��
    // ������Ӧ�õ�GPIOD�˿ڣ�ʹCS������Ч��
    GPIO_InitPeripheral(FLASH_CS_PORT, &GPIO_InitStructure);
    FLASH_CS_HIGH(); // ��ʼ�øߣ�ȡ��Ƭѡ

// -------------------------- 4. ����SPI�������ؼ�������ƥ��FlashҪ�� --------------------------
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

    // ��SPI����Ӧ�õ�SPI3���裨ʹSPI������Ч��
    SPI_Init(SPI_FLASH, &SPI_InitStructure);
    // ʹ��SPI3��SPI������ɺ󣬱���ʹ�ܲ��ܹ�����
    SPI_Enable(SPI_FLASH, ENABLE);
}

/**
 * @brief ��SPI���跢��һ���ֽ����ݣ������շ��ص�һ���ֽ�����
 * @param data Ҫ���͵�����
 * @return ���յ�������
 */
uint8_t W25Q64_SPI_SendReceiveByte(uint8_t data)
{
    // 1. �ȴ�SPI���ͻ�����Ϊ�գ�ȷ��֮ǰ�������ѷ��꣬���ܷ������ݣ�
    while (SPI_I2S_GetStatus(SPI_FLASH, SPI_I2S_TE_FLAG) == RESET); 
    // 2. ����1���ֽڵ�Flash����dataд��SPI���ͼĴ�����
    SPI_I2S_TransmitData(SPI_FLASH, data);
    // 3. �ȴ�SPI���ջ����������ݣ�ȷ��Flash�ѷ������ݣ�
    while (SPI_I2S_GetStatus(SPI_FLASH, SPI_I2S_RNE_FLAG) == RESET);
    // 4. ��ȡ���ջ����������ݣ�����Flash�������ֽڣ�    
    return SPI_I2S_ReceiveData(SPI_FLASH);
}

/**
 * @brief �ȴ�W25Q64JVSSQæ��־λ�������������̲���ʱ���ã�
 */
void W25Q64_WaitForIdle(void)
{
      uint8_t status = 0x01;  // ��ʼֵ��Ϊ1����ʾæ��
    FLASH_CS_LOW();  // ����CS��ѡ��Flash��Ҫ��״̬��������ѡ�У�
    
    // ���͡���״̬�Ĵ���1�����0x05��оƬ�ֲ�涨��
    W25Q64_SPI_SendReceiveByte(0x05);
    
    // ѭ����ȡ״̬�Ĵ�����bit0��æ��־��1=æ��0=���У�
    while (status & 0x01)
    {
        // ����0xFF�����ֽڣ�������״̬�Ĵ�����ֵ
        status = W25Q64_SPI_SendReceiveByte(0xFF);
    }
    
    FLASH_CS_HIGH();  // ����CS��ȡ��ѡ��Flash
}

/**
 * @brief ��ȡW25Q64JVSSQ��������ID���豸ID
 * @param[out] mfg_id �洢������ID�ı���ָ��
 * @param[out] dev_id �洢�豸ID�ı���ָ��
 */
void W25Q64_ReadID(uint8_t *mfg_id, uint16_t *dev_id)
{
    uint8_t id_buf[3];  // �洢3�ֽ�ID��1�ֽ�������ID + 2�ֽ��豸ID��
    FLASH_CS_LOW();  // ����CS��ѡ��Flash
    
    // ���͡���ID�����0x9F��
    W25Q64_SPI_SendReceiveByte(W25Q64J_READ_ID);
    // ����3�ֽ�ID������0xFF���ֽڣ���ȡID��
    id_buf[0] = W25Q64_SPI_SendReceiveByte(0xFF);  // ��1�ֽڣ�������ID��0xEF��
    id_buf[1] = W25Q64_SPI_SendReceiveByte(0xFF);  // ��2�ֽڣ��豸ID��λ��0x40��
    id_buf[2] = W25Q64_SPI_SendReceiveByte(0xFF);  // ��3�ֽڣ��豸ID��λ��0x17��
    
    FLASH_CS_HIGH();  // ����CS��ȡ��ѡ��

    // ��ID��ֵ���ⲿ�������ж�ָ��ǿգ����������
    if (mfg_id) *mfg_id = id_buf[0];
    if (dev_id) *dev_id = (id_buf[1] << 8) | id_buf[2];  // �ϲ��ߵ�λ���õ�16λ�豸ID
}


/**
 * @brief ����дʹ�������������ı�̡���������
 */
void W25Q64_WriteEnable(void)
{
    FLASH_CS_LOW();  // ����CS��ѡ��Flash
    W25Q64_SPI_SendReceiveByte(W25Q64J_WRITE_ENABLE);  // ����дʹ�����0x06��
    FLASH_CS_HIGH();  // ����CS��ȡ��ѡ�У������ѷ�����ɣ�
}


/**
 * @brief  ��Flashָ����ַд��һҳ���ݣ����256�ֽڣ�
 * @param  addr��д�����ʼ��ַ��W25Q64��ַ��3�ֽڣ���Χ0x000000~0x7FFFFF��
 * @param  data��Ҫд������ݻ�����ָ��
 * @param  len��д������ݳ��ȣ����ܳ���256�ֽڣ�����ֻдǰ256�ֽڣ�
 * @note   �ؼ���Flashд��ǰ�����Ȳ����������������ֽ�Ϊ0xFF�����ҵ�ַ���ܿ�ҳ
 */
void W25Q64_PageProgram(uint32_t addr, const uint8_t *data, uint16_t len)
{
    // �߽籣����һҳ���256�ֽڣ�������ض�
    if (len > 256) len = 256;

    // 1. ����дʹ�ܣ������̲�����
    W25Q64_WriteEnable();
    // 2. ����CS��ѡ��Flash
    FLASH_CS_LOW();
    // 3. ���͡�ҳ��̡����0x02��
    W25Q64_SPI_SendReceiveByte(W25Q64J_PAGE_PROGRAM);
    // 4. ����3�ֽ���ʼ��ַ��Flash��ַ��24λ����3�η��ͣ���λ����λ����λ��
    W25Q64_SPI_SendReceiveByte((addr >> 16) & 0xFF);  // ��ַ��λ����23~16λ��
    W25Q64_SPI_SendReceiveByte((addr >> 8) & 0xFF);   // ��ַ��λ����15~8λ��
    W25Q64_SPI_SendReceiveByte(addr & 0xFF);          // ��ַ��λ����7~0λ��
    // 5. ѭ������Ҫд������ݣ�ÿ��1�ֽڣ�
    for (uint16_t i = 0; i < len; i++)
    {
        W25Q64_SPI_SendReceiveByte(data[i]);
    }
    // 6. ����CS��ȡ��ѡ�У�������̲�����ʼ��
    FLASH_CS_HIGH();
    // 7. �ȴ������ɣ���̺�ʱ������ȿ��У�
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
    FLASH_CS_LOW();  // ����CS��ѡ��Flash
    // 1. ���͡������ݡ����0x03��
    W25Q64_SPI_SendReceiveByte(W25Q64J_READ_DATA);
    // 2. ����3�ֽ���ʼ��ַ����λ����λ����λ��
    W25Q64_SPI_SendReceiveByte((addr >> 16) & 0xFF);
    W25Q64_SPI_SendReceiveByte((addr >> 8) & 0xFF);
    W25Q64_SPI_SendReceiveByte(addr & 0xFF);
    // 3. ѭ����ȡ���ݣ�����0xFF���ֽڣ�����Flash���ص����ݣ�
    for (uint16_t i = 0; i < len; i++)
    {
        data[i] = W25Q64_SPI_SendReceiveByte(0xFF);
    }
    FLASH_CS_HIGH();  // ����CS��ȡ��ѡ��
}

/**
 * @brief ����ָ��������4KB��С����ַ�谴�������룩
 * @param addr Ҫ������������ʼ��ַ
 */
void W25Q64_SectorErase(uint32_t addr)
{
    // 1. ����дʹ�ܣ��������������
    W25Q64_WriteEnable();
    FLASH_CS_LOW();  // ����CS��ѡ��Flash
    // 2. ���͡��������������0x20��
    W25Q64_SPI_SendReceiveByte(W25Q64J_SECTOR_ERASE);
    // 3. ����3�ֽ�������ַ��ֻҪ�������ڵ������ַ��Flash�����������������
    W25Q64_SPI_SendReceiveByte((addr >> 16) & 0xFF);
    W25Q64_SPI_SendReceiveByte((addr >> 8) & 0xFF);
    W25Q64_SPI_SendReceiveByte(addr & 0xFF);
    FLASH_CS_HIGH();  // ����CS��ȡ��ѡ�У���������������ʼ��
    W25Q64_WaitForIdle();  // �ȴ�������ɣ�����������ʱԼ��ʮ���룩
}

/**
 * @brief ��������W25Q64оƬ��8MB�ռ䣬��ʱ�ϳ���
 */
void W25Q64_ChipErase(void)
{
    W25Q64_WriteEnable();  // ����дʹ��
    FLASH_CS_LOW();
    W25Q64_SPI_SendReceiveByte(W25Q64J_CHIP_ERASE);  // ���͡���Ƭ���������0xC7��
    FLASH_CS_HIGH();
    W25Q64_WaitForIdle();  // �ȴ���Ƭ������ɣ���ʱ�ã����ĵȣ�
}


int main(void)
{
    // �����ñ��������ڴ�ӡ��ʾ��
    int a = 123;
    char str[] = "Hello USART!";

    // -------------------------- 1. ��ʼ�����ڣ����ڴ�ӡ������Ϣ�� --------------------------
    USART1_init();

    // 2. ���ڴ�ӡ���ԣ���֤�����Ƿ�������
    printf("a = %d, str = %s\r\n", a, str);
    USART_Print(USART1, "USART_Print Test: num = %d, hex = 0x%X\r\n", a, a);
    LOG("USART\r\n");
    LOG("%s\r\n",str);
   
   
// -------------------------- 2. Flash���Ժ��ı��� --------------------------
    uint8_t mfg_id;          // ������ID
    uint16_t dev_id;         // �豸ID
    const char *write_str = "hello flash";  // Ҫд��Flash���ַ���������ֹ��'\0'��
    uint16_t str_len = strlen(write_str);   // �ַ������ȣ���'\0'�����������ȡ���ӡ��
    char read_str[32] = {0};                // �洢��Flash��ȡ���ַ�������ʼ��ȫ0��           // �洢��ȡ���ַ���

    // -------------------------- 3. Flash�������� --------------------------
    // 3.1 ��ʼ��SPI Flash��Ӳ��׼����
    W25Q64_SPI_Init();

    // 3.2 ��ID��֤ͨ�ţ���һ����������ȷ��Flash�Ƿ����ã�
    W25Q64_ReadID(&mfg_id, &dev_id);
    LOG("1. Flash ID ��֤��\n");
    LOG("   MFG ID: 0x%02X, Dev ID: 0x%04X\n", mfg_id, dev_id);
    // ��֤ID�Ƿ���ȷ��W25Q64�ı�׼ID��MFG=0xEF��Dev=0x4017��
    if (mfg_id != 0xEF || dev_id != 0x4017) {
        LOG("   ����Flashͨ��ʧ�ܣ�ֹͣ���ԣ�\n");
        while(1);  // ID��˵��ͨ�������⣬��ס�ȴ�����
    }

    // 3.3 ����Ŀ��������д��ǰ�����������ַ0x000000����������
    LOG("\n2. ������ַ0x000000��������...\n");
    W25Q64_SectorErase(0x000000);  // ����0x000000~0x000FFF��4KB����
    LOG("   �����������\n");

    // 3.4 ��ӡҪд����ַ�����Ϣ
    LOG("\n3. ׼��д����ַ�����\n");
    LOG("   ���ݣ�\"%s\"\n", write_str);
    LOG("   ���ȣ�%d�ֽڣ�����ֹ����\n", str_len);
    LOG("   ʮ�����ƣ�");
    for (uint16_t i = 0; i < str_len; i++) {
        LOG("0x%02X ", write_str[i]);  // ��ӡÿ���ַ���ʮ������ֵ����'h'��0x68��
    }
    LOG("\n");

    // 3.5 ��Flashд���ַ�������ַ0x000000��
    LOG("\n4. ���ַ0x000000д������...\n");
    W25Q64_PageProgram(0x000000, (uint8_t*)write_str, str_len);  // ǿ��ת��Ϊuint8_t*
    LOG("   д�����\n");

    // 3.6 ��Flash��ȡ�ַ�������ַ0x000000����str_len�ֽڣ�
    LOG("\n5. �ӵ�ַ0x000000��ȡ����...\n");
    W25Q64_ReadData(0x000000, (uint8_t*)read_str, str_len);
    LOG("   ��ȡ���\n");

    // 3.7 ��ӡ��ȡ�����ַ�����Ϣ
    LOG("\n6. ��ȡ�����ַ�����\n");
    LOG("   ���ݣ�\"%s\"\n", read_str);  // ����ȡ��ȷ�����ӡ"hello flash"
    LOG("   ���ȣ�%d�ֽڣ�����ֹ����\n", strlen(read_str));
    LOG("   ʮ�����ƣ�");
    for (uint16_t i = 0; i < str_len; i++) {
    LOG("0x%02X ", read_str[i]);  // ��д���ʮ������ֵ�Աȣ�Ӧ��ȫһ��
    }
    LOG("\n");

    // 3.8 ��֤д�����ȡ�Ƿ�һ�£���strcmp�Ƚ������ַ�����
    if (strcmp(write_str, read_str) == 0) {
        LOG("\n7. ��֤��������ɹ���д�����ȡ��ȫһ�£�\n");
    } else {
        LOG("\n7. ��֤�������ʧ�ܡ����ݲ�ƥ�䣡\n");
    }  
    
    // ��ѭ����������ɺ�ͣ�����
    while (1)
    {
        // ����Ӻ������ܣ���ѭ����д�����ݴ洢��
    }
}
