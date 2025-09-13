#include "i2c.h"
#include "init.h"
SI2C_Init_t SI2C_Init_Type[1];
/**
 * @brief  ΢�뼶��ʱ�����������ʱ������CPU��Ƶ��
 * @param  us����Ҫ��ʱ��΢����������5������ʱ5��s��
 * @return ��
 * @note   ԭ������CPU��Ƶ��168MHz��������Ҫִ�еĿ�ָ�__NOP()������
 *         168MHz = 168,000,000��ָ��/�� �� 1��=1,000,000��s �� 1��s��168��ָ��
 *         �������5����Ϊ��whileѭ��+__NOP()Լռ5��ָ�ȷ����ʱ���Ƚӽ�1��s
 */
void delay_loop_us(uint32_t us) {
    uint32_t cycles = us * (168000000UL / 1000000UL / 5);  // 5��ָ������Լ1us
    while (cycles--) {
        __NOP();  // ��ָ�����1������
    }
}
/**
 * @brief  I2C1���߳�ʼ����ָ��SCL/SDA���ţ�����Ӳ��������
 * @param  ��
 * @return ��
 * @note   �������õ��ǵ�0·I2C��x=0����ʵ�ʶ�ӦӲ����I2C1
 */
void SI2C1_Configuration(void)
{
	#define x 0
	// ��I2C��ʼ���ṹ�帳ֵ��ָ��SCL��SDA��GPIO�˿ڡ�����
	SI2C_Init_Type[x] = (SI2C_Init_t)
	{
		.SCL_M = GPIOA,       // SCL�ߵ�GPIO�˿ڣ�GPIOA
		.SDA_M = GPIOA,       // SDA�ߵ�GPIO�˿ڣ�GPIOA
		.SCL_Pin =GPIO_PIN_12,// SCL�ߵ����ţ�PA12
		.SDA_Pin =GPIO_PIN_11,// SDA�ߵ����ţ�PA11
	};
    // ����I2C��ʼ�����ĺ�������init.h�ж��壩��
	// ʵ�ʻ�����GPIOΪ��©�����I2CЭ��Ҫ�󣩡�ʹ��GPIOʱ�ӵȵײ����
	SI2C_INIT(SI2C_Init_Type[x]);
	
	#undef x
}
/**
 * @brief  ��SDA��������Ϊ����ģʽ�����ڽ������ݣ�
 * @param  x��I2C��ţ�����0�����0·I2C��
 * @return ��
 */
void SDA_InputMode(uint8_t x)
{ 
    GPIO_InitType GPIO_InitStruct;// GPIO��ʼ�������ṹ�壨��init.h�ж��壩
    // ��������ģʽ����
	GPIO_InitStruct.GPIO_Mode = GPIO_MODE_INPUT;  // ģʽ������ģʽ
	GPIO_InitStruct.GPIO_Pull = GPIO_PULL_DOWN;   // �������裺��ֹ��������ʱ��ƽ���ȶ�
	GPIO_InitStruct.GPIO_Current =GPIO_DC_12mA;   // ��������������12mA���ɸ���Ӳ��������
	GPIO_InitStruct.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;//  slew�ʣ����٣��������ݴ����ٶȣ�
	GPIO_InitStruct.Pin = SI2C_Init_Type[x].SDA_Pin ;// ���ţ�ʹ�õ�x·I2C���õ�SDA���ţ�PA11��
    // ���õײ㺯����������Ӧ�õ�SDA���ţ�GPIOA�˿ڣ�   
    GPIO_InitPeripheral(SI2C_Init_Type[x].SDA_M ,&GPIO_InitStruct);
}
/**
 * @brief  ��SDA��������Ϊ���ģʽ�����ڷ������ݣ�
 * @param  x��I2C��ţ�����0�����0·I2C��
 * @return ��
 * @note   I2CЭ��Ҫ��SDAΪ��©����������������������GPIO_MODE_OUTPUT_PP��Ҳ�ܹ���
 *         ����©������ⲿ�������裬��������ڲ��Ѵ�����Ӳ����
 */
