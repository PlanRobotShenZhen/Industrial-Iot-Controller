#include "main.h"

void GTIM8_INIT(void)
{
        // 1. 给GTIM8定时器"通电"（开启时钟）
        // 单片机外设默认是"断电"的，必须先开时钟才能工作
        // RCC_APB2_PERIPH_GTIM8：指定要开时钟的外设是GTIM8
        // ENABLE：开启时钟（类似按电源开关）
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GTIM8, ENABLE);
        
        // 2. 配置定时器中断的"优先级"（告诉CPU：这个定时器的提醒重要程度）
        // NVIC_InitType：中断配置结构体，用来存中断的参数
        NVIC_InitType NVIC_InitStructure;
        // 指定要配置的中断：GTIM8的中断（告诉CPU是哪个设备的中断）
        NVIC_InitStructure.NVIC_IRQChannel                   = GTIM8_IRQn;
        // 配置抢占优先级：数值越小，中断优先级越高
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
        // 配置子优先级：辅助优先级设置，用于同抢占优先级的中断排序
        NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
        // 使能该中断通道
        NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
        // 应用中断配置
        NVIC_Init(&NVIC_InitStructure);

        // 配置时基单元
        TIM_TimeBaseInitType TIM_TimeBaseStructure;

        TIM_InitTimBaseStruct(&TIM_TimeBaseStructure);
        // 配置分频系数：系统时钟经过分频后作为定时器计数时钟
        // 计算：假设系统时钟为24MHz，24000分频后，计数时钟为1000Hz（周期1ms）
        TIM_TimeBaseStructure.Prescaler     = 24000-1;//分频
        // 配置计数周期（装载值）：计数到该值后触发中断并清零
        // 计算：1000个计数周期 × 1ms（计数周期）= 1000ms = 1s，即1秒触发一次中断
        TIM_TimeBaseStructure.Period        = 1000 - 1;//装载值
        // 时钟分频因子：1分频（定时器时钟与计数时钟频率相同）
        TIM_TimeBaseStructure.ClkDiv        = TIM_CLK_DIV1;
        // 计数模式：向上计数（从0开始递增到Period值）
        TIM_TimeBaseStructure.CounterMode   = TIM_CNT_MODE_UP;

        TIM_InitTimeBase(GTIM8, &TIM_TimeBaseStructure);
        // 使能定时器更新中断（计数到Period值时触发）
        TIM_ConfigInt(GTIM8, TIM_INT_UPDATE, ENABLE);
        // 启动定时器计数
        TIM_Enable(GTIM8, ENABLE);
}

/**
 * @brief  GTIM8定时器中断服务函数
 * @note   定时器触发中断时，自动执行此函数内的代码
 */
void GTIM8_IRQHandler(void)
{
     // 检查是否为GTIM8的更新中断触发（过滤其他中断） 
    if (TIM_GetIntStatus(GTIM8, TIM_INT_UPDATE) != RESET)
    {
        // 翻转运行指示灯状态（亮→灭 或 灭→亮）
        LED(LED_RUN,CHANGE);
        delay_ms(1000);
        // 注意：中断服务函数应尽量简短，避免使用长延时
        // 原有的delay_ms(1000)会导致系统阻塞，建议移除
        
        // 清除中断标志位
        // 说明：必须清除标志，否则中断会被持续触发
        TIM_ClearFlag(GTIM8, TIM_INT_UPDATE);
    }
}
/**
 * @brief  主函数，程序执行的入口点
 */
int main(void)
{
    // 配置中断优先级分组（设置抢占优先级和子优先级的位数分配）
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    // 初始化DWT计时器（用于高精度延时等功能）
    DWT_INIT();
    // 初始化LED指示灯（配置GPIO引脚为输出模式）
    LED_Configuration();
    // 初始化GTIM8定时器（配置定时中断）
    GTIM8_INIT();

    // 主循环：程序在此处循环执行，等待中断触发
    while(1)
    {
        // 可添加其他主任务代码，中断触发时会暂时打断此处执行
    }
}

