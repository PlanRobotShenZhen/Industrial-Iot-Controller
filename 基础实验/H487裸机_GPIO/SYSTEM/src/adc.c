#include "adc.h"
#include "init.h"

ADC_Init_t ADC_Init_Type;
uint16_t ADC_Result[2];

void ADC1_Configuration(void)
{
	GPIO_Init_t GPIO_Init_Type;
	GPIO_Init_Type = (GPIO_Init_t)
	{
		.GPIO_M 		= GPIOB,
		.GPIO_Pull 		= GPIO_PIN_1,
		.GPIO_Mode 		= GPIO_MODE_ANALOG,
		.GPIO_Alternate = GPIO_AF0,
	};
	GPIO_INIT(GPIO_Init_Type);
	GPIO_Init_Type = (GPIO_Init_t)
	{
		.GPIO_M 		= GPIOF,
		.GPIO_Pull 		= GPIO_PIN_2,
		.GPIO_Mode 		= GPIO_MODE_ANALOG,
		.GPIO_Alternate = GPIO_AF0,
	};
	GPIO_INIT(GPIO_Init_Type);
	
	ADC_Init_Type.ADC_M = ADC1;
	ADC_Init_Type.DMA_M = DMA1;
	ADC_Init_Type.Channel_Group = (uint8_t[]){ADC1_Channel_12_PB1, ADC1_Channel_13_PF2};
	ADC_Init_Type.Channel_Num = 2;
	ADC_Init_Type.DMAx_CHx = DMA1_CH1;
	ADC_Init_Type.DMA_Remap = DMA_REMAP_ADC1;
	ADC_Init_Type.Resolution = ADC_DATA_RES_12BIT;
	ADC_Init_Type.SampleTime = ADC_SAMP_TIME_CYCLES_13_5;
	ADC_Init_Type.Result_Addr = ADC_Result;
	
	ADC_INIT(ADC_Init_Type);
}

void ADC_Updata(void)
{
	ADC_EnableSoftwareStartConv(ADC_Init_Type.ADC_M, ENABLE);
}
