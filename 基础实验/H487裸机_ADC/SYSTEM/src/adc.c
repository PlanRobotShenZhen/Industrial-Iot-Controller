
#include "adc.h"

// 16λADCת������洢������__IO���Σ���ʾ����ʧ�ԡ�����ֹ�������Ż�����
// ADC��12λ�ֱ��ʣ����Խ����Χ��0~4095��2^12=4096��
__IO uint16_t ADCConvertedValue;

// ADC��ʼ�������ṹ�壨�洢ADC�Ĺ���ģʽ���ֱ��ʵ����ã�
ADC_InitType ADC_InitStructure;
// DMA��ʼ�������ṹ�壨�洢DMA�Ĵ��䷽�򡢻�������С�����ã�
DMA_InitType DMA_InitStructure;


// ������������ǰ���߱�������Щ�������ں��涨�壨������뱨��
void RCC_Configuration_ADC(void);  // ����ADC��ص�ʱ��
void GPIO_Configuration_ADC(void);  // ����ADC��Ӧ��GPIO����
void DMA_Config(void);              // ����DMA�������Զ�����ADC���ݣ�


/**
*\*\name    ADC_Initial.
*\*\fun     ADC���ĳ�ʼ������������ADC�Ĺ���������
*\*\return  none
**/
void ADC_Initial(void)
{
        /* -------------------------- ADC1 �������� -------------------------- */
    // 1. ����ģʽ������ģʽ��ADC1������������������ADCЭͬ��
    ADC_InitStructure.WorkMode       = ADC_WORKMODE_INDEPENDENT;
    // 2. ��ͨ��ʹ�ܣ����ã���ǰֻ����1��ͨ��������Ҫ��ͨ���л���
    ADC_InitStructure.MultiChEn      = DISABLE;
    // 3. ����ת��ʹ�ܣ�ʹ�ܣ�ADC����һ�κ��Զ���ʼ��һ�β���������������ݣ�
    ADC_InitStructure.ContinueConvEn = ENABLE;
    // 4. �ⲿ����ѡ�����������ͨ����������ADCת���������ⲿ���Ŵ�����
    ADC_InitStructure.ExtTrigSelect  = ADC_EXT_TRIG_REG_CONV_SOFTWARE;
    // 5. ���ݶ��룺�Ҷ��루ADC��12λ���ݣ��Ҷ���ʱ������ĵ�12λ��Ч����4λ��0��������㣩
    ADC_InitStructure.DatAlign       = ADC_DAT_ALIGN_R;
    // 6. ͨ��������1������ǰֻ����PB1���Ŷ�Ӧ��ͨ����
    ADC_InitStructure.ChsNumber      = 1;
    // 7. �ֱ��ʣ�12λ��ADC�����ֵ���С��ѹ�ǡ��ο���ѹ/4095��������3.3V�ο���ѹʱ����С�ֱ��ʡ�0.8mV��
    ADC_InitStructure.Resolution     = ADC_DATA_RES_12BIT;
    // 8. ����������Ӧ�õ�ADC1�����õײ㺯������������Ч��
    ADC_Init(ADC1, &ADC_InitStructure);
    
    
        /* -------------------------- ADC1 ����ͨ������ -------------------------- */
    // ����ADC1�ġ��������ͨ������
    // ����1��ADC1��Ҫ���õ�ADCģ�飩
    // ����2��ADC1_Channel_12_PB1������ͨ����PB1���Ŷ�ӦADC1�ĵ�12ͨ����Ӳ���̶���
    // ����3��1��ͨ�����ȼ�����ͨ��ʱ�����ã���ǰ��ͨ����Ϊ1���ɣ�
    // ����4��ADC_SAMP_TIME_CYCLES_13_5������ʱ�䣺13.5��ADCʱ�����ڣ�����ʱ��Խ��������Խ�ȶ���
    ADC_ConfigRegularChannel(ADC1, ADC1_Channel_12_PB1, 1, ADC_SAMP_TIME_CYCLES_13_5);
    
       /* -------------------------- ����ADC��У׼ -------------------------- */
    // 1. ʹ��ADC1����ADC�ϵ磬׼��������
    ADC_Enable(ADC1, ENABLE);
    // 2. �ȴ�ADC1׼��������ADC�ϵ����Ҫһ��ʱ���ʼ����ֱ��������־��1��
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_RDY) == RESET)
        ;  // û������ѭ���ȴ���ֱ������
    // 3. ����ADC1У׼��ADC������΢С��У׼������߲������ȣ�
    ADC_CalibrationOperation(ADC1, ADC_CALIBRATION_SINGLE_MODE);
    // 4. �ȴ�У׼��ɣ�У׼������Ҫʱ�䣬ֱ��У׼״̬��־��0��
    while (ADC_GetCalibrationStatus(ADC1, ADC_CALIBRATION_SINGLE_MODE))
        ;  // û��ɾ�ѭ���ȴ���ֱ�����
     // 5. ����ADC1���ת�����ֶ�������һ�β���������������ת��ʹ�ܡ����Զ�������
    ADC_EnableSoftwareStartConv(ADC1, ENABLE);

}
/**
*\*\name    ADC_init.
*\*\fun     ADC������ʼ����ڣ�����ʱ�ӡ�GPIO��DMA��ADC�������ã�
*\*\return  none
**/
void ADC_init(void)
{
    /* 1. ����ADC��ص�ʱ�ӣ���ADC��GPIO��DMAģ���ϵ磩 */
    RCC_Configuration_ADC();

    /* 2. ����ADC��Ӧ��GPIO���ţ���PB1��Ϊģ������ģʽ�� */
    GPIO_Configuration_ADC();

    /* 3. ����DMA�������Զ�����ADCת�����������CPUƵ����ȡ�� */
    DMA_Config();
    
    /* 4. ����ADC���Ĳ���������ģʽ������ͨ���ȣ� */
    ADC_Initial();
    
    /* 5. �ٴ�����ADC���ת����ȷ����������������ʼ�� */
    ADC_EnableSoftwareStartConv(ADC1, ENABLE);
}