void SDA_OutputMode(uint8_t x)
{
    GPIO_InitType GPIO_InitStruct;// GPIO��ʼ�������ṹ��
    // �������ģʽ����
	GPIO_InitStruct.GPIO_Mode = GPIO_MODE_OUTPUT_PP; // ģʽ������������ʺ�˫��ͨ�ţ�
	GPIO_InitStruct.GPIO_Pull = GPIO_PULL_DOWN; // �������裺��ֹ����ʱ��ƽƯ��
	GPIO_InitStruct.GPIO_Current =GPIO_DC_12mA;// ��������������12mA
	GPIO_InitStruct.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;// ����slew��
	GPIO_InitStruct.Pin = SI2C_Init_Type[x].SDA_Pin ;// ���ţ���x·I2C��SDA���ţ�PA11��
    // ������Ӧ�õ�SDA����
    GPIO_InitPeripheral(SI2C_Init_Type[x].SDA_M ,&GPIO_InitStruct);
}
/**
 * @brief  I2C��ʱ��������װ�ײ���ʱ��ͳһ��ʱ�ӿڣ�
 * @param  us����ʱ΢����
 * @return ��
 * @note   ��������I2Cʱ�����ʱ���������������������ͳһ����
 */
void SI2C_delay(uint32_t us)
{
	delay_loop_us(us);
}
/**
 * @brief  ��SDA����Ϊ�ߵ�ƽ����ʾ����1����У�
 * @param  x��I2C���
 * @return ��
 */
void SDA_H(uint8_t x)
{
    // GPIO_WriteBits���ײ㺯��������ָ�����ŵĵ�ƽ��Bit_SET=�ߵ�ƽ��
	GPIO_WriteBits(SI2C_Init_Type[x].SDA_M, SI2C_Init_Type[x].SDA_Pin, Bit_SET);
}
/**
 * @brief  ��SCL����Ϊ�ߵ�ƽ��ʱ�Ӹߵ�ƽ�������������ݣ�
 * @param  x��I2C���
 * @return ��
 * @note   SCL�ߵ�ƽʱ��SDA�ĵ�ƽ���ܱ䣨����ᱻʶ��Ϊ��ʼ/ֹͣ�źţ�
 */
void SCL_H(uint8_t x)
{
	GPIO_WriteBits(SI2C_Init_Type[x].SCL_M, SI2C_Init_Type[x].SCL_Pin, Bit_SET);
}
/**
 * @brief  ��SDA����Ϊ�͵�ƽ����ʾ����0��ʼ/ֹͣ�źţ�
 * @param  x��I2C���
 * @return ��
 */
void SDA_L(uint8_t x)
{
	GPIO_WriteBits(SI2C_Init_Type[x].SDA_M, SI2C_Init_Type[x].SDA_Pin, Bit_RESET);
}
/**
 * @brief  ��ȡSDA�ߵĵ�ǰ��ƽ�����ڽ������ݻ�Ӧ���źţ�
 * @param  x��I2C���
 * @return 0��SDA�͵�ƽ��1��SDA�ߵ�ƽ
 * @note   ��ȡǰ�����Ƚ�SDA��Ϊ����ģʽ��������л����ģʽ����֤�����ܷ������ݣ�
 */
