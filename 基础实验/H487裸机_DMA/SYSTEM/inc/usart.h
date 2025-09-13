#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

// 1. 包含 N32 标准库头文件（官方例程同款）
#include "n32h47x_48x.h"
#include "misc.h"
#include <stdio.h>
#include "n32h47x_48x_rcc.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_usart.h"

// 2. 关键：定义 USART1 + PA9（RX）、PA10（TX）配置（替换官方 USART3）
#define _USART1_COM_  // 启用 USART1（你的需求）

#ifdef _USART1_COM_
#define USARTx                  USART1          // 目标 USART 外设：USART1
#define USARTx_CLK              RCC_APB2_PERIPH_USART1  // USART1 时钟（APB2 总线）
#define USARTx_GPIO             GPIOA           // 引脚所属 GPIO 组：GPIOA
#define USARTx_GPIO_CLK         RCC_AHB_PERIPHEN_GPIOA  // GPIOA 时钟（AHB 总线）
#define USARTx_RXPin            GPIO_PIN_9      // 你的 RX 引脚：PA9
#define USARTx_TXPin            GPIO_PIN_10     // 你的 TX 引脚：PA10
#define USARTx_RX_GPIO_AF       GPIO_AF5        // USART1_RX（PA9）复用号：AF5（查手册确认）
#define USARTx_TX_GPIO_AF       GPIO_AF5        // USART1_TX（PA10）复用号：AF5（与 RX 一致）
#define USART_APBxClkCmd        RCC_EnableAPB2PeriphClk  // USART1 时钟使能函数（APB2）
#endif

// 3. 测试状态枚举（官方例程保留）
typedef enum
{
    FAILED = 0,
    PASSED = !FAILED
} TestStatus;

// 4. 函数声明（时钟、GPIO 配置）
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