/**
*\*\name    RCC_Configuration_ADC.
*\*\fun     ����ADC��ص�ʱ�ӣ�ʹ��ADC��GPIO��DMA��ʱ�ӣ�
*\*\return  none
**/
void RCC_Configuration_ADC(void)
{
   /* -------------------------- ʹ������ʱ�� -------------------------- */
    // 1. ʹ��DMA1ʱ�ӣ�DMA1���ڴ���ADC���ݣ���Ҫ�ϵ磩
    RCC_EnableAHBPeriphClk(RCC_AHB_PERIPHEN_DMA1, ENABLE);

    // 2. ʹ��GPIOA��GPIOB��GPIOCʱ�ӣ���ǰ��GPIOB��PB1���ţ�����GPIO������չ��
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOA | RCC_AHB_PERIPHEN_GPIOB | RCC_AHB_PERIPHEN_GPIOC, ENABLE);
    
    // 3. ʹ��ADC1ʱ�ӣ�ADC1ģ���ϵ磬���ܹ�����
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_ADC1, ENABLE);

    /* -------------------------- ����ADCʱ��Ƶ�� -------------------------- */
    // 1. ADCʱ��Դѡ��AHBʱ�ӣ�����Ƶ16����ȷ��ADCʱ��Ƶ��������Χ�������������
    ADC_ConfigClk(ADC_CTRL3_CKMOD_AHB, RCC_ADCHCLK_DIV16);
    // 2. ����ADC1��1MHzʱ��Դ��ѡ��HSI���ڲ�����ʱ�ӣ�������Ƶ8��������ADCУ׼�Ϳ����߼���
    RCC_ConfigAdc1mClk(RCC_ADC1MCLK_SRC_HSI, RCC_ADC1MCLK_DIV8);  //selsect HSI as RCC ADC1M CLK Source
}

/**
*\*\name    GPIO_Configuration_ADC.
*\*\fun     ����ADC������Ӧ��GPIO���ţ�PB1��Ϊģ�����룩
*\*\return  none
**/
void GPIO_Configuration_ADC(void)
{
    // GPIO��ʼ�������ṹ�壨�洢����ģʽ�������������ã�
    GPIO_InitType GPIO_InitStructure;
    /* -------------------------- ����PB1Ϊģ������ -------------------------- */
    // 1. ����ѡ��GPIO_PIN_1����PB1���ţ�
    GPIO_InitStructure.Pin       = GPIO_PIN_1;
    // 2. ����ģʽ��GPIO_MODE_ANALOG��ģ������ģʽ��ר������ADC������
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_ANALOG;
    // ��ע�⣺ģ������ģʽ�£�����Ҫ���á����������衱�͡�����ٶȡ�����Ϊ����ֻ����ģ���źţ��������
    
    // 3. ������Ӧ�õ�GPIOB�˿ڣ���PB1������ЧΪģ�����룩
    GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);
}