uint8_t SDA_R(uint8_t x)
{
    SDA_InputMode(x);// 1. �л�SDAΪ����ģʽ�����ܶ�ȡ�ⲿ��ƽ��
    // 2. ��ȡSDA���ŵ�ƽ��GPIO_ReadInputDataBit���ײ㺯������ȡָ����������ֵ��
    uint8_t ret = GPIO_ReadInputDataBit(SI2C_Init_Type[x].SDA_M, SI2C_Init_Type[x].SDA_Pin);
    SDA_OutputMode(x);// 3. �л����ģʽ�����������������ã�
	return ret;// ���ض�ȡ���ĵ�ƽֵ
}
/**
 * @brief  ��SCL����Ϊ�͵�ƽ��ʱ�ӵ͵�ƽ������׼����һλ���ݣ�
 * @param  x��I2C���
 * @return ��
 * @note   SCL�͵�ƽʱ��SDA�ĵ�ƽ���Ա仯��׼����һ������λ��
 */
void SCL_L(uint8_t x)
{
	GPIO_WriteBits(SI2C_Init_Type[x].SCL_M, SI2C_Init_Type[x].SCL_Pin, Bit_RESET);
}
/**
 * @brief  ����I2C��ʼ�źţ�����ͨ�ţ�
 * @param  x��I2C���
 * @return ��
 * @note   I2CЭ��涨�Ŀ�ʼ�źţ�SCL�ߵ�ƽʱ��SDA�Ӹߵ�ƽ�����͵�ƽ
 *         ʱ���裺SDA�ߡ�SCL�ߣ���ʱ����SDA�ͣ���ʱ����SCL�ͣ�׼���������ݣ�
 */
void SI2C_Start(uint8_t x)
{
    SDA_H(x);  // 1. SDA���øߣ�����״̬��
	SCL_H(x);  // 2. SCL�øߣ�ʱ�Ӹߵ�ƽ����ʱSDA�仯�ᱻʶ��Ϊ��ʼ�źţ�
	SI2C_delay(5);  // ��ʱ5��s��ȷ����ƽ�ȶ�
	SDA_L(x);  // 3. SDA�Ӹ����ͣ��ؼ������ɿ�ʼ�źţ�
	SI2C_delay(5);  // ��ʱ5��s��ȷ����������ʶ��
	SCL_L(x);  // 4. SCL�õͣ��������ݴ���׶Σ�SDA�ɱ仯��
}
/**
 * @brief  ����I2Cֹͣ�źţ�����ͨ�ţ�
 * @param  x��I2C���
 * @return ��
 * @note   I2CЭ��涨��ֹͣ�źţ�SCL�ߵ�ƽʱ��SDA�ӵ͵�ƽ�����ߵ�ƽ
 *         ʱ���裺SDA�͡�SCL�ߣ���ʱ����SDA�ߣ���ʱ�������ֹͣ
 */
void SI2C_Stop(uint8_t x)
{
	SDA_L(x);  // 1. SDA���õͣ����ݴ���״̬��
	SCL_H(x);  // 2. SCL�øߣ�ʱ�Ӹߵ�ƽ����ʱSDA�仯�ᱻʶ��Ϊֹͣ�źţ�
	SI2C_delay(5);  // ��ʱ5��s��ȷ����ƽ�ȶ�
	SDA_H(x);  // 3. SDA�ӵ����ߣ��ؼ�������ֹͣ�źţ�
	SI2C_delay(5);  // ��ʱ5��s��ȷ����������ʶ��
}
/**
 * @brief  ��I2C���豸����1���ֽڣ�8λ���ݣ�
 * @param  x��I2C���
 * @param  Byte��Ҫ���͵�8λ���ݣ����紫������ַ�����
 * @return ��
 * @note   ����˳�򣺴����λ��bit7�������λ��bit0����ÿbit����ҪSCLʱ������
 *         ʱ���裺׼��bit��SCL�ߣ����棩��SCL�ͣ�׼����һ��bit��
 */
