#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "usart.h"
#include "init.h"

#define USART1_RS485 1
#define UART7_RS485 1
#define USART4_RS485 1
#define UART6_RS485 1

USART_Init_t USART_Init_Type[4];
USART_Use_t USART_Use_Type[4];

char USART1_RXBuf[1024];
char UART7_RXBuf[1024];
char USART4_RXBuf[1024];
char UART6_RXBuf[1024];

// RS485 1
void USART1_Configuration(void)
{
	#define x USART1_ID
	
	// 配置GPIO\串口\中断
	USART_Init_Type[x] = (USART_Init_t)
	{
		.USART_M = USART1,
		.TX_Init_Type = &(GPIO_Init_t)
		{
			.GPIO_M			= GPIOA,
			.Pin			= GPIO_PIN_4,
			.GPIO_Mode		= GPIO_MODE_AF_PP,
			.GPIO_Pull		= GPIO_PULL_UP,
			.GPIO_Alternate = GPIO_AF15,
		},
		.RX_Init_Type = &(GPIO_Init_t)
		{
			.GPIO_M			= GPIOA,
			.Pin			= GPIO_PIN_5,
			.GPIO_Mode		= GPIO_MODE_AF_PP,
			.GPIO_Pull		= GPIO_PULL_UP,
			.GPIO_Alternate = GPIO_AF15,
		},
		.Baud = 115200,
		.DMAChx = DMA1_CH1,
		.RXBuf = USART1_RXBuf,
		.RXBufSize = 1024,
		.DMARemap = DMA_REMAP_USART1_RX,
	};
	
	// 配置缓存区
	USART_Use_Type[x] = (USART_Use_t)
	{
		.USART_M	= USART_Init_Type[x].USART_M,
		.RXBufAddr 	= USART1_RXBuf,
		.BufSize	= 1024,
		.Len	= 0,
	};
	USART_INIT(USART_Init_Type[x]);
	memset(USART_Use_Type[x].RXBufAddr, 0, USART_Use_Type[x].BufSize);
	
	#if USART1_RS485
	// 配置RS485模式切换引脚
	GPIO_Init_t GPIO_Init_Type;
	GPIO_Init_Type = (GPIO_Init_t)
	{
		.GPIO_M			= GPIOA,
		.Pin			= GPIO_PIN_6,
		.GPIO_Mode		= GPIO_MODE_OUTPUT_PP,
		.GPIO_Pull		= GPIO_NO_PULL,
		.GPIO_Alternate = GPIO_AF0,
	};
	GPIO_INIT(GPIO_Init_Type);
	GPIO_Write_Delayms(GPIO_Init_Type.GPIO_M, GPIO_Init_Type.Pin, Bit_RESET, 2);
	#endif
	
	// 发送调试信息
	char Log_Ready[] = "[READY]RS485_1\r\n";
	USART_SendStr(USART_Init_Type[x].USART_M, Log_Ready, strlen((char*)Log_Ready));
	
	#undef x
}
// RS485 2
void UART7_Configuration(void)
{
	#define x UART7_ID
	
	// 配置GPIO\串口\中断
	USART_Init_Type[x] = (USART_Init_t)
	{
		.USART_M = UART7,
		.TX_Init_Type = &(GPIO_Init_t)
		{
			.GPIO_M			= GPIOC,
			.Pin			= GPIO_PIN_4,
			.GPIO_Mode		= GPIO_MODE_AF_PP,
			.GPIO_Pull		= GPIO_PULL_UP,
			.GPIO_Alternate = GPIO_AF13,
		},
		.RX_Init_Type = &(GPIO_Init_t)
		{
			.GPIO_M			= GPIOC,
			.Pin			= GPIO_PIN_5,
			.GPIO_Mode		= GPIO_MODE_AF_PP,
			.GPIO_Pull		= GPIO_PULL_UP,
			.GPIO_Alternate = GPIO_AF13,
		},
		.Baud = 115200,
		.DMAChx = DMA1_CH2,
		.RXBuf = UART7_RXBuf,
		.RXBufSize = 1024,
		.DMARemap = DMA_REMAP_UART7_RX,
	};
	// 配置缓存区
	USART_Use_Type[x] = (USART_Use_t)
	{
		.USART_M	= USART_Init_Type[x].USART_M,
		.RXBufAddr 	= UART7_RXBuf,
		.BufSize	= 1024,
		.Len	= 0,
	};
	USART_INIT(USART_Init_Type[x]);
	
	memset(USART_Use_Type[x].RXBufAddr, 0, USART_Use_Type[x].BufSize);
	
	#if UART7_RS485
	// 配置RS485模式切换引脚
	GPIO_Init_t GPIO_Init_Type;
	GPIO_Init_Type = (GPIO_Init_t)
	{
		.GPIO_M			= GPIOA,
		.Pin			= GPIO_PIN_7,
		.GPIO_Mode		= GPIO_MODE_OUTPUT_PP,
		.GPIO_Pull		= GPIO_NO_PULL,
		.GPIO_Alternate = GPIO_AF0,
	};
	GPIO_INIT(GPIO_Init_Type);
	GPIO_Write_Delayms(GPIO_Init_Type.GPIO_M, GPIO_Init_Type.Pin, Bit_RESET, 2);
	#endif

	// 发送调试信息
	char Log_Ready[] = "[READY]RS485_2\r\n";
	USART_SendStr(USART_Init_Type[x].USART_M, Log_Ready, strlen((char*)Log_Ready));
	#undef x
}
// RS485 3
void USART4_Configuration(void)
{
	#define x USART4_ID
	
	// 配置GPIO\串口\中断
	USART_Init_Type[x] = (USART_Init_t)
	{
		.USART_M = USART4,
		.TX_Init_Type = &(GPIO_Init_t)
		{
			.GPIO_M			= GPIOB,
			.Pin			= GPIO_PIN_0,
			.GPIO_Mode		= GPIO_MODE_AF_PP,
			.GPIO_Pull		= GPIO_PULL_UP,
			.GPIO_Alternate = GPIO_AF13,
		},
		.RX_Init_Type = &(GPIO_Init_t)
		{
			.GPIO_M			= GPIOB,
			.Pin			= GPIO_PIN_1,
			.GPIO_Mode		= GPIO_MODE_AF_PP,
			.GPIO_Pull		= GPIO_PULL_UP,
			.GPIO_Alternate = GPIO_AF13,
		},
		.Baud = 115200,
		.DMAChx = DMA1_CH3,
		.RXBuf = USART4_RXBuf,
		.RXBufSize = 1024,
		.DMARemap = DMA_REMAP_USART4_RX,
	};
	
	// 配置缓存区
	USART_Use_Type[x] = (USART_Use_t)
	{
		.USART_M	= USART_Init_Type[x].USART_M,
		.RXBufAddr 	= USART4_RXBuf,
		.BufSize	= 1024,
		.Len	= 0,
	};
	USART_INIT(USART_Init_Type[x]);
	memset(USART_Use_Type[x].RXBufAddr, 0, USART_Use_Type[x].BufSize);

	#if USART4_RS485
	// 配置RS485模式切换引脚
	GPIO_Init_t GPIO_Init_Type;
	GPIO_Init_Type = (GPIO_Init_t)
	{
		.GPIO_M			= GPIOE,
		.Pin			= GPIO_PIN_9,
		.GPIO_Mode		= GPIO_MODE_OUTPUT_PP,
		.GPIO_Pull		= GPIO_PULL_UP,
		.GPIO_Alternate = GPIO_AF0,
	};
	GPIO_INIT(GPIO_Init_Type);
	#endif
	
	// 发送调试信息
	char Log_Ready[] = "[READY]RS485_3\r\n";
	USART_SendStr(USART_Init_Type[x].USART_M, Log_Ready, strlen((char*)Log_Ready));
	#undef x
}

