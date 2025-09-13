#include "init.h"
#include <string.h>
#include <stdio.h>

/*
*功能：根据Module_Union的值（如GPIOA、USART1）自动开启对应的时钟
*参数：Module_Union：联合体类型，可赋予各种Module（如GPIOA、USART1）
*返回值：无
*/
void RCC_INIT(Module_Union_t Module_Union)
{
 
    if (Module_Union.GPIO_M == GPIOA)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOA, ENABLE);
    }
    else if (Module_Union.GPIO_M == GPIOB)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOB, ENABLE);
    }
    else if (Module_Union.GPIO_M == GPIOC)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOC, ENABLE);
    }
    else if (Module_Union.GPIO_M == GPIOD)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOD, ENABLE);
    }
    else if (Module_Union.GPIO_M == GPIOE)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOE, ENABLE);
    }
    else if (Module_Union.GPIO_M == GPIOF)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOF, ENABLE);
    }
    else if (Module_Union.GPIO_M == GPIOG)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOG, ENABLE);
    }
    else if (Module_Union.GPIO_M == GPIOH)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOH, ENABLE);
    }
    // USART
    else if (Module_Union.USART_M == USART1)
    {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_USART1, ENABLE);
    }
    else if (Module_Union.USART_M == USART2)
    {
        RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_USART2, ENABLE);
    }
    else if (Module_Union.USART_M == USART3)
    {
        RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_USART3, ENABLE);
    }
    else if (Module_Union.USART_M == USART4)
    {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_USART4, ENABLE);
    }
    else if (Module_Union.USART_M == UART5)
    {
        RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_UART5, ENABLE);
    }
    else if (Module_Union.USART_M == UART6)
    {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_UART6, ENABLE);
    }
    else if (Module_Union.USART_M == UART7)
    {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_UART7, ENABLE);
    }
    else if (Module_Union.USART_M == UART8)
    {
        RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_UART8, ENABLE);
    }
    // TIM
    else if (Module_Union.TIM_M == GTIM1)
    {
        RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_GTIM1, ENABLE);
    }
    else if (Module_Union.TIM_M == GTIM2)
    {
        RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_GTIM2, ENABLE);
    }
    else if (Module_Union.TIM_M == GTIM3)
    {
        RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_GTIM3, ENABLE);
    }
    else if (Module_Union.TIM_M == GTIM4)
    {
        RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_GTIM4, ENABLE);
    }
    else if (Module_Union.TIM_M == GTIM5)
    {
        RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_GTIM5, ENABLE);
    }
    else if (Module_Union.TIM_M == GTIM6)
    {
        RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_GTIM6, ENABLE);
    }
    else if (Module_Union.TIM_M == GTIM7)
    {
        RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_GTIM7, ENABLE);
    }
    else if (Module_Union.TIM_M == GTIM8)
    {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GTIM8, ENABLE);
    }
    else if (Module_Union.TIM_M == GTIM9)
    {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GTIM9, ENABLE);
    }
    else if (Module_Union.TIM_M == GTIM10)
    {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GTIM10, ENABLE);
    }
    else if (Module_Union.TIM_M == BTIM1)
    {
        RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_BTIM1, ENABLE);
    }
    else if (Module_Union.TIM_M == BTIM2)
    {
        RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_BTIM2, ENABLE);
    }
    // ADC
    else if (Module_Union.ADC_M == ADC1)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_ADC1, ENABLE);
    }
    else if (Module_Union.ADC_M == ADC2)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_ADC2, ENABLE);
    }
    else if (Module_Union.ADC_M == ADC3)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_ADC3, ENABLE);
    }
    else if (Module_Union.ADC_M == ADC4)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_ADC4, ENABLE);
    }
    // DMA
    else if (Module_Union.DMA_M == DMA1)
    {
        RCC_EnableAHBPeriphClk(RCC_AHB_PERIPHEN_DMA1, ENABLE);
    }
    else if (Module_Union.DMA_M == DMA2)
    {
        RCC_EnableAHBPeriphClk(RCC_AHB_PERIPHEN_DMA2, ENABLE);
    }
}
/*
*功能：根据GPIO_Init_t结构体自动开启GPIO的时钟并初始化
*参数：GPIO_Init_Type：结构体包含GPIO的信息
*返回值：无
*/
void GPIO_INIT(GPIO_Init_t GPIO_Init_Type)
{
    // 开时钟
    RCC_INIT(MAKE_UNION_GPIO(GPIO_Init_Type.GPIO_M));

    GPIO_InitType GPIO_InitStructure;
    GPIO_InitStruct(&GPIO_InitStructure);

    GPIO_InitStructure = (GPIO_InitType)
    {
        .Pin            = GPIO_Init_Type.Pin,
        .GPIO_Mode      = GPIO_Init_Type.GPIO_Mode,
        .GPIO_Pull      = GPIO_Init_Type.GPIO_Pull,
        .GPIO_Alternate = GPIO_Init_Type.GPIO_Alternate,
    };

    GPIO_InitPeripheral(GPIO_Init_Type.GPIO_M, &GPIO_InitStructure);
}
/*
*功能：给GPIO引脚赋予电平并可选择延时一段时间
*参数：见函数
*返回值：无
*/
void GPIO_Write_Delayms(GPIO_Module* GPIOx, uint32_t Pin, Bit_OperateType BitCmd, uint32_t DelayTime)
{
    GPIO_WriteBits(GPIOx, Pin, BitCmd);
    delay_ms(DelayTime);
}
/*
*功能：给GPIO引脚赋予电平并可选择延时一段时间
*参数：见函数
*返回值：无
*/
void GPIO_Write_Delayus(GPIO_Module* GPIOx, uint32_t Pin, Bit_OperateType BitCmd, uint32_t DelayTime)
{
    GPIO_WriteBits(GPIOx, Pin, BitCmd);
    delay_us(DelayTime);
}
/*
*功能：根据USART_Init_t结构体自动开启USART的时钟并初始化
*参数：USART_Init_Type：结构体包含USART的信息
*返回值：无
*/
void USART_INIT(USART_Init_t USART_Init_Type)
{
    // 开时钟
	RCC_INIT(MAKE_UNION_DMA(DMA1));
    RCC_INIT(MAKE_UNION_GPIO(USART_Init_Type.RX_Init_Type->GPIO_M));
    RCC_INIT(MAKE_UNION_GPIO(USART_Init_Type.TX_Init_Type->GPIO_M));
    RCC_INIT(MAKE_UNION_USART(USART_Init_Type.USART_M));

    // GPIO初始化
    GPIO_InitType GPIO_InitStructure;
    GPIO_InitStruct(&GPIO_InitStructure);
    // RX初始化
    GPIO_InitStructure = (GPIO_InitType)
    {
        .Pin            = USART_Init_Type.RX_Init_Type->Pin,
        .GPIO_Mode      = GPIO_MODE_AF_PP,
        .GPIO_Pull      = GPIO_PULL_UP,
        .GPIO_Alternate = USART_Init_Type.RX_Init_Type->GPIO_Alternate,
    };
    GPIO_InitPeripheral(USART_Init_Type.RX_Init_Type->GPIO_M, &GPIO_InitStructure);
    // TX初始化
    GPIO_InitStructure = (GPIO_InitType)
    {
        .Pin            = USART_Init_Type.TX_Init_Type->Pin,
        .GPIO_Mode      = GPIO_MODE_AF_PP,
        .GPIO_Pull      = GPIO_PULL_UP,
        .GPIO_Alternate = USART_Init_Type.TX_Init_Type->GPIO_Alternate,
    };
    GPIO_InitPeripheral(USART_Init_Type.TX_Init_Type->GPIO_M, &GPIO_InitStructure);
    // 中断初始化
    NVIC_InitType NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel                   = USARTx_IRQn(USART_Init_Type.USART_M);
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    DMA_InitType DMA_InitStructure;

    DMA_DeInit(USART_Init_Type.DMAChx);
    DMA_StructInit(&DMA_InitStructure);
    DMA_InitStructure.PeriphAddr     = (uint32_t)&USART_Init_Type.USART_M->DAT;
    DMA_InitStructure.MemAddr        = (uint32_t)USART_Init_Type.RXBuf;
    DMA_InitStructure.Direction      = DMA_DIR_PERIPH_SRC;
    DMA_InitStructure.BufSize        = USART_Init_Type.RXBufSize;
    DMA_InitStructure.PeriphInc      = DMA_PERIPH_INC_DISABLE;
    DMA_InitStructure.MemoryInc      = DMA_MEM_INC_ENABLE;
    DMA_InitStructure.PeriphDataSize = DMA_PERIPH_DATA_WIDTH_BYTE;
    DMA_InitStructure.MemDataSize    = DMA_MEM_DATA_WIDTH_BYTE;
    DMA_InitStructure.CircularMode   = DMA_MODE_NORMAL;
    DMA_InitStructure.Priority       = DMA_PRIORITY_VERY_HIGH;
    DMA_InitStructure.Mem2Mem        = DMA_M2M_DISABLE;
    DMA_Init(USART_Init_Type.DMAChx, &DMA_InitStructure);
    DMA_RequestRemap(USART_Init_Type.DMARemap, USART_Init_Type.DMAChx, ENABLE);

    // 串口初始化
    USART_InitType USART_InitStructure;
    USART_StructInit(&USART_InitStructure);
    USART_InitStructure.BaudRate            = USART_Init_Type.Baud;
    USART_InitStructure.WordLength          = USART_WL_8B;
    USART_InitStructure.StopBits            = USART_STPB_1;
    USART_InitStructure.Parity              = USART_PE_NO;
    USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
    USART_InitStructure.OverSampling        = USART_16OVER;
    USART_InitStructure.Mode                = USART_MODE_TX|USART_MODE_RX;

    USART_Init(USART_Init_Type.USART_M, &USART_InitStructure);
    USART_ConfigInt(USART_Init_Type.USART_M, USART_INT_IDLEF, ENABLE);
    USART_ClrFlag(USART_Init_Type.USART_M, USART_FLAG_IDLEF);
    USART_EnableDMA(USART_Init_Type.USART_M, USART_DMAREQ_RX, ENABLE);
    DMA_EnableChannel(USART_Init_Type.DMAChx, ENABLE);
    USART_Enable(USART_Init_Type.USART_M, ENABLE);
}
/*
*功能：根据USART_Module返回对应的中断号
*参数：见函数
*返回值：USART中断号
*备注：默认返回USART1_IRQn
*/
IRQn_Type USARTx_IRQn(USART_Module* USART_M)
{
    if (USART_M == USART1)
        return USART1_IRQn;
    else if (USART_M == USART2)
        return USART2_IRQn;
    else if (USART_M == USART3)
        return USART3_IRQn;
    else if (USART_M == USART4)
        return USART4_IRQn;
    else if (USART_M == UART5)
        return UART5_IRQn;
    else if (USART_M == UART6)
        return UART6_IRQn;
    else if (USART_M == UART7)
        return UART7_IRQn;
    else if (USART_M == UART8)
        return UART8_IRQn;
    return USART1_IRQn;
}
/*
*功能：初始化软件I2C的两个引脚
*参数：见函数
*返回值：无
*/
void SI2C_INIT(SI2C_Init_t SI2C_Init_Type)
{
    // 开时钟
    RCC_INIT(MAKE_UNION_GPIO(SI2C_Init_Type.SCL_M));
    RCC_INIT(MAKE_UNION_GPIO(SI2C_Init_Type.SDA_M));

    // 配置GPIO
    GPIO_Init_t GPIO_Init_Type;
    GPIO_Init_Type = (GPIO_Init_t)
    {
        .GPIO_M 		= SI2C_Init_Type.SCL_M,
		.Pin			= SI2C_Init_Type.SCL_Pin,
		.GPIO_Mode		= GPIO_MODE_OUTPUT_OD,
		.GPIO_Pull		= GPIO_NO_PULL,
		.GPIO_Alternate	= GPIO_AF0,
    };
    GPIO_INIT(GPIO_Init_Type);
    GPIO_Init_Type = (GPIO_Init_t)
    {
        .GPIO_M 		= SI2C_Init_Type.SDA_M,
		.Pin			= SI2C_Init_Type.SDA_Pin,
		.GPIO_Mode		= GPIO_MODE_OUTPUT_OD,
		.GPIO_Pull		= GPIO_NO_PULL,
		.GPIO_Alternate	= GPIO_AF0,
    };
    GPIO_INIT(GPIO_Init_Type);
    GPIO_WriteBits(SI2C_Init_Type.SCL_M, SI2C_Init_Type.SCL_Pin, Bit_SET);
    GPIO_WriteBits(SI2C_Init_Type.SDA_M, SI2C_Init_Type.SDA_Pin, Bit_SET);
}