void SI2C_SendByte(uint32_t x, uint8_t Byte)
{
    uint8_t i;
    // ѭ��8�Σ�����8��bit����bit7��bit0��
    for (i = 0; i < 8; i++)
	{
         // �жϵ�ǰҪ���͵�bit�Ƿ�Ϊ1��Byte & 0x80��ȡ���λ��
        if (Byte & 0x80)
		{
            SDA_H(x); // ��Ϊ1��SDA�ø�
        }
		else
		{
            SDA_L(x); // ��Ϊ0��SDA�õ�
        }
        Byte <<= 1;// ��������1λ��׼����һ��bit������0x12��0x24����һλ��bit6��
        SI2C_delay(2);// ��ʱ2��s��ȷ��SDA��ƽ�ȶ�
        SCL_H(x); // SCL�øߣ��������ݣ��������ڴ�ʱ��ȡSDA�ĵ�ƽ��
        SI2C_delay(5); // ��ʱ5��s��ȷ����������ȡ���
        SCL_L(x); //  SCL�õͣ�׼����һ��bit��SDA��ƽ��
        SI2C_delay(2);// ��ʱ2��s��ȷ��SDA��ƽ�ȶ�
    }
}

/**
 * @brief  ��I2C���豸����1���ֽڣ�8λ���ݣ�
 * @param  x��I2C���
 * @return ���յ���8λ����
 * @note   ����˳�򣺴����λ��bit7�������λ��bit0����ÿbit����ҪSCLʱ�Ӳ���
 *         ����ǰ���뽫SDA��Ϊ����ģʽ�����ܶ�ȡ���������͵ĵ�ƽ��
 */
uint8_t SI2C_ReceiveByte(uint32_t x)
{
    uint8_t i, Byte = 0; // Byte��ʼ��Ϊ0�����ڴ洢���յ�����
    
    SDA_InputMode(x);    // 1. �л�SDAΪ����ģʽ��׼���������ݣ�
    // ѭ��8�Σ�����8��bit����bit7��bit0��
    for (i = 0; i < 8; i++)
    {
        SCL_H(x);// 2. SCL�øߣ��������ڴ�ʱ�����ǰbit�ĵ�ƽ����Ƭ��������
        SI2C_delay(5);// ��ʱ5��s��ȷ������������ĵ�ƽ�ȶ�
        Byte <<= 1;// 3. ��������1λ���ڳ����λ��׼���洢��ǰbit��
        // 4. ��ȡSDA��ƽ����Ϊ1����Byte�����λ��1����Ϊ0������0
        if (SDA_R(x))
        {
            Byte |= 0x01;// ���λ��1
        }
        SCL_L(x);// 5. SCL�õͣ�������׼����һ��bit�ĵ�ƽ��
        SI2C_delay(2);// ��ʱ2��s��ȷ��SCL��ƽ�ȶ�
    }

    return Byte;  // 6. ���ؽ��յ���8λ����
}
/**
 * @brief  ����Ӧ���źţ�ACK�������ߴ����������յ������ˣ���������
 * @param  x��I2C���
 * @return ��
 * @note   Ӧ���źţ�SCL�ߵ�ƽʱ��SDAΪ�͵�ƽ
 */
void SI2C_SendACK(uint32_t x)
{
    SDA_L(x);  // 1. SDA�õͣ���ʾӦ��
    SI2C_delay(2);  // ��ʱ2��s��ȷ����ƽ�ȶ�
    SCL_H(x);  // 2. SCL�øߣ�����������SDA��ƽ��ʶ��ΪӦ��
    SI2C_delay(5);  // ��ʱ5��s��ȷ��������ʶ�����
    SCL_L(x);  // 3. SCL�õͣ�����Ӧ��
    SI2C_delay(2);  // ��ʱ2��s��ȷ����ƽ�ȶ�
    SDA_H(x);  // 4. SDA�øߣ��ָ�����״̬��׼������ͨ�ţ�
}

/**
 * @brief  ���ͷ�Ӧ���źţ�NACK�������ߴ��������������ˣ����ˡ�
 * @param  x��I2C���
 * @return ��
 * @note   ��Ӧ���źţ�SCL�ߵ�ƽʱ��SDAΪ�ߵ�ƽ
 *         ͨ���ڽ������1�ֽ�ʱ���ͣ����⴫������������Ч����
 */
