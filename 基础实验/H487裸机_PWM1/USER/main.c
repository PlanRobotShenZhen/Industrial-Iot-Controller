#include "main.h"
// 1. 包含定时器库头文件
#include "n32h47x_48x_tim.h"
// 2. 包含GPIO库头文件（GPIO配置函数）
#include "n32h47x_48x_gpio.h"
// 3. 包含时钟库头文件（时钟使能函数）
#include "n32h47x_48x_rcc.h"

// 全局变量声明（与库结构体类型匹配）

TIM_TimeBaseInitType TIM_TimeBaseStructure;  // 定时器时基结构体（库已定义）
OCInitType TIM_OCInitStructure;              // PWM输出比较结构体（库已定义）
uint32_t CCR1_Val       = 600;               // GTIM10_CH1 初始比较值
uint32_t PrescalerValue = 0;                 // 预分频系数

// 定时器定义
#define TIMx           GTIM10
#define TIMx_CLK       RCC_APB2_PERIPH_GTIM10  // 假设 GTIM10 时钟使能在 APB2，需根据芯片手册确认，若不对则修改
// 引脚定义
#define TIMx_CH1_GPIO_PORT    GPIOE
#define TIMx_CH1_GPIO_PIN     GPIO_PIN_1
#define TIMx_CH1_GPIO_CLK     RCC_AHB_PERIPHEN_GPIOE //RCC_AHB_PERIPH_GPIOE
#define TIMx_CH1_GPIO_AF      GPIO_AF_GTIM10_CH1_PE1  // 对应 GPIO_AF_GTIM10_CH1_PE1 的复用功能

/**
 * @brief  配置PWM相关外设的时钟（GTIM10、GPIOE、AFIO）
 * @param  无
 * @return 无
 * @note   关键：不同外设的时钟总线不同（APB1/APB2/AHB1），必须选对，否则外设不工作
 */
void RCC_Configuration(void)
{
    // 1. 使能GTIM10的时钟（假设在APB2总线，若实际是APB1，需改为RCC_APB1_PERIPH_GTIM10）
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GTIM10, ENABLE); 
    // 2. 使能GPIOE的时钟（PWM引脚在GPIOE，必须开启）
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOE, ENABLE); 
    // 3. 使能AFIO的时钟（引脚复用功能依赖AFIO，必须开启，否则GPIO无法复用为定时器输出）
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE); 
}
/**
 * @brief  配置GPIOE_PIN_1为GTIM10_CH1的PWM输出引脚（复用推挽模式）
 * @param  无
 * @return 无
 * @note   核心：复用模式是PWM输出的关键，普通输出模式无法输出定时器波形
 */
void GPIO_Configuration(void)
{
    // GPIO初始化参数结构体（存储引脚模式、复用功能等配置）
    GPIO_InitType GPIO_InitStructure;
    // 初始化结构体（库函数，避免脏数据影响配置）
    GPIO_InitStruct(&GPIO_InitStructure);
    
    /* -------------------------- 配置GPIOE_PIN_1 -------------------------- */
    // 1. 选择引脚：GPIOE_PIN_1（PWM输出引脚）
    GPIO_InitStructure.Pin        = GPIO_PIN_1;
    // 2. 引脚模式：复用推挽输出（GPIO_MODE_AF_PP）
    //    - 复用：引脚功能交给定时器（而非普通GPIO）
    //    - 推挽：能输出强高/低电平，适合驱动LED、电机等外设
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF_PP;  
    // 3.  slew率：慢速（GPIO_SLEW_RATE_SLOW），减少PWM波形的高频干扰
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
    // 4. 复用功能：指定为GTIM10_CH1（GPIO_AF_GTIM10_CH1_PE1，硬件固定）
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF_GTIM10_CH1_PE1;  
   
      // 5. 将配置应用到GPIOE端口（使配置生效）
    GPIO_InitPeripheral(GPIOE, &GPIO_InitStructure);
    
    // 6. 引脚重映射（可选，部分芯片需要：确保复用功能正确映射到引脚）
    //    - GPIOE_PORT_SOURCE：端口源为GPIOE
    //    - GPIO_PIN_SOURCE1：引脚源为PIN1
    //    - GPIO_AF1：复用功能编号（需与GTIM10_CH1匹配，查手册确认）
    GPIO_ConfigPinRemap(GPIOE_PORT_SOURCE,  GPIO_PIN_SOURCE1, GPIO_AF1);
}

/**
 * @brief  设置GTIM10_CH1的PWM占空比
 * @param  duty_cycle：占空比参数（范围0~999，对应0%~100%）
 * @return 无
 * @note   原理：PWM占空比 = 比较值 / 周期值 × 100%（周期值在时基配置中为239）
 */
