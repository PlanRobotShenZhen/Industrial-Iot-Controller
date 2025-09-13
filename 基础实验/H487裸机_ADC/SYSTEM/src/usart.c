#include "usart.h"
#include "string.h"
#include <stdarg.h>
USART_InitType USART_InitStructure;

/**
 * @brief  ��������USART1 ��ʼ�� + ��ӡ����
 * @param  ��
 * @return ��
 */
void USART1_init(void)
{  
    /* ����1������ϵͳʱ�� */
    RCC_Configuration();

    /* ����2������ GPIO��PA9/RX��PA10/TX Ϊ USART1 ���ù��ܣ� */
    GPIO_Configuration();
    
    /* ����3������ USART1 ���� */
    USART_StructInit(&USART_InitStructure);  // �ṹ��Ĭ�ϳ�ʼ��
    USART_InitStructure.BaudRate            = 115200;    // �����ʣ�115200���봮�ڹ���һ�£�
    USART_InitStructure.WordLength          = USART_WL_8B;// ����λ��8λ
    USART_InitStructure.StopBits            = USART_STPB_1;// ֹͣλ��1λ
    USART_InitStructure.Parity              = USART_PE_NO; // У��λ����
    USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE; // ��Ӳ������
    USART_InitStructure.OverSampling        = USART_16OVER;// ��������16��������ȶ��ԣ�
    USART_InitStructure.Mode                = USART_MODE_RX | USART_MODE_TX; // ʹ�� RX + TX

    /* ����4����ʼ�� USART1 ���� */
    USART_Init(USARTx, &USART_InitStructure);

    /* ����5��ʹ�� USART1�����룬�����޷��շ��� */
    USART_Enable(USARTx, ENABLE);

}

/**
 * @brief  ����ϵͳʱ�ӣ����� USART1 �� GPIOA��
 * @param  ��
 * @return ��
 */
void RCC_Configuration(void)
{
    /* 1. ���� GPIOA ʱ�ӣ�PA9/PA10 ���� GPIO �飩 */
    RCC_EnableAHB1PeriphClk(USARTx_GPIO_CLK, ENABLE);  // AHB ����ʱ��ʹ��

    /* 2. ���� USART1 ʱ�ӣ�APB2 ���ߣ� */
    USART_APBxClkCmd(USARTx_CLK, ENABLE);

    /* 3. ���� AFIO ʱ�ӣ����ù��ܱ��룬�ٷ����̱����� */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);
}


/**
 * @brief  ���� GPIO��PA9��RX����PA10��TX��Ϊ USART1 ���ù���
 * @param  ��
 * @return ��
 */
void GPIO_Configuration(void)
{
    GPIO_InitType GPIO_InitStructure;

    /* 1. ��ʼ�� GPIO �ṹ��ΪĬ��ֵ */
    GPIO_InitStruct(&GPIO_InitStructure);
    
    /************************** ���� PA9 Ϊ USART1 RX�������������룩 **************************/
    GPIO_InitStructure.Pin            = USARTx_RXPin;          // ���ţ�PA9
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;       // �������죨USART �������ű�׼ģʽ��
    GPIO_InitStructure.GPIO_Alternate = USARTx_RX_GPIO_AF;     // ���úţ�AF5��USART1��
    GPIO_InitStructure.GPIO_Pull      = GPIO_PULL_UP;          // �������������ʱ��ƽ���ȶ���
    GPIO_InitPeripheral(USARTx_GPIO, &GPIO_InitStructure);     // ��ʼ�� GPIOA

    /************************** ���� PA10 Ϊ USART1 TX��������������� **************************/
    GPIO_InitStructure.Pin            = USARTx_TXPin;          // ���ţ�PA10
    GPIO_InitStructure.GPIO_Alternate = USARTx_TX_GPIO_AF;     // ���úţ�AF5��USART1��
    // ����������Mode��Pull��Speed���� RX һ�£������ظ�����
    GPIO_InitPeripheral(USARTx_GPIO, &GPIO_InitStructure);     // ��ʼ�� GPIOA
}

/**
 * @brief  �ض��� printf �� USART1
 * @param  ch��Ҫ���͵��ַ�
 * @param  f���ļ�ָ�루��׼��Ĭ�ϲ�����
 * @return ���͵��ַ�
 */
int fputc(int ch, FILE* f)
{
    /* �ȴ�������ɣ�TX �Ĵ���Ϊ�գ� */
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TXDE) == RESET);
    
    /* ���� 1 �ֽ����ݵ� USART1 TX ���ţ�PA10�� */
    USART_SendData(USARTx, (uint8_t)ch);

    return (ch);
}

void USART_SendStr(USART_Module* usart, const char* Str, uint16_t Len)
{
     // ��ָ�뱣�������⴫����Ч��ַ��
    if (usart == NULL || Str == NULL || Len == 0)
        return;
    
	for (int i = 0; i < Len; i++)
	{
        // ��ȷ���ȴ����ͼĴ����գ�TXDE��
		while (USART_GetFlagStatus(usart, USART_FLAG_TXDE) == RESET);
//		USART_ClrFlag(usart, USART_FLAG_TXC);
		USART_SendData(usart, Str[i]);
		
	}
    while (USART_GetFlagStatus(usart, USART_FLAG_TXC) == RESET);
    USART_ClrFlag(usart, USART_FLAG_TXC);
}
void USART_Print(USART_Module* usart, const char* Format, ...)
{
    char SendBuf[1024];
    va_list args;

    va_start(args, Format);
    vsnprintf(SendBuf, 1024, Format, args);
    va_end(args);

    uint32_t Len = strlen(SendBuf);
    USART_SendStr(usart, SendBuf, Len);
}