void SI2C_SendNACK(uint32_t x)
{
    SDA_H(x);  // 1. SDA�øߣ���ʾ��Ӧ��
    SI2C_delay(2);  // ��ʱ2��s��ȷ����ƽ�ȶ�
    SCL_H(x);  // 2. SCL�øߣ�����������SDA��ƽ��ʶ��Ϊ��Ӧ��
    SI2C_delay(5);  // ��ʱ5��s��ȷ��������ʶ�����
    SCL_L(x);  // 3. SCL�õͣ�������Ӧ��
    SI2C_delay(2);  // ��ʱ2��s��ȷ����ƽ�ȶ�
}

/**
 * @brief  �ȴ����豸��Ӧ���źţ�ACK����ȷ�ϴ������Ƿ��յ�����
 * @param  x��I2C���
 * @return 0���յ�Ӧ��ACK���������ɹ����գ���1��δ�յ�Ӧ��NACK������ʧ�ܣ�
 * @note   ����1�ֽں������ô˺�����ȷ��ͨ���Ƿ�����
 */
uint8_t SI2C_WaitACK(uint32_t x)
{
    uint8_t Ack;  // �洢Ӧ������0=ACK��1=NACK��
	
    SCL_H(x);  // 1. SCL�øߣ��������ڴ�ʱ���Ӧ���źţ�
    SI2C_delay(5);  // ��ʱ5��s��ȷ�������������Ӧ���ź��ȶ�
    Ack = SDA_R(x);  // 2. ��ȡSDA��ƽ��0=ACK��1=NACK��
    SCL_L(x);  // 3. SCL�õͣ�����Ӧ��ȴ���
    SI2C_delay(2);  // ��ʱ2��s��ȷ����ƽ�ȶ�

    return Ack;
}

/**
 * @brief  ��I2C���豸��ָ���Ĵ���д�����ֽ�����
 * @param  x��I2C���
 * @param  SlaveAddr�����豸��ַ������SHT40�ĵ�ַ��0x44��
 * @param  RegAddr��Ҫд��ļĴ�����ַ�����紫���������üĴ�����
 * @param  Data��Ҫд����������飨�洢����ֽڣ�
 * @param  Num��Ҫд��������ֽ���
 * @return 0��д��ɹ���1��д��ʧ�ܣ�δ�յ�Ӧ��
 * @note   д�������̣���ʼ�����ӻ���ַ����Ӧ������Ĵ�����ַ����Ӧ��������ݡ���Ӧ���ֹͣ
 */
uint8_t SI2C_WriteData(uint8_t x, uint8_t SlaveAddr, uint8_t RegAddr, uint8_t* Data, uint32_t Num)
{
    SI2C_Start(x);  // 1. ���Ϳ�ʼ�źţ�����ͨ�ţ�
    // 2. ���ʹӻ���ַ�� SlaveAddr << 1������1λ�����λ0��ʾ��д��������
    SI2C_SendByte(x, SlaveAddr << 1);
    if (SI2C_WaitACK(x))  // 3. �ȴ�Ӧ��������1��NACK������ʾ������û��Ӧ
    {
        SI2C_Stop(x);  // ����ֹͣ�źţ�����ͨ��
        return 1;      // ����1�����ʧ��
    }
    // 4. ����Ҫд��ļĴ�����ַ�����ߴ���������Ҫд����Ĵ�������
    SI2C_SendByte(x, RegAddr);
    if (SI2C_WaitACK(x))  // 5. �ȴ�Ӧ����ʧ�ܣ�����ͨ��
    {
        SI2C_Stop(x);
        return 1;
    }
    // 6. ѭ��д��Num���ֽ����ݣ�ÿ���ֽڶ�Ҫ��Ӧ��
	for (uint32_t i = 0; i < Num; i++)
	{
		SI2C_SendByte(x, Data[i]);  // ���͵�i���ֽ�
		if (SI2C_WaitACK(x))        // �ȴ�Ӧ����ʧ�ܣ�����ͨ��
		{
			SI2C_Stop(x);
			return 1;
		}
	}
    SI2C_Stop(x);  // 7. ����ֹͣ�źţ�����ͨ��
    return 0;      // ����0����ǳɹ�
}

