#include "main.h"

void GTIM8_INIT(void)
{
        // 1. ��GTIM8��ʱ��"ͨ��"������ʱ�ӣ�
        // ��Ƭ������Ĭ����"�ϵ�"�ģ������ȿ�ʱ�Ӳ��ܹ���
        // RCC_APB2_PERIPH_GTIM8��ָ��Ҫ��ʱ�ӵ�������GTIM8
        // ENABLE������ʱ�ӣ����ư���Դ���أ�
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GTIM8, ENABLE);
        
        // 2. ���ö�ʱ���жϵ�"���ȼ�"������CPU�������ʱ����������Ҫ�̶ȣ�
        // NVIC_InitType���ж����ýṹ�壬�������жϵĲ���
        NVIC_InitType NVIC_InitStructure;
        // ָ��Ҫ���õ��жϣ�GTIM8���жϣ�����CPU���ĸ��豸���жϣ�
        NVIC_InitStructure.NVIC_IRQChannel                   = GTIM8_IRQn;
        // ������ռ���ȼ�����ֵԽС���ж����ȼ�Խ��
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
        // ���������ȼ����������ȼ����ã�����ͬ��ռ���ȼ����ж�����
        NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
        // ʹ�ܸ��ж�ͨ��
        NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
        // Ӧ���ж�����
        NVIC_Init(&NVIC_InitStructure);

        // ����ʱ����Ԫ
        TIM_TimeBaseInitType TIM_TimeBaseStructure;

        TIM_InitTimBaseStruct(&TIM_TimeBaseStructure);
        // ���÷�Ƶϵ����ϵͳʱ�Ӿ�����Ƶ����Ϊ��ʱ������ʱ��
        // ���㣺����ϵͳʱ��Ϊ24MHz��24000��Ƶ�󣬼���ʱ��Ϊ1000Hz������1ms��
        TIM_TimeBaseStructure.Prescaler     = 24000-1;//��Ƶ
        // ���ü������ڣ�װ��ֵ������������ֵ�󴥷��жϲ�����
        // ���㣺1000���������� �� 1ms���������ڣ�= 1000ms = 1s����1�봥��һ���ж�
        TIM_TimeBaseStructure.Period        = 1000 - 1;//װ��ֵ
        // ʱ�ӷ�Ƶ���ӣ�1��Ƶ����ʱ��ʱ�������ʱ��Ƶ����ͬ��
        TIM_TimeBaseStructure.ClkDiv        = TIM_CLK_DIV1;
        // ����ģʽ�����ϼ�������0��ʼ������Periodֵ��
        TIM_TimeBaseStructure.CounterMode   = TIM_CNT_MODE_UP;

        TIM_InitTimeBase(GTIM8, &TIM_TimeBaseStructure);
        // ʹ�ܶ�ʱ�������жϣ�������Periodֵʱ������
        TIM_ConfigInt(GTIM8, TIM_INT_UPDATE, ENABLE);
        // ������ʱ������
        TIM_Enable(GTIM8, ENABLE);
}

/**
 * @brief  GTIM8��ʱ���жϷ�����
 * @note   ��ʱ�������ж�ʱ���Զ�ִ�д˺����ڵĴ���
 */
void GTIM8_IRQHandler(void)
{
     // ����Ƿ�ΪGTIM8�ĸ����жϴ��������������жϣ� 
    if (TIM_GetIntStatus(GTIM8, TIM_INT_UPDATE) != RESET)
    {
        // ��ת����ָʾ��״̬�������� �� �������
        LED(LED_RUN,CHANGE);
        delay_ms(1000);
        // ע�⣺�жϷ�����Ӧ������̣�����ʹ�ó���ʱ
        // ԭ�е�delay_ms(1000)�ᵼ��ϵͳ�����������Ƴ�
        
        // ����жϱ�־λ
        // ˵�������������־�������жϻᱻ��������
        TIM_ClearFlag(GTIM8, TIM_INT_UPDATE);
    }
}
/**
 * @brief  ������������ִ�е���ڵ�
 */
int main(void)
{
    // �����ж����ȼ����飨������ռ���ȼ��������ȼ���λ�����䣩
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    // ��ʼ��DWT��ʱ�������ڸ߾�����ʱ�ȹ��ܣ�
    DWT_INIT();
    // ��ʼ��LEDָʾ�ƣ�����GPIO����Ϊ���ģʽ��
    LED_Configuration();
    // ��ʼ��GTIM8��ʱ�������ö�ʱ�жϣ�
    GTIM8_INIT();

    // ��ѭ���������ڴ˴�ѭ��ִ�У��ȴ��жϴ���
    while(1)
    {
        // �����������������룬�жϴ���ʱ����ʱ��ϴ˴�ִ��
    }
}

