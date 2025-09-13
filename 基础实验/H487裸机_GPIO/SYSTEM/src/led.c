#include "led.h"
#include "init.h"

GPIO_Init_t LED_Init_Type[3];

void LED_Configuration(void)
{
    // ���õ�һ��LED (LED0) - ��ӦGPIOE����0
	LED_Init_Type[0] = (GPIO_Init_t)
	{
        .GPIO_M = GPIOE,               // ʹ��GPIOE�˿�
        .Pin = GPIO_PIN_0,             // ʹ������0
        .GPIO_Mode = GPIO_MODE_OUTPUT_PP, // �������ģʽ���ʺ�LED���ƣ�
        .GPIO_Pull = GPIO_PULL_DOWN,   // �������裨δͨ��ʱΪ�͵�ƽ��
        .GPIO_Alternate = GPIO_NO_AF,  // ��ʹ�ø��ù���
	};
    // ���õڶ���LED (LED1) - ��ӦGPIOE����1
	LED_Init_Type[1] = (GPIO_Init_t)
	{
		.GPIO_M = GPIOE,
		.Pin = GPIO_PIN_1,  // ʹ������1
		.GPIO_Mode = GPIO_MODE_OUTPUT_PP,
		.GPIO_Pull = GPIO_PULL_DOWN,
		.GPIO_Alternate = GPIO_NO_AF,
	};
	// ʹ��AFIOʱ�ӣ����ù���IOʱ�ӣ�
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO,ENABLE);
    // ����������ӳ�䣺����JTAG���ܣ�����SWD���ܣ�������LED�޹أ��ǵ��Խӿ����ã�
	GPIO_ConfigPinRemap(GPIOB_PORT_SOURCE, GPIO_PIN_SOURCE4, GPIO_RMP_SWJ_JTAG_NO_NJTRST);
	
    // ��ʼ��3��LED��Ӧ��GPIO����
    GPIO_INIT(LED_Init_Type[0]);  // ��ʼ����һ��LED    ��init.c �г�ʼ��
    GPIO_INIT(LED_Init_Type[1]);  // ��ʼ���ڶ���LED

//	LED(LED_ERROR, OFF);
//	LED(LED_RUN, OFF);
  
//	LED(LED_ERROR, ON);
//	LED(LED_RUN, ON);

}

void LED(e_LED x, e_BitCmd BitCmd)
{
	switch (BitCmd)
	{
        // ������ΪONʱ������͵�ƽ�����ݵ�·��ƣ��͵�ƽ����LED��
        case ON:  GPIO_WriteBits(LED_Init_Type[x].GPIO_M, LED_Init_Type[x].Pin, Bit_RESET); break;
        // ������ΪOFFʱ������ߵ�ƽ���ߵ�ƽϨ��LED��
        case OFF: GPIO_WriteBits(LED_Init_Type[x].GPIO_M, LED_Init_Type[x].Pin, Bit_SET); break;
        // ������ΪCHANGEʱ����ת��ǰ״̬
        case CHANGE: GPIO_TogglePin(LED_Init_Type[x].GPIO_M, LED_Init_Type[x].Pin); break;
	}
}