/**
*\*\name    DMA_Config.
*\*\fun     ����DMA���Զ�����ADC1��ת��������ڴ������
*\*\return  none
**/
void DMA_Config(void)
{
    // 1. ����DMA1ͨ��1���Ȼָ�Ĭ�����ã�����֮ǰ�����ø��ţ�
    DMA_DeInit(DMA1_CH1);

    /* -------------------------- DMA �������� -------------------------- */
    // 1. �����ַ��ADC1��DAT�Ĵ�����ַ��ADCת��������������Ĵ���������ݵġ�Դͷ����
    DMA_InitStructure.PeriphAddr     = (uint32_t)&ADC1->DAT;
    // 2. �ڴ��ַ��&ADCConvertedValue��Ҫ�����ݰ��˵��������������ݵġ�Ŀ�ĵء���
    DMA_InitStructure.MemAddr        = (uint32_t)&ADCConvertedValue;
    // 3. ���䷽�򣺴����赽�ڴ棨ADC�Ĵ�����������������ADC���ݵ��ڴ桱��
    DMA_InitStructure.Direction      = DMA_DIR_PERIPH_SRC;
    // 4. ��������С��1��ÿ��ֻ����1�����ݣ���ΪADCÿ��ת�����1��16λ�����
    DMA_InitStructure.BufSize        = 1;
    // 5. �����ַ���������ã�ADC��DAT�Ĵ�����ַ�̶���ÿ�ζ���ͬһ����ַ�����ݣ�
    DMA_InitStructure.PeriphInc      = DMA_PERIPH_INC_DISABLE;
    // 6. �ڴ��ַ���������ã�����ֻ�浽ADCConvertedValueһ���������ַ�̶���
    DMA_InitStructure.MemoryInc      = DMA_MEM_INC_DISABLE;
    // 7. �������ݿ�ȣ����֣�16λ��ADC��DAT�Ĵ�����16λ�ģ�
    DMA_InitStructure.PeriphDataSize = DMA_PERIPH_DATA_WIDTH_HALFWORD;
    // 8. �ڴ����ݿ�ȣ����֣�16λ��ADCConvertedValue��uint16_t���ͣ�16λ��
    DMA_InitStructure.MemDataSize    = DMA_MEM_DATA_WIDTH_HALFWORD;
    // 9. ����ģʽ��ѭ��ģʽ������1�κ��Զ����¿�ʼ���䣬��������ADCConvertedValue��
    DMA_InitStructure.CircularMode   = DMA_MODE_CIRCULAR;
    // 10. ���ȼ��������ȼ���ȷ��ADC���ݴ��䲻������DMA�����ϣ�
    DMA_InitStructure.Priority       = DMA_PRIORITY_HIGH;
    // 11. �ڴ浽�ڴ棺���ã���ǰ�ǡ�������ڴ桱�����ǡ��ڴ���ڴ桱��
    DMA_InitStructure.Mem2Mem        = DMA_M2M_DISABLE;

    // 12. ������Ӧ�õ�DMA1ͨ��1��ADC1��DMA����Ĭ��ӳ�䵽DMA1ͨ��1��Ӳ���̶���
    DMA_Init(DMA1_CH1, &DMA_InitStructure);
    
    // 13. DMA������ӳ�䣺ʹ��ADC1��DMA1ͨ��1��ӳ�䣨ȷ��ADC�����������ܱ�DMA1ͨ��1��Ӧ��
    DMA_RequestRemap(DMA_REMAP_ADC1, DMA1_CH1, ENABLE);
    
    // 14. ʹ��DMA1ͨ��1������DMA����ʼ�Զ��������ݣ�
    DMA_EnableChannel(DMA1_CH1, ENABLE);
}


