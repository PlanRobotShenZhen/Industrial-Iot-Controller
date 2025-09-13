#include "main.h"
// 1. ������ʱ����ͷ�ļ�
#include "n32h47x_48x_tim.h"
// 2. ����GPIO��ͷ�ļ���GPIO���ú�����
#include "n32h47x_48x_gpio.h"
// 3. ����ʱ�ӿ�ͷ�ļ���ʱ��ʹ�ܺ�����
#include "n32h47x_48x_rcc.h"

// ȫ�ֱ������������ṹ������ƥ�䣩

TIM_TimeBaseInitType TIM_TimeBaseStructure;  // ��ʱ��ʱ���ṹ�壨���Ѷ��壩
OCInitType TIM_OCInitStructure;              // PWM����ȽϽṹ�壨���Ѷ��壩
uint32_t CCR1_Val       = 600;               // GTIM10_CH1 ��ʼ�Ƚ�ֵ
uint32_t PrescalerValue = 0;                 // Ԥ��Ƶϵ��

// ��ʱ������
#define TIMx           GTIM10
#define TIMx_CLK       RCC_APB2_PERIPH_GTIM10  // ���� GTIM10 ʱ��ʹ���� APB2�������оƬ�ֲ�ȷ�ϣ����������޸�
// ���Ŷ���
#define TIMx_CH1_GPIO_PORT    GPIOE
#define TIMx_CH1_GPIO_PIN     GPIO_PIN_1
#define TIMx_CH1_GPIO_CLK     RCC_AHB_PERIPHEN_GPIOE //RCC_AHB_PERIPH_GPIOE
#define TIMx_CH1_GPIO_AF      GPIO_AF_GTIM10_CH1_PE1  // ��Ӧ GPIO_AF_GTIM10_CH1_PE1 �ĸ��ù���

/**
 * @brief  ����PWM��������ʱ�ӣ�GTIM10��GPIOE��AFIO��
 * @param  ��
 * @return ��
 * @note   �ؼ�����ͬ�����ʱ�����߲�ͬ��APB1/APB2/AHB1��������ѡ�ԣ��������費����
 */
void RCC_Configuration(void)
{
    // 1. ʹ��GTIM10��ʱ�ӣ�������APB2���ߣ���ʵ����APB1�����ΪRCC_APB1_PERIPH_GTIM10��
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GTIM10, ENABLE); 
    // 2. ʹ��GPIOE��ʱ�ӣ�PWM������GPIOE�����뿪����
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOE, ENABLE); 
    // 3. ʹ��AFIO��ʱ�ӣ����Ÿ��ù�������AFIO�����뿪��������GPIO�޷�����Ϊ��ʱ�������
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE); 
}
/**
 * @brief  ����GPIOE_PIN_1ΪGTIM10_CH1��PWM������ţ���������ģʽ��
 * @param  ��
 * @return ��
 * @note   ���ģ�����ģʽ��PWM����Ĺؼ�����ͨ���ģʽ�޷������ʱ������
 */
void GPIO_Configuration(void)
{
    // GPIO��ʼ�������ṹ�壨�洢����ģʽ�����ù��ܵ����ã�
    GPIO_InitType GPIO_InitStructure;
    // ��ʼ���ṹ�壨�⺯��������������Ӱ�����ã�
    GPIO_InitStruct(&GPIO_InitStructure);
    
    /* -------------------------- ����GPIOE_PIN_1 -------------------------- */
    // 1. ѡ�����ţ�GPIOE_PIN_1��PWM������ţ�
    GPIO_InitStructure.Pin        = GPIO_PIN_1;
    // 2. ����ģʽ���������������GPIO_MODE_AF_PP��
    //    - ���ã����Ź��ܽ�����ʱ����������ͨGPIO��
    //    - ���죺�����ǿ��/�͵�ƽ���ʺ�����LED�����������
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF_PP;  
    // 3.  slew�ʣ����٣�GPIO_SLEW_RATE_SLOW��������PWM���εĸ�Ƶ����
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
    // 4. ���ù��ܣ�ָ��ΪGTIM10_CH1��GPIO_AF_GTIM10_CH1_PE1��Ӳ���̶���
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF_GTIM10_CH1_PE1;  
   
      // 5. ������Ӧ�õ�GPIOE�˿ڣ�ʹ������Ч��
    GPIO_InitPeripheral(GPIOE, &GPIO_InitStructure);
    
    // 6. ������ӳ�䣨��ѡ������оƬ��Ҫ��ȷ�����ù�����ȷӳ�䵽���ţ�
    //    - GPIOE_PORT_SOURCE���˿�ԴΪGPIOE
    //    - GPIO_PIN_SOURCE1������ԴΪPIN1
    //    - GPIO_AF1�����ù��ܱ�ţ�����GTIM10_CH1ƥ�䣬���ֲ�ȷ�ϣ�
    GPIO_ConfigPinRemap(GPIOE_PORT_SOURCE,  GPIO_PIN_SOURCE1, GPIO_AF1);
}

/**
 * @brief  ����GTIM10_CH1��PWMռ�ձ�
 * @param  duty_cycle��ռ�ձȲ�������Χ0~999����Ӧ0%~100%��
 * @return ��
 * @note   ԭ��PWMռ�ձ� = �Ƚ�ֵ / ����ֵ �� 100%������ֵ��ʱ��������Ϊ239��
 */
void Set_PWM_DutyCycle(uint16_t duty_cycle)
{
    // 1. �߽籣����ȷ��ռ�ձȲ�����100%��������>999��ǿ����Ϊ999��
    if(duty_cycle > 999) duty_cycle = 999;
    
    // 2. ����GTIM10_CH1�ıȽ�ֵ���Ƚ�ֵ����ռ�ձȣ�ֵԽ��ռ�ձ�Խ�ߣ�
    TIM_SetCmp1(GTIM10, duty_cycle);
    
    // 3. ������ʱ�������¼������µıȽ�ֵ������Ч������оƬ��Ҫ������ռ�ձ��ӳٸ��£�
    GTIM10->EVTGEN |= TIM_EVTGEN_UDGN ; // TIM_EVTGEN_UDGN�������¼�����λ�����ֲ�ȷ�Ϻ궨�壩
}

int main(void)
{
    DWT_INIT();
	// RS485-1 PA4-TX PA5-RX
	USART1_Configuration();
	// RS485-2 PC4-TX PC5-RX 
	UART7_Configuration();
	// RS485-3 PB0-TX PB1-RX
	USART4_Configuration();
	// RS485-4 PB2-TX PE7-RX
	UART6_Configuration();

    USART_Print(USART_LOG,"SystemCoreClock = %lu Hz\r\n", SystemCoreClock);  // ��ӡϵͳʱ��
     /* System Clocks Configuration */
    RCC_Configuration();
    /* GPIO Configuration */
    GPIO_Configuration();
   
      /* -------------------------- ��ʱ��ʱ�����ã�����PWMƵ�ʣ� -------------------------- */
    // ��ʼ��ʱ���ṹ�壨�⺯�������������ݣ�
    TIM_InitTimBaseStruct(&TIM_TimeBaseStructure);
    // 1. ����ֵ��Period������ʱ��������239��240-1��ʱ���ã����� = (Period+1) �� ��������
    TIM_TimeBaseStructure.Period    = 240 - 1;  
    // 2. Ԥ��Ƶϵ����Prescaler������ʱ��ʱ�ӷ�Ƶ1000������������ = ��Ƶ��ʱ������
    TIM_TimeBaseStructure.Prescaler = 1000 - 1;
    // 3. ʱ�ӷ�Ƶ��ClkDiv��������Ƶ��TIM_CLK_DIV1������ʱ��ʱ��ֱ�����ڼ���
    TIM_TimeBaseStructure.ClkDiv    = TIM_CLK_DIV1;
    // 4. ����ģʽ�����ϼ�����TIM_CNT_MODE_UP������0��Periodѭ������
    TIM_TimeBaseStructure.CounterMode   = TIM_CNT_MODE_UP;

    // 5. ��ʱ������Ӧ�õ�GTIM10��ʹ������Ч��
    TIM_InitTimeBase(GTIM10, &TIM_TimeBaseStructure);

  /* -------------------------- PWMģʽ���ã�GTIM10_CH1ͨ���� -------------------------- */
    // ��ʼ��PWM����ȽϽṹ�壨�⺯����
    TIM_InitOcStruct(&TIM_OCInitStructure);
    
    // 1. PWMģʽ��PWM1ģʽ������ģʽ��������<�Ƚ�ֵʱ����ߵ�ƽ������͵�ƽ��
    TIM_OCInitStructure.OCMode      = TIM_OCMODE_PWM1;
    // 2. ���״̬��ʹ�ܣ�TIM_OUTPUT_STATE_ENABLE��������PWM�źŴ��������
    TIM_OCInitStructure.OutputState = TIM_OUTPUT_STATE_ENABLE;
    // 3. ��ʼ�Ƚ�ֵ��Pulse����30��ռ�ձ� = 30/240 �� 100% = 12.5%��
    TIM_OCInitStructure.Pulse       = 30;  
    // 4. ������ԣ��ߵ�ƽ��Ч��TIM_OC_POLARITY_HIGH�������ߵ�ƽʱLED��������Ӳ��������
    TIM_OCInitStructure.OCPolarity  = TIM_OC_POLARITY_HIGH;

    // 5. ��PWM����Ӧ�õ�GTIM10_CH1ͨ����CH1��ӦGPIOE_PIN_1��
    TIM_InitOc1(GTIM10, &TIM_OCInitStructure);
    // 6. ʹ��CH1ͨ����Ԥװ�أ�TIM_OC_PRE_LOAD_ENABLE����ȷ���Ƚ�ֵ�޸ĺ��´�������Ч
    TIM_ConfigOc1Preload(GTIM10, TIM_OC_PRE_LOAD_ENABLE);

    // 7. ʹ�ܶ�ʱ���Զ���װ�أ�TIM_ConfigArPreload��������ֵ�޸ĺ��´�������Ч
    TIM_ConfigArPreload(GTIM10, ENABLE);


    /* -------------------------- ������ʱ������ʼ���PWM�� -------------------------- */
    // ʹ��GTIM10��������TIM_Enable������ʱ����ʼ������PWM�źŴ�GPIOE_PIN_1���
    TIM_Enable(GTIM10, ENABLE);
    
    // ʹ�ܶ�ʱ�������������оƬ��Ҫ������PWM�ź��޷���������ֲ�ȷ�ϣ�
    GTIM10->BKDT |= TIM_BKDT_MOEN;
    
     /* -------------------------- �������߼�����̬����ռ�ձȣ� -------------------------- */
    uint16_t duty = 999;  // ռ�ձȲ�������ʼ999����Ӧ�ӽ�100%��
    int8_t step = 0;      // ���������ڵ���ռ�ձȱ仯���򣬴�������δʹ�ã����Ż���
    
    while (1)  // ��ѭ�����������к������߼�
    {
        // 1. ����ռ�ձ�Ϊstep����ʼ0����Ӧ0%ռ�ձȣ�LED��
        TIM_SetCmp1(TIMx, step);
        delay_ms(1000);  // ��ʱ1�루���ֵ�ǰռ�ձ�1�룩
        for (volatile int i = 0; i < 30000; i++);  // ���������ʱ����ǿ�ȶ��ԣ�
            
        // 2. ����ռ�ձ�Ϊduty��999����Ӧ�ӽ�100%��LED������
        TIM_SetCmp1(TIMx, duty);
        delay_ms(1000);  // ��ʱ1��
        for (volatile int i = 0; i < 30000; i++);  // ���������ʱ
    }

}
