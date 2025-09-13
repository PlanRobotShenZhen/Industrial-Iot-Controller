#include "i2c.h"
//#include "easyinit.h"
#include "init.h"
#include "FreeRTOS.h"
//#include "queue.h"
//#include "userdef.h"
#include "usart.h"
SI2C_Init_t SI2C_Init_Type[1];
#define USART_LOG       USART4

// CRC8У�麯��������SHT40������ʽ��0x31����ʼֵ��0xFF���޷���
uint8_t CRC8_Caculate(uint16_t data) 
{
    uint8_t crc = 0xFF;  // ��ʼֵ
    uint8_t buf[2] = {0};
    buf[0] = (uint8_t)(data >> 8);  // ��λ�ֽ�
    buf[1] = (uint8_t)(data & 0xFF); // ��λ�ֽ�

    for (int i = 0; i < 2; i++) {
        crc ^= buf[i];  // ����ʼֵ
        for (int j = 0; j < 8; j++) {
            crc = (crc & 0x80) ? (crc << 1) ^ 0x31 : (crc << 1); // ����ʽ0x31
        }
    }
    return crc;
}

// ��ʪ�ȶ�ȡ����������������I2C������
bool Read_TempHumi_I2C(float *pTemp, float *pHumi) {
    uint8_t recv_buf[6] = {0};  // �洢SHT40���ص�6�ֽ�����
    uint16_t temp_raw = 0, humi_raw = 0;

    // 1. ͨ��I2C��ȡSHT40���ݣ�x=0��I2C����0�����ӵ�ַ0x44������0xFD����ȡ6�ֽ�
    if (SI2C_ReadData(0, 0x44, 0xFD, recv_buf, 6) != 0) {
        USART_Print(USART_LOG, "I2C Read Failed!\r\n");  // ��ȡʧ�ܴ�ӡ
        return false;
    }

    // 2. ��ȡԭʼ���ݣ�ƴ�Ӹ�λ+��λ��
    temp_raw = (recv_buf[0] << 8) | recv_buf[1];  // �¶�ԭʼֵ��16λ��
    humi_raw = (recv_buf[3] << 8) | recv_buf[4];  // ʪ��ԭʼֵ��16λ��

    // 3. CRCУ�飨��֤������ȷ�ԣ�
    if (recv_buf[2] != CRC8_Caculate(temp_raw) || recv_buf[5] != CRC8_Caculate(humi_raw)) {
        USART_Print(USART_LOG, "Data CRC Error!\r\n");  // CRC�����ӡ
        return false;
    }

    // 4. ����Ϊʵ����ʪ�ȣ�SHT40��׼��ʽ��
    if (pTemp != NULL) {
        *pTemp = temp_raw * 175.0f / 65535.0f - 45.0f;  // �¶ȷ�Χ��-45�� ~ 130��
    }
    if (pHumi != NULL) {
        *pHumi = humi_raw * 125.0f / 65535.0f - 6.0f;   // ʪ�ȷ�Χ��0%RH ~ 100%RH
    }

    return true;
}


void delay_loop_us(uint32_t us) {
    uint32_t cycles = us * (168000000UL / 1000000UL / 5);  // 5��ָ������Լ1us
    while (cycles--) {
        __NOP();  // ��ָ�����1������
    }
}

void SI2C1_Configuration(void)
{
	#define x 0
	
	SI2C_Init_Type[x] = (SI2C_Init_t)
	{
		.SCL_M = GPIOA,
		.SDA_M = GPIOA,
		.SCL_Pin =GPIO_PIN_12,
		.SDA_Pin =GPIO_PIN_11,
	};
	SI2C_INIT(SI2C_Init_Type[x]);
	
	#undef x
}
void SDA_InputMode(uint8_t x)
{
    GPIO_InitType GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_MODE_INPUT; 
	GPIO_InitStruct.GPIO_Pull = GPIO_PULL_DOWN;
	GPIO_InitStruct.GPIO_Current =GPIO_DC_12mA;
	GPIO_InitStruct.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
	GPIO_InitStruct.Pin = SI2C_Init_Type[x].SDA_Pin ;
    GPIO_InitPeripheral(SI2C_Init_Type[x].SDA_M ,&GPIO_InitStruct);
}
void SDA_OutputMode(uint8_t x)
{
    GPIO_InitType GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_MODE_OUTPUT_PP; 
	GPIO_InitStruct.GPIO_Pull = GPIO_PULL_DOWN;
	GPIO_InitStruct.GPIO_Current =GPIO_DC_12mA;
	GPIO_InitStruct.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
	GPIO_InitStruct.Pin = SI2C_Init_Type[x].SDA_Pin ;
    GPIO_InitPeripheral(SI2C_Init_Type[x].SDA_M ,&GPIO_InitStruct);
}
void SI2C_delay(uint32_t us)
{
	delay_loop_us(us);
}
void SDA_H(uint8_t x)
{
	GPIO_WriteBits(SI2C_Init_Type[x].SDA_M, SI2C_Init_Type[x].SDA_Pin, Bit_SET);
}
void SCL_H(uint8_t x)
{
	GPIO_WriteBits(SI2C_Init_Type[x].SCL_M, SI2C_Init_Type[x].SCL_Pin, Bit_SET);
}
void SDA_L(uint8_t x)
{
	GPIO_WriteBits(SI2C_Init_Type[x].SDA_M, SI2C_Init_Type[x].SDA_Pin, Bit_RESET);
}
uint8_t SDA_R(uint8_t x)
{
    SDA_InputMode(x);
    uint8_t ret = GPIO_ReadInputDataBit(SI2C_Init_Type[x].SDA_M, SI2C_Init_Type[x].SDA_Pin);
    SDA_OutputMode(x);
	return ret;
}

