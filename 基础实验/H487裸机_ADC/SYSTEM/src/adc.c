
#include "adc.h"

// 16位ADC转换结果存储变量（__IO修饰：表示“易失性”，防止编译器优化掉）
// ADC是12位分辨率，所以结果范围是0~4095（2^12=4096）
__IO uint16_t ADCConvertedValue;

// ADC初始化参数结构体（存储ADC的工作模式、分辨率等配置）
ADC_InitType ADC_InitStructure;
// DMA初始化参数结构体（存储DMA的传输方向、缓冲区大小等配置）
DMA_InitType DMA_InitStructure;


// 函数声明：提前告诉编译器这些函数会在后面定义（避免编译报错）
void RCC_Configuration_ADC(void);  // 配置ADC相关的时钟
void GPIO_Configuration_ADC(void);  // 配置ADC对应的GPIO引脚
void DMA_Config(void);              // 配置DMA（用于自动传输ADC数据）


/**
*\*\name    ADC_Initial.
*\*\fun     ADC核心初始化函数（配置ADC的工作参数）
*\*\return  none
**/
void ADC_Initial(void)
{
        /* -------------------------- ADC1 基础配置 -------------------------- */
    // 1. 工作模式：独立模式（ADC1单独工作，不与其他ADC协同）
    ADC_InitStructure.WorkMode       = ADC_WORKMODE_INDEPENDENT;
    // 2. 多通道使能：禁用（当前只采样1个通道，不需要多通道切换）
    ADC_InitStructure.MultiChEn      = DISABLE;
    // 3. 连续转换使能：使能（ADC采样一次后，自动开始下一次采样，持续输出数据）
    ADC_InitStructure.ContinueConvEn = ENABLE;
    // 4. 外部触发选择：软件触发（通过代码启动ADC转换，不用外部引脚触发）
    ADC_InitStructure.ExtTrigSelect  = ADC_EXT_TRIG_REG_CONV_SOFTWARE;
    // 5. 数据对齐：右对齐（ADC是12位数据，右对齐时，结果的低12位有效，高4位补0，方便计算）
    ADC_InitStructure.DatAlign       = ADC_DAT_ALIGN_R;
    // 6. 通道数量：1个（当前只采样PB1引脚对应的通道）
    ADC_InitStructure.ChsNumber      = 1;
    // 7. 分辨率：12位（ADC能区分的最小电压是“参考电压/4095”，比如3.3V参考电压时，最小分辨率≈0.8mV）
    ADC_InitStructure.Resolution     = ADC_DATA_RES_12BIT;
    // 8. 将以上配置应用到ADC1（调用底层函数，让配置生效）
    ADC_Init(ADC1, &ADC_InitStructure);
    
    
        /* -------------------------- ADC1 采样通道配置 -------------------------- */
    // 配置ADC1的“常规采样通道”：
    // 参数1：ADC1（要配置的ADC模块）
    // 参数2：ADC1_Channel_12_PB1（采样通道：PB1引脚对应ADC1的第12通道，硬件固定）
    // 参数3：1（通道优先级：多通道时才有用，当前单通道设为1即可）
    // 参数4：ADC_SAMP_TIME_CYCLES_13_5（采样时间：13.5个ADC时钟周期，采样时间越长，数据越稳定）
    ADC_ConfigRegularChannel(ADC1, ADC1_Channel_12_PB1, 1, ADC_SAMP_TIME_CYCLES_13_5);
    
       /* -------------------------- 启动ADC并校准 -------------------------- */
    // 1. 使能ADC1（给ADC上电，准备工作）
    ADC_Enable(ADC1, ENABLE);
    // 2. 等待ADC1准备就绪（ADC上电后需要一点时间初始化，直到就绪标志置1）
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_RDY) == RESET)
        ;  // 没就绪就循环等待，直到就绪
    // 3. 启动ADC1校准（ADC出厂有微小误差，校准后能提高采样精度）
    ADC_CalibrationOperation(ADC1, ADC_CALIBRATION_SINGLE_MODE);
    // 4. 等待校准完成（校准过程需要时间，直到校准状态标志置0）
    while (ADC_GetCalibrationStatus(ADC1, ADC_CALIBRATION_SINGLE_MODE))
        ;  // 没完成就循环等待，直到完成
     // 5. 启动ADC1软件转换（手动触发第一次采样，后续因“连续转换使能”会自动采样）
    ADC_EnableSoftwareStartConv(ADC1, ENABLE);

}
/**
*\*\name    ADC_init.
*\*\fun     ADC完整初始化入口（整合时钟、GPIO、DMA、ADC核心配置）
*\*\return  none
**/
void ADC_init(void)
{
    /* 1. 配置ADC相关的时钟（给ADC、GPIO、DMA模块上电） */
    RCC_Configuration_ADC();

    /* 2. 配置ADC对应的GPIO引脚（将PB1设为模拟输入模式） */
    GPIO_Configuration_ADC();

    /* 3. 配置DMA（用于自动传输ADC转换结果，避免CPU频繁读取） */
    DMA_Config();
    
    /* 4. 配置ADC核心参数（工作模式、采样通道等） */
    ADC_Initial();
    
    /* 5. 再次启动ADC软件转换（确保连续采样正常开始） */
    ADC_EnableSoftwareStartConv(ADC1, ENABLE);
}

