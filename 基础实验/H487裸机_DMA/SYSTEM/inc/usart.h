#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

// 1. ���� N32 ��׼��ͷ�ļ����ٷ�����ͬ�
#include "n32h47x_48x.h"
#include "misc.h"
#include <stdio.h>
#include "n32h47x_48x_rcc.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_usart.h"

// 2. �ؼ������� USART1 + PA9��RX����PA10��TX�����ã��滻�ٷ� USART3��
#define _USART1_COM_  // ���� USART1���������

#ifdef _USART1_COM_
#define USARTx                  USART1          // Ŀ�� USART ���裺USART1
#define USARTx_CLK              RCC_APB2_PERIPH_USART1  // USART1 ʱ�ӣ�APB2 ���ߣ�
#define USARTx_GPIO             GPIOA           // �������� GPIO �飺GPIOA
#define USARTx_GPIO_CLK         RCC_AHB_PERIPHEN_GPIOA  // GPIOA ʱ�ӣ�AHB ���ߣ�
#define USARTx_RXPin            GPIO_PIN_9      // ��� RX ���ţ�PA9
#define USARTx_TXPin            GPIO_PIN_10     // ��� TX ���ţ�PA10
#define USARTx_RX_GPIO_AF       GPIO_AF5        // USART1_RX��PA9�����úţ�AF5�����ֲ�ȷ�ϣ�
#define USARTx_TX_GPIO_AF       GPIO_AF5        // USART1_TX��PA10�����úţ�AF5���� RX һ�£�
#define USART_APBxClkCmd        RCC_EnableAPB2PeriphClk  // USART1 ʱ��ʹ�ܺ�����APB2��
#endif

// 3. ����״̬ö�٣��ٷ����̱�����
typedef enum
{
    FAILED = 0,
    PASSED = !FAILED
} TestStatus;

// 4. ����������ʱ�ӡ�GPIO ���ã�
void RCC_Configuration(void);
void GPIO_Configuration(void);
void USART1_init(void);
int fputc(int ch, FILE* f);
void USART_SendStr(USART_Module* usart, const char* Str, uint16_t Len);
void USART_Print(USART_Module* usart, const char* Format, ...);
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */
