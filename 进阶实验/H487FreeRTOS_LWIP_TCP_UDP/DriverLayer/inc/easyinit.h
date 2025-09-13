#ifndef __INIT_H__
#define __INIT_H__

#include <stddef.h>
#include "n32h47x_48x.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_usart.h"
#include "n32h47x_48x_rcc.h"
#include "n32h47x_48x_tim.h"
#include "n32h47x_48x_dma.h"
#include "n32h47x_48x_adc.h"
#include "misc.h"

#define MAKE_UNION_GPIO(value) 	((Module_Union_t){.GPIO_M = value})
#define MAKE_UNION_USART(value) ((Module_Union_t){.USART_M = value})
#define MAKE_UNION_TIM(value) 	((Module_Union_t){.TIM_M = value})
#define MAKE_UNION_ADC(value) 	((Module_Union_t){.ADC_M = value})
#define MAKE_UNION_DMA(value) 	((Module_Union_t){.DMA_M = value})

typedef union
{
	GPIO_Module* 	GPIO_M;
	USART_Module* 	USART_M;
	TIM_Module*		TIM_M;
	ADC_Module*		ADC_M;
	DMA_Module*		DMA_M;
}Module_Union_t;

typedef struct
{
	GPIO_Module*	GPIO_M;
	uint32_t		Pin;
	uint32_t		GPIO_Mode;
	uint32_t		GPIO_Pull;
	uint32_t		GPIO_Alternate;
}GPIO_Init_t;

typedef struct
{
	USART_Module*		USART_M;
	GPIO_Init_t*		TX_Init_Type;
	GPIO_Init_t*		RX_Init_Type;
	uint32_t			Baud;
	DMA_ChannelType* 	DMAChx;
	char*			    RXBuf;
	size_t  			RXBufSize;
	uint32_t 			DMARemap;
}USART_Init_t;

typedef struct
{
	GPIO_Module* 	SCL_M;
	GPIO_Module* 	SDA_M;
	uint32_t		SCL_Pin;
	uint32_t		SDA_Pin;
}SI2C_Init_t;

typedef struct
{
	ADC_Module*			ADC_M;
	DMA_Module*			DMA_M;
	DMA_ChannelType*	DMAx_CHx;
	uint32_t			DMA_Remap;
	uint8_t*			Channel_Group;
	uint32_t			Channel_Num;
	uint16_t*			Result_Addr;
	uint8_t 			SampleTime;
	uint8_t 			Resolution;
}ADC_Init_t;

void RCC_INIT(Module_Union_t ModuleUnionType);
void GPIO_INIT(GPIO_Init_t GPIOInitType);
void GPIO_Write_Delayms(GPIO_Module* GPIOx, uint32_t Pin, Bit_OperateType BitCmd, uint32_t DelayTime);
void GPIO_Write_Delayus(GPIO_Module* GPIOx, uint32_t Pin, Bit_OperateType BitCmd, uint32_t DelayTime);
void USART_INIT(USART_Init_t USART_Init_Type);
IRQn_Type USARTx_IRQn(USART_Module* USART_M);
void SI2C_INIT(SI2C_Init_t SI2C_Init_Type);
void ADC_INIT(ADC_Init_t ADC_Init_Type);
void DWT_INIT(void);
void delay_loop(uint32_t us);

#endif
