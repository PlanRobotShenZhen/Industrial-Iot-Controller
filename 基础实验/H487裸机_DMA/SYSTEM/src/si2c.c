#include "si2c.h"

SI2C_Init_t SI2C_Init_Type[1];

void SI2C1_Configuration(void)
{
	#define x 0
	
	SI2C_Init_Type[x] = (SI2C_Init_t)
	{
		.SCL_M = GPIOC,
		.SDA_M = GPIOB,
		.SCL_Pin =GPIO_PIN_0,
		.SDA_Pin =GPIO_PIN_5,
	};
	SI2C_INIT(SI2C_Init_Type[x]);
	
	
	#undef x
}

void SI2C_Delay(uint32_t us)
{
	Delay_us(us);
}
void SDA_H(uint32_t x)
{
	GPIO_WriteBits(SI2C_Init_Type[x].SDA_M, SI2C_Init_Type[x].SDA_Pin, Bit_SET);
}
void SCL_H(uint32_t x)
{
	GPIO_WriteBits(SI2C_Init_Type[x].SCL_M, SI2C_Init_Type[x].SCL_Pin, Bit_SET);
}
void SDA_L(uint32_t x)
{
	GPIO_WriteBits(SI2C_Init_Type[x].SDA_M, SI2C_Init_Type[x].SDA_Pin, Bit_RESET);
}
uint8_t SDA_R(uint32_t x)
{
	return GPIO_ReadInputDataBit(SI2C_Init_Type[x].SDA_M, SI2C_Init_Type[x].SDA_Pin);
}
void SCL_L(uint32_t x)
{
	GPIO_WriteBits(SI2C_Init_Type[x].SCL_M, SI2C_Init_Type[x].SCL_Pin, Bit_RESET);
}
void SI2C_Start(uint32_t x)
{
	SDA_H(x);
	SCL_H(x);
	SI2C_Delay(5);
	SDA_L(x);
	Delay_us(5);
	SCL_L(x);
}
void SI2C_Stop(uint32_t x)
{
	SDA_L(x);
	SCL_H(x);
	SI2C_Delay(5);
	SDA_H(x);
	SI2C_Delay(5);
}
// 发送一个字节数据
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
        SI2C_Delay(2);
        SCL_H(x);
        SI2C_Delay(5);
        SCL_L(x);
        SI2C_Delay(2);
    }
}

// 接收一个字节数据
uint8_t SI2C_ReceiveByte(uint32_t x)
{
    uint8_t i, Byte = 0;

    for (i = 0; i < 8; i++)
    {
        SCL_H(x);
        SI2C_Delay(5);
        Byte <<= 1;
        if (SDA_R(x))
        {
            Byte |= 0x01;
        }
        SCL_L(x);
        SI2C_Delay(2);
    }

    return Byte;
}
// 发送应答信号
void SI2C_SendACK(uint32_t x)
{
    SDA_L(x);
    SI2C_Delay(2);
    SCL_H(x);
    SI2C_Delay(5);
    SCL_L(x);
    SI2C_Delay(2);
    SDA_H(x);
}

// 发送非应答信号
void SI2C_SendNACK(uint32_t x)
{
    SDA_H(x);
    SI2C_Delay(2);
    SCL_H(x);
    SI2C_Delay(5);
    SCL_L(x);
    SI2C_Delay(2);
}

// 等待应答信号
uint8_t SI2C_WaitACK(uint32_t x)
{
    uint8_t Ack;
	
    SCL_H(x);
    SI2C_Delay(5);
    Ack = SDA_R(x);
    SCL_L(x);
    SI2C_Delay(2);

    return Ack;
}

// 写数据到从设备
uint8_t SI2C_WriteData(uint32_t x, uint8_t SlaveAddr, uint8_t RegAddr, uint8_t* Data, uint32_t Num)
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

// 从从设备读取数据
uint8_t SI2C_ReadData(uint32_t x, uint8_t SlaveAddr, uint8_t RegAddr, uint8_t *Data, uint32_t Num)
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