void SCL_L(uint8_t x)
{
	GPIO_WriteBits(SI2C_Init_Type[x].SCL_M, SI2C_Init_Type[x].SCL_Pin, Bit_RESET);
}
void SI2C_Start(uint8_t x)
{
	SDA_H(x);
	SCL_H(x);
	SI2C_delay(5);
	SDA_L(x);
	SI2C_delay(5);
	SCL_L(x);
}
void SI2C_Stop(uint8_t x)
{
	SDA_L(x);
	SCL_H(x);
	SI2C_delay(5);
	SDA_H(x);
	SI2C_delay(5);
}
// ����һ���ֽ�����
void SI2C_SendByte(uint32_t x, uint8_t Byte)
{
    uint8_t i;
    for (i = 0; i < 8; i++)
	{
        if (Byte & 0x80)
		{
            SDA_H(x);
        }
		else
		{
            SDA_L(x);
        }
        Byte <<= 1;
        SI2C_delay(2);
        SCL_H(x);
        SI2C_delay(5);
        SCL_L(x);
        SI2C_delay(2);
    }
}

// ����һ���ֽ�����
uint8_t SI2C_ReceiveByte(uint32_t x)
{
    uint8_t i, Byte = 0;
    
    SDA_InputMode(x);
    for (i = 0; i < 8; i++)
    {
        SCL_H(x);
        SI2C_delay(5);
        Byte <<= 1;
        if (SDA_R(x))
        {
            Byte |= 0x01;
        }
        SCL_L(x);
        SI2C_delay(2);
    }

    return Byte;
}
// ����Ӧ���ź�
void SI2C_SendACK(uint32_t x)
{
    SDA_L(x);
    SI2C_delay(2);
    SCL_H(x);
    SI2C_delay(5);
    SCL_L(x);
    SI2C_delay(2);
    SDA_H(x);
}

// ���ͷ�Ӧ���ź�
void SI2C_SendNACK(uint32_t x)
{
    SDA_H(x);
    SI2C_delay(2);
    SCL_H(x);
    SI2C_delay(5);
    SCL_L(x);
    SI2C_delay(2);
}

// �ȴ�Ӧ���ź�
uint8_t SI2C_WaitACK(uint32_t x)
{
    uint8_t Ack;
	
    SCL_H(x);
    SI2C_delay(5);
    Ack = SDA_R(x);
    SCL_L(x);
    SI2C_delay(2);

    return Ack;
}

// д���ݵ����豸
uint8_t SI2C_WriteData(uint8_t x, uint8_t SlaveAddr, uint8_t RegAddr, uint8_t* Data, uint32_t Num)
{
    SI2C_Start(x);
    SI2C_SendByte(x, SlaveAddr << 1);
    if (SI2C_WaitACK(x))
    {
        SI2C_Stop(x);
        return 1;
    }
    SI2C_SendByte(x, RegAddr);
    if (SI2C_WaitACK(x))
    {
        SI2C_Stop(x);
        return 1;
    }
	for (uint32_t i = 0; i < Num; i++)
	{
		SI2C_SendByte(x, Data[i]);
		if (SI2C_WaitACK(x))
		{
			SI2C_Stop(x);
			return 1;
		}
	}
    SI2C_Stop(x);
    return 0;
}

// �Ӵ��豸��ȡ����
uint8_t SI2C_ReadData(uint8_t x, uint8_t SlaveAddr, uint8_t RegAddr, uint8_t *Data, uint32_t Num)
{
    SI2C_Start(x);
    SI2C_SendByte(x, SlaveAddr << 1);
    if (SI2C_WaitACK(x))
    {
        SI2C_Stop(x);
        return 1;
    }
    SI2C_SendByte(x, RegAddr);
    if (SI2C_WaitACK(x))
    {
        SI2C_Stop(x);
        return 1;
    }
	
    SI2C_delay(10000);
    SI2C_Start(x);
    SI2C_SendByte(x, (SlaveAddr << 1) | 0x01);
    if (SI2C_WaitACK(x))
    {
        SI2C_Stop(x);
        return 1;
    }
	for (uint32_t i = 0; i < Num - 1; i++)
	{
		Data[i] = SI2C_ReceiveByte(x);
		SI2C_SendACK(x);
	}
	Data[Num - 1] = SI2C_ReceiveByte(x);
    SI2C_SendNACK(x);
	
    SI2C_Stop(x);
    return 0;
}