// RS485 4
void UART6_Configuration(void)
{
	#define x UART6_ID
	
	// 配置GPIO\串口\中断
	USART_Init_Type[x] = (USART_Init_t)
	{
		.USART_M = UART6,
		.TX_Init_Type = &(GPIO_Init_t)
		{
			.GPIO_M			= GPIOB,
			.Pin			= GPIO_PIN_2,
			.GPIO_Mode		= GPIO_MODE_AF_PP,
			.GPIO_Pull		= GPIO_PULL_UP,
			.GPIO_Alternate = GPIO_AF12,
		},
		.RX_Init_Type = &(GPIO_Init_t)
		{
			.GPIO_M			= GPIOE,
			.Pin			= GPIO_PIN_7,
			.GPIO_Mode		= GPIO_MODE_AF_PP,
			.GPIO_Pull		= GPIO_PULL_UP,
			.GPIO_Alternate = GPIO_AF8,
		},
		.Baud = 115200,
		.DMAChx = DMA1_CH4,
		.RXBuf = UART6_RXBuf,
		.RXBufSize = 1024,
		.DMARemap = DMA_REMAP_UART6_RX,
	};
	// 配置缓存区
	USART_Use_Type[x] = (USART_Use_t)
	{
		.USART_M	= USART_Init_Type[x].USART_M,
		.RXBufAddr 	= UART6_RXBuf,
		.BufSize	= 1024,
		.Len	= 0,
	};
	USART_INIT(USART_Init_Type[x]);
	memset(USART_Use_Type[x].RXBufAddr, 0, USART_Use_Type[x].BufSize);
	
	#if UART6_RS485
	// 配置RS485模式切换引脚
	GPIO_Init_t GPIO_Init_Type;
	GPIO_Init_Type = (GPIO_Init_t)
	{
		.GPIO_M			= GPIOE,
		.Pin			= GPIO_PIN_12,
		.GPIO_Mode		= GPIO_MODE_OUTPUT_PP,
		.GPIO_Pull		= GPIO_NO_PULL,
		.GPIO_Alternate = GPIO_AF0,
	};
	GPIO_INIT(GPIO_Init_Type);
	GPIO_Write_Delayms(GPIO_Init_Type.GPIO_M, GPIO_Init_Type.Pin, Bit_RESET, 2);
	#endif
	
	char Log_Ready[] = "[READY]RS485_4\r\n";
	USART_SendStr(USART_Init_Type[x].USART_M, Log_Ready, strlen((char*)Log_Ready));
	#undef x
}