/**
 * @brief  ��I2C���豸��ָ���Ĵ�����ȡ����ֽ�����
 * @param  x��I2C���
 * @param  SlaveAddr�����豸��ַ������SHT40�ĵ�ַ��0x44��
 * @param  RegAddr��Ҫ��ȡ�ļĴ�����ַ�����紫��������ʪ�����ݼĴ�����
 * @param  Data���洢��ȡ���ݵ����飨��������ݴ������
 * @param  Num��Ҫ��ȡ�������ֽ���
 * @return 0����ȡ�ɹ���1����ȡʧ�ܣ�δ�յ�Ӧ��
 * @note   ���������̣�I2C�����ֽڱ�׼���̣���
 *         ��ʼ�����ӻ���ַ��д������Ӧ������Ĵ�����ַ����Ӧ�����ʱ��������׼�����ݣ���
 *         ���¿�ʼ�����ӻ���ַ����������Ӧ��������ݣ����1�ֽڷ�NACK����ֹͣ
 */
uint8_t SI2C_ReadData(uint8_t x, uint8_t SlaveAddr, uint8_t RegAddr, uint8_t *Data, uint32_t Num)
{
    // ��һ�׶Σ����ߴ���������Ҫ���ĸ��Ĵ�������д�Ĵ�����ַ��
    SI2C_Start(x);  // 1. ���Ϳ�ʼ�ź�
    
    // 2. ���ʹӻ���ַ��д���������λ0��
    SI2C_SendByte(x, SlaveAddr << 1);
    if (SI2C_WaitACK(x))  // 3. �ȴ�Ӧ��ʧ�������
    {
        SI2C_Stop(x);
        return 1;
    }
    
    // 4. ����Ҫ��ȡ�ļĴ�����ַ�����ߴ���������Ҫ������Ĵ�������
    SI2C_SendByte(x, RegAddr);
    if (SI2C_WaitACK(x))  // 5. �ȴ�Ӧ��ʧ�������
    {
        SI2C_Stop(x);
        return 1;
    }
	
    // �ؼ���ʱ����������ʱ��׼�����ݣ�����SHT40��Ҫʱ�������ʪ�ȣ�Լ10ms��
    SI2C_delay(10000);  // 10000��s = 10ms
    
    // �ڶ��׶Σ��Ӵ�������ȡ���ݣ���������
    SI2C_Start(x);  // 6. �������¿�ʼ�źţ��л�Ϊ��������
    
    // 7. ���ʹӻ���ַ�������������λ1��
    SI2C_SendByte(x, (SlaveAddr << 1) | 0x01);
    if (SI2C_WaitACK(x))  // 8. �ȴ�Ӧ��ʧ�������
    {
        SI2C_Stop(x);
        return 1;
    }
    // 9. ѭ����ȡNum���ֽڣ�ǰNum-1���ֽڷ�ACK�����1����NACK��
	for (uint32_t i = 0; i < Num - 1; i++)
	{
		Data[i] = SI2C_ReceiveByte(x);  // ��ȡ��i���ֽ�
		SI2C_SendACK(x);                // ����ACK�����ߴ���������������
	}
	
	// 10. ��ȡ���1���ֽڣ���NACK�����ߴ����������ˡ���
	Data[Num - 1] = SI2C_ReceiveByte(x);
    SI2C_SendNACK(x);
	
    SI2C_Stop(x);  // 11. ����ֹͣ�źţ�����ͨ��
    return 0;      // ����0����ǳɹ�
}