void Set_PWM_DutyCycle(uint16_t duty_cycle)
{
    // 1. 边界保护：确保占空比不超过100%（若输入>999，强制设为999）
    if(duty_cycle > 999) duty_cycle = 999;
    
    // 2. 设置GTIM10_CH1的比较值（比较值决定占空比，值越大，占空比越高）
    TIM_SetCmp1(GTIM10, duty_cycle);
    
    // 3. 触发定时器更新事件（让新的比较值立即生效，部分芯片需要，否则占空比延迟更新）
    GTIM10->EVTGEN |= TIM_EVTGEN_UDGN ; // TIM_EVTGEN_UDGN：更新事件触发位（查手册确认宏定义）
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

    USART_Print(USART_LOG,"SystemCoreClock = %lu Hz\r\n", SystemCoreClock);  // 打印系统时钟
     /* System Clocks Configuration */
    RCC_Configuration();
    /* GPIO Configuration */
    GPIO_Configuration();
   
      /* -------------------------- 定时器时基配置（决定PWM频率） -------------------------- */
    // 初始化时基结构体（库函数，避免脏数据）
    TIM_InitTimBaseStruct(&TIM_TimeBaseStructure);
    // 1. 周期值（Period）：定时器计数到239（240-1）时重置，周期 = (Period+1) × 计数周期
    TIM_TimeBaseStructure.Period    = 240 - 1;  
    // 2. 预分频系数（Prescaler）：定时器时钟分频1000倍，计数周期 = 分频后时钟周期
    TIM_TimeBaseStructure.Prescaler = 1000 - 1;
    // 3. 时钟分频（ClkDiv）：不分频（TIM_CLK_DIV1），定时器时钟直接用于计数
    TIM_TimeBaseStructure.ClkDiv    = TIM_CLK_DIV1;
    // 4. 计数模式：向上计数（TIM_CNT_MODE_UP），从0→Period循环计数
    TIM_TimeBaseStructure.CounterMode   = TIM_CNT_MODE_UP;

    // 5. 将时基配置应用到GTIM10（使配置生效）
    TIM_InitTimeBase(GTIM10, &TIM_TimeBaseStructure);

  /* -------------------------- PWM模式配置（GTIM10_CH1通道） -------------------------- */
    // 初始化PWM输出比较结构体（库函数）
    TIM_InitOcStruct(&TIM_OCInitStructure);
    
    // 1. PWM模式：PWM1模式（常用模式，计数器<比较值时输出高电平，否则低电平）
    TIM_OCInitStructure.OCMode      = TIM_OCMODE_PWM1;
    // 2. 输出状态：使能（TIM_OUTPUT_STATE_ENABLE），允许PWM信号从引脚输出
    TIM_OCInitStructure.OutputState = TIM_OUTPUT_STATE_ENABLE;
    // 3. 初始比较值（Pulse）：30（占空比 = 30/240 × 100% = 12.5%）
    TIM_OCInitStructure.Pulse       = 30;  
    // 4. 输出极性：高电平有效（TIM_OC_POLARITY_HIGH），即高电平时LED亮（根据硬件调整）
    TIM_OCInitStructure.OCPolarity  = TIM_OC_POLARITY_HIGH;

    // 5. 将PWM配置应用到GTIM10_CH1通道（CH1对应GPIOE_PIN_1）
    TIM_InitOc1(GTIM10, &TIM_OCInitStructure);
    // 6. 使能CH1通道的预装载（TIM_OC_PRE_LOAD_ENABLE），确保比较值修改后下次周期生效
    TIM_ConfigOc1Preload(GTIM10, TIM_OC_PRE_LOAD_ENABLE);

    // 7. 使能定时器自动重装载（TIM_ConfigArPreload），周期值修改后下次周期生效
    TIM_ConfigArPreload(GTIM10, ENABLE);


    /* -------------------------- 启动定时器（开始输出PWM） -------------------------- */
    // 使能GTIM10计数器（TIM_Enable），定时器开始计数，PWM信号从GPIOE_PIN_1输出
    TIM_Enable(GTIM10, ENABLE);
    
    // 使能定时器主输出（部分芯片需要，否则PWM信号无法输出，查手册确认）
    GTIM10->BKDT |= TIM_BKDT_MOEN;
    
     /* -------------------------- 呼吸灯逻辑（动态调整占空比） -------------------------- */
    uint16_t duty = 999;  // 占空比参数（初始999，对应接近100%）
    int8_t step = 0;      // 步长（用于调整占空比变化方向，代码中暂未使用，需优化）
    
    while (1)  // 死循环，持续运行呼吸灯逻辑
    {
        // 1. 设置占空比为step（初始0，对应0%占空比，LED灭）
        TIM_SetCmp1(TIMx, step);
        delay_ms(1000);  // 延时1秒（保持当前占空比1秒）
        for (volatile int i = 0; i < 30000; i++);  // 额外软件延时（增强稳定性）
            
        // 2. 设置占空比为duty（999，对应接近100%，LED最亮）
        TIM_SetCmp1(TIMx, duty);
        delay_ms(1000);  // 延时1秒
        for (volatile int i = 0; i < 30000; i++);  // 额外软件延时
    }

}
