#include "led.h"
#include "init.h"

GPIO_Init_t LED_Init_Type[3];

void LED_Configuration(void)
{
    // 配置第一个LED (LED0) - 对应GPIOE引脚0
	LED_Init_Type[0] = (GPIO_Init_t)
	{
        .GPIO_M = GPIOE,               // 使用GPIOE端口
        .Pin = GPIO_PIN_0,             // 使用引脚0
        .GPIO_Mode = GPIO_MODE_OUTPUT_PP, // 推挽输出模式（适合LED控制）
        .GPIO_Pull = GPIO_PULL_DOWN,   // 下拉电阻（未通电时为低电平）
        .GPIO_Alternate = GPIO_NO_AF,  // 不使用复用功能
	};
    // 配置第二个LED (LED1) - 对应GPIOE引脚1
	LED_Init_Type[1] = (GPIO_Init_t)
	{
		.GPIO_M = GPIOE,
		.Pin = GPIO_PIN_1,  // 使用引脚1
		.GPIO_Mode = GPIO_MODE_OUTPUT_PP,
		.GPIO_Pull = GPIO_PULL_DOWN,
		.GPIO_Alternate = GPIO_NO_AF,
	};
	// 使能AFIO时钟（复用功能IO时钟）
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO,ENABLE);
    // 配置引脚重映射：禁用JTAG功能，保留SWD功能（这行与LED无关，是调试接口配置）
	GPIO_ConfigPinRemap(GPIOB_PORT_SOURCE, GPIO_PIN_SOURCE4, GPIO_RMP_SWJ_JTAG_NO_NJTRST);
	
    // 初始化3个LED对应的GPIO引脚
    GPIO_INIT(LED_Init_Type[0]);  // 初始化第一个LED    在init.c 中初始化
    GPIO_INIT(LED_Init_Type[1]);  // 初始化第二个LED

//	LED(LED_ERROR, OFF);
//	LED(LED_RUN, OFF);
  
//	LED(LED_ERROR, ON);
//	LED(LED_RUN, ON);

}

void LED(e_LED x, e_BitCmd BitCmd)
{
	switch (BitCmd)
	{
        // 当命令为ON时：输出低电平（根据电路设计，低电平点亮LED）
        case ON:  GPIO_WriteBits(LED_Init_Type[x].GPIO_M, LED_Init_Type[x].Pin, Bit_RESET); break;
        // 当命令为OFF时：输出高电平（高电平熄灭LED）
        case OFF: GPIO_WriteBits(LED_Init_Type[x].GPIO_M, LED_Init_Type[x].Pin, Bit_SET); break;
        // 当命令为CHANGE时：翻转当前状态
        case CHANGE: GPIO_TogglePin(LED_Init_Type[x].GPIO_M, LED_Init_Type[x].Pin); break;
	}
}

