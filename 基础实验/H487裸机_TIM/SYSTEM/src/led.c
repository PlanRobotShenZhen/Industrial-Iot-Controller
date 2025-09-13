#include "led.h"
#include "init.h"

GPIO_Init_t LED_Init_Type[3];

void LED_Configuration(void)
{
	LED_Init_Type[0] = (GPIO_Init_t)
	{
		.GPIO_M = GPIOE,
		.Pin = GPIO_PIN_0,
		.GPIO_Mode = GPIO_MODE_OUTPUT_PP,
		.GPIO_Pull = GPIO_PULL_DOWN,
		.GPIO_Alternate = GPIO_NO_AF,
	};
	LED_Init_Type[1] = (GPIO_Init_t)
	{
		.GPIO_M = GPIOE,
		.Pin = GPIO_PIN_1,
		.GPIO_Mode = GPIO_MODE_OUTPUT_PP,
		.GPIO_Pull = GPIO_PULL_DOWN,
		.GPIO_Alternate = GPIO_NO_AF,
	};
	LED_Init_Type[2] = (GPIO_Init_t)
	{
		.GPIO_M = GPIOE,
		.Pin = GPIO_PIN_4,
		.GPIO_Mode = GPIO_MODE_OUTPUT_PP,
		.GPIO_Pull = GPIO_PULL_DOWN,
		.GPIO_Alternate = GPIO_NO_AF,
	};
	
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO,ENABLE);
	GPIO_ConfigPinRemap(GPIOB_PORT_SOURCE, GPIO_PIN_SOURCE4, GPIO_RMP_SWJ_JTAG_NO_NJTRST);
	
	GPIO_INIT(LED_Init_Type[0]);
	GPIO_INIT(LED_Init_Type[1]);
	GPIO_INIT(LED_Init_Type[2]);
	
//	LED(LED_ERROR, OFF);
//	LED(LED_RUN, OFF);
//	LED(LED_DEBUG, OFF);
//	
//	LED(LED_ERROR, ON);
//	LED(LED_RUN, ON);
//	LED(LED_DEBUG, ON);


}

void LED(e_LED x, e_BitCmd BitCmd)
{
	switch (BitCmd)
	{
		case ON:	GPIO_WriteBits(LED_Init_Type[x].GPIO_M, LED_Init_Type[x].Pin, Bit_RESET);break;
		case OFF:	GPIO_WriteBits(LED_Init_Type[x].GPIO_M, LED_Init_Type[x].Pin, Bit_SET);break;
		case CHANGE:GPIO_TogglePin(LED_Init_Type[x].GPIO_M, LED_Init_Type[x].Pin);break;
	}
}