/*
*功能：初始化ADC输出
*参数：见函数
*返回值：无
*/
void ADC_INIT(ADC_Init_t ADC_Init_Type)
{
	// 开时钟
	RCC_INIT(MAKE_UNION_ADC(ADC_Init_Type.ADC_M));
	RCC_INIT(MAKE_UNION_DMA(ADC_Init_Type.DMA_M));
	ADC_ConfigClk(ADC_CTRL3_CKMOD_AHB,RCC_ADCHCLK_DIV16);
	
	// 配置DMA
	DMA_DeInit(ADC_Init_Type.DMAx_CHx);
	DMA_InitType DMA_InitStructure;
    DMA_InitStructure.PeriphAddr     	= (uint32_t)&ADC_Init_Type.ADC_M->DAT;          //adc的地址
    DMA_InitStructure.MemAddr        	= (uint32_t)ADC_Init_Type.Result_Addr;			//存储数据地址
    DMA_InitStructure.Direction      	= DMA_DIR_PERIPH_SRC;    						//传输方向，外设-->内存
    DMA_InitStructure.BufSize        	= ADC_Init_Type.Channel_Num;                    //存储数据大小   
    DMA_InitStructure.PeriphInc     	= DMA_PERIPH_INC_DISABLE;						//外设地址不增量
    DMA_InitStructure.MemoryInc			= DMA_MEM_INC_ENABLE;    						//内存地址增量
    DMA_InitStructure.PeriphDataSize 	= DMA_PERIPH_DATA_WIDTH_HALFWORD; 				//外设数据宽度，半字16位
    DMA_InitStructure.MemDataSize    	= DMA_MEM_DATA_WIDTH_HALFWORD;   				//外设与内存数据宽度应为一样
    DMA_InitStructure.CircularMode   	= DMA_MODE_CIRCULAR;             				//DMA传输模式为循环传输
    DMA_InitStructure.Priority       	= DMA_PRIORITY_HIGH;             				//通道优先级为高
    DMA_InitStructure.Mem2Mem       	= DMA_M2M_DISABLE;               				//关闭从存储器到存储器模式
    DMA_Init(ADC_Init_Type.DMAx_CHx, &DMA_InitStructure);
    
    DMA_RequestRemap(ADC_Init_Type.DMA_Remap, ADC_Init_Type.DMAx_CHx, ENABLE);
    
    DMA_EnableChannel(ADC_Init_Type.DMAx_CHx, ENABLE);
	
	ADC_InitType ADC_InitStructure;
    ADC_InitStructure.WorkMode       = ADC_WORKMODE_INDEPENDENT;
    ADC_InitStructure.MultiChEn      = ENABLE;
    ADC_InitStructure.ContinueConvEn = ENABLE;
    ADC_InitStructure.ExtTrigSelect  = ADC_EXT_TRIG_REG_CONV_SOFTWARE;
    ADC_InitStructure.DatAlign       = ADC_DAT_ALIGN_R;
    ADC_InitStructure.ChsNumber      = ADC_Init_Type.Channel_Num;
    ADC_InitStructure.Resolution     = ADC_DATA_RES_12BIT;
    ADC_Init(ADC_Init_Type.ADC_M, &ADC_InitStructure);
	
    // 配置ADC_Init_Type.ADC_M输出口和采样时间
	for (uint32_t i = 0; i < ADC_Init_Type.Channel_Num; i++)
	{
		volatile uint8_t Rank = i + 1;
		ADC_ConfigRegularChannel(ADC_Init_Type.ADC_M, ADC_Init_Type.Channel_Group[i], Rank, ADC_Init_Type.SampleTime);
	}
    ADC_Enable(ADC_Init_Type.ADC_M, ENABLE);
	// 确认ADC就绪
    while(ADC_GetFlagStatus(ADC_Init_Type.ADC_M,ADC_FLAG_RDY) == RESET);
    ADC_CalibrationOperation(ADC_Init_Type.ADC_M,ADC_CALIBRATION_SINGLE_MODE);
	// ADC校准
    while (ADC_GetCalibrationStatus(ADC_Init_Type.ADC_M,ADC_CALIBRATION_SINGLE_MODE));
    ADC_SetDMATransferMode(ADC_Init_Type.ADC_M, ADC_MULTI_REG_DMA_EACH_ADC);
}

void DWT_INIT(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // 使能 DWT 计数器
    DWT->CYCCNT = 0;  // 计数器清零
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk; // 使能 CYCCNT 计数功能
}

void delay_us(uint32_t us)
{
    uint32_t start = DWT->CYCCNT;  // 记录起始计数值
    uint32_t ticks = us * (SystemCoreClock / 1000000); // 计算需要的周期数
    while ((DWT->CYCCNT - start) < ticks); // 等待计时完成
}


void delay_ms(uint32_t ms)
{
    while (ms--)
        delay_us(1000);
}

void delay_loop(uint32_t us)
{
    uint32_t cycles = us * (240);
    volatile uint32_t i;
    for (i = 0; i < cycles; i++) 
    {
        __NOP();
    }
}

