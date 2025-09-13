#include "log.h"

#if LOG_ENABLE

#include "n32h47x_48x.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_usart.h"
#include "n32h47x_48x_rcc.h"

#define LOG_USARTx      USART1
#define LOG_PERIPH      RCC_APB2_PERIPH_USART1
#define LOG_GPIO        GPIOA
#define LOG_PERIPH_GPIO RCC_AHB_PERIPHEN_GPIOA
#define LOG_TX_PIN      GPIO_PIN_9
#define LOG_RX_PIN      GPIO_PIN_10
#define LOG_TX_AF       (GPIO_AF5)
#define LOG_RX_AF       (GPIO_AF5)

void log_init(void)
{
    GPIO_InitType GPIO_InitStructure;
    USART_InitType USART_InitStructure;

    GPIO_InitStruct(&GPIO_InitStructure);

    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);

    RCC_EnableAPB2PeriphClk(LOG_PERIPH, ENABLE);
    
    RCC_EnableAHB1PeriphClk(LOG_PERIPH_GPIO, ENABLE);

    GPIO_InitStructure.Pin            = LOG_TX_PIN;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = LOG_TX_AF;
    GPIO_InitPeripheral(LOG_GPIO, &GPIO_InitStructure);

    GPIO_InitStructure.Pin             = LOG_RX_PIN;
    GPIO_InitStructure.GPIO_Alternate  = LOG_RX_AF;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.BaudRate            = 115200;
    USART_InitStructure.WordLength          = USART_WL_8B;
    USART_InitStructure.StopBits            = USART_STPB_1;
    USART_InitStructure.Parity              = USART_PE_NO;
    USART_InitStructure.OverSampling        = USART_16OVER;
    USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
    USART_InitStructure.Mode                = USART_MODE_RX | USART_MODE_TX;

    // init usart
    USART_Init(LOG_USARTx, &USART_InitStructure);

    // enable usart
    USART_Enable(LOG_USARTx, ENABLE);
}
static int is_lr_sent = 0;

int fputc(int ch, FILE* f)
{
    if (ch == '\r')
    {
        is_lr_sent = 1;
    }
    else if (ch == '\n')
    {
        if (!is_lr_sent)
        {
            /* Loop until the end of transmission */
            while (USART_GetFlagStatus(LOG_USARTx, USART_FLAG_TXC) == RESET)
            {
            }
            USART_SendData(LOG_USARTx, (uint8_t)'\r');
        }
        is_lr_sent = 0;
    }
    else
    {
        is_lr_sent = 0;
    }
    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(LOG_USARTx, USART_FLAG_TXC) == RESET)
    {
    }
    USART_SendData(LOG_USARTx, (uint8_t)ch);
    return ch;
}


#endif