void USART_SendStr(USART_Module* USARTx, const char* Str, size_t Len)
{
	RS485_Mode(USARTx, SendMode);
	for (int i = 0; i < Len; i++)
	{
		USART_ClrFlag(USARTx, USART_FLAG_TXC);
		USART_SendData(USARTx, Str[i]);
		while (USART_GetFlagStatus(USARTx, USART_FLAG_TXC) == RESET);
	}
	RS485_Mode(USARTx, RecvMode);
}


static void RS485_Mode(USART_Module* USARTx, e_RS485_Mode Mode)
{
	if (USARTx == USART1)
	{
		GPIO_Write_Delayms(GPIOA, GPIO_PIN_6, (Mode == SendMode)?Bit_SET:Bit_RESET, 1);
	}
	else if (USARTx == UART7)
	{
		GPIO_Write_Delayms(GPIOA, GPIO_PIN_7, (Mode == SendMode)?Bit_SET:Bit_RESET, 1);
	}
	else if (USARTx == USART4)
	{
		GPIO_Write_Delayms(GPIOE, GPIO_PIN_9, (Mode == SendMode)?Bit_SET:Bit_RESET, 1);
	}
	else if (USARTx == UART6)
	{
		GPIO_Write_Delayms(GPIOE, GPIO_PIN_12, (Mode == SendMode)?Bit_SET:Bit_RESET, 1);
	}
}


void USART_Print(USART_Module* USARTx, const char* Format, ...)
{
    char SendBuf[1024];
    va_list args;

    va_start(args, Format);
    vsnprintf(SendBuf, 1024, Format, args);
    va_end(args);

    uint32_t Len = strlen(SendBuf);
    USART_SendStr(USARTx, SendBuf, Len);
}

void USART_IRQHandler(uint32_t x)
{	
	#define USARTx         	USART_Use_Type[x].USART_M
	#define USARTx_RXBuf   	USART_Use_Type[x].RXBufAddr
	#define USARTx_BufSize	USART_Use_Type[x].BufSize
	#define USARTx_Len		USART_Use_Type[x].Len
	#define USARTx_DMAChx	USART_Init_Type[x].DMAChx
	
	if ((USART_GetFlagStatus(USARTx, USART_FLAG_IDLEF) != RESET) && (USART_GetIntStatus(USARTx, USART_INT_IDLEF) != RESET))
    {
		USARTx->STS;
		USARTx->DAT;

		USARTx_Len = USARTx_BufSize - DMA_GetCurrDataCounter(USARTx_DMAChx);
        USARTx_RXBuf[USARTx_Len] = '\0';
		
        USART_SendStr(USARTx, USARTx_RXBuf, USARTx_Len);
        
		DMA_EnableChannel(USARTx_DMAChx, DISABLE);
		DMA_SetCurrDataCounter(USARTx_DMAChx, USARTx_BufSize);
		DMA_EnableChannel(USARTx_DMAChx, ENABLE);	
    }
	
	#undef USARTx
	#undef USARTx_RXBuf
	#undef USARTx_BufSize
	#undef USARTx_Len
	#undef USARTx_DMAChx
}
void USART1_IRQHandler(void)
{
	USART_IRQHandler(USART1_ID);
}

void UART7_IRQHandler(void)
{
	USART_IRQHandler(UART7_ID);
}

void USART4_IRQHandler(void)
{
	USART_IRQHandler(USART4_ID);
}

void UART6_IRQHandler(void)
{
	USART_IRQHandler(UART6_ID);
}