/**
*\*\name    RCC_Configuration_ADC.
*\*\fun     配置ADC相关的时钟（使能ADC、GPIO、DMA的时钟）
*\*\return  none
**/
void RCC_Configuration_ADC(void)
{
   /* -------------------------- 使能外设时钟 -------------------------- */
    // 1. 使能DMA1时钟（DMA1用于传输ADC数据，需要上电）
    RCC_EnableAHBPeriphClk(RCC_AHB_PERIPHEN_DMA1, ENABLE);

    // 2. 使能GPIOA、GPIOB、GPIOC时钟（当前用GPIOB的PB1引脚，其他GPIO留作扩展）
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOA | RCC_AHB_PERIPHEN_GPIOB | RCC_AHB_PERIPHEN_GPIOC, ENABLE);
    
    // 3. 使能ADC1时钟（ADC1模块上电，才能工作）
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_ADC1, ENABLE);

    /* -------------------------- 配置ADC时钟频率 -------------------------- */
    // 1. ADC时钟源选择AHB时钟，并分频16倍（确保ADC时钟频率在允许范围，避免采样错误）
    ADC_ConfigClk(ADC_CTRL3_CKMOD_AHB, RCC_ADCHCLK_DIV16);
    // 2. 配置ADC1的1MHz时钟源：选择HSI（内部高速时钟），并分频8倍（用于ADC校准和控制逻辑）
    RCC_ConfigAdc1mClk(RCC_ADC1MCLK_SRC_HSI, RCC_ADC1MCLK_DIV8);  //selsect HSI as RCC ADC1M CLK Source
}

/**
*\*\name    GPIO_Configuration_ADC.
*\*\fun     配置ADC采样对应的GPIO引脚（PB1设为模拟输入）
*\*\return  none
**/
void GPIO_Configuration_ADC(void)
{
    // GPIO初始化参数结构体（存储引脚模式、上下拉等配置）
    GPIO_InitType GPIO_InitStructure;
    /* -------------------------- 配置PB1为模拟输入 -------------------------- */
    // 1. 引脚选择：GPIO_PIN_1（即PB1引脚）
    GPIO_InitStructure.Pin       = GPIO_PIN_1;
    // 2. 引脚模式：GPIO_MODE_ANALOG（模拟输入模式，专门用于ADC采样）
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_ANALOG;
    // （注意：模拟输入模式下，不需要配置“上下拉电阻”和“输出速度”，因为引脚只接收模拟信号，不输出）
    
    // 3. 将配置应用到GPIOB端口（让PB1引脚生效为模拟输入）
    GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);
}

/**
*\*\name    DMA_Config.
*\*\fun     配置DMA（自动传输ADC1的转换结果到内存变量）
*\*\return  none
**/
void DMA_Config(void)
{
    // 1. 重置DMA1通道1（先恢复默认配置，避免之前的配置干扰）
    DMA_DeInit(DMA1_CH1);

    /* -------------------------- DMA 基础配置 -------------------------- */
    // 1. 外设地址：ADC1的DAT寄存器地址（ADC转换结果会存在这个寄存器里，是数据的“源头”）
    DMA_InitStructure.PeriphAddr     = (uint32_t)&ADC1->DAT;
    // 2. 内存地址：&ADCConvertedValue（要把数据搬运到这个变量里，是数据的“目的地”）
    DMA_InitStructure.MemAddr        = (uint32_t)&ADCConvertedValue;
    // 3. 传输方向：从外设到内存（ADC寄存器→变量，即“读ADC数据到内存”）
    DMA_InitStructure.Direction      = DMA_DIR_PERIPH_SRC;
    // 4. 缓冲区大小：1（每次只传输1个数据，因为ADC每次转换输出1个16位结果）
    DMA_InitStructure.BufSize        = 1;
    // 5. 外设地址递增：禁用（ADC的DAT寄存器地址固定，每次都从同一个地址读数据）
    DMA_InitStructure.PeriphInc      = DMA_PERIPH_INC_DISABLE;
    // 6. 内存地址递增：禁用（数据只存到ADCConvertedValue一个变量里，地址固定）
    DMA_InitStructure.MemoryInc      = DMA_MEM_INC_DISABLE;
    // 7. 外设数据宽度：半字（16位，ADC的DAT寄存器是16位的）
    DMA_InitStructure.PeriphDataSize = DMA_PERIPH_DATA_WIDTH_HALFWORD;
    // 8. 内存数据宽度：半字（16位，ADCConvertedValue是uint16_t类型，16位）
    DMA_InitStructure.MemDataSize    = DMA_MEM_DATA_WIDTH_HALFWORD;
    // 9. 传输模式：循环模式（传输1次后，自动重新开始传输，持续更新ADCConvertedValue）
    DMA_InitStructure.CircularMode   = DMA_MODE_CIRCULAR;
    // 10. 优先级：高优先级（确保ADC数据传输不被其他DMA任务打断）
    DMA_InitStructure.Priority       = DMA_PRIORITY_HIGH;
    // 11. 内存到内存：禁用（当前是“外设→内存”，不是“内存→内存”）
    DMA_InitStructure.Mem2Mem        = DMA_M2M_DISABLE;

    // 12. 将配置应用到DMA1通道1（ADC1的DMA请求默认映射到DMA1通道1，硬件固定）
    DMA_Init(DMA1_CH1, &DMA_InitStructure);
    
    // 13. DMA请求重映射：使能ADC1到DMA1通道1的映射（确保ADC产生的请求能被DMA1通道1响应）
    DMA_RequestRemap(DMA_REMAP_ADC1, DMA1_CH1, ENABLE);
    
    // 14. 使能DMA1通道1（启动DMA，开始自动搬运数据）
    DMA_EnableChannel(DMA1_CH1, ENABLE);
}


