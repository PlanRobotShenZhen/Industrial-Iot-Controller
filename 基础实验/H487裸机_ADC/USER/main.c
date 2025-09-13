#include "main.h"
#include "usart.h"
#include "adc.h"
#include "string.h"
int main(void)
{
    USART1_init();
    ADC_init();
    while (1)
    {
        // 方式1：查询ADC转换完成标志（EOC）
        if (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC_ANY) == SET)  // EOC=转换完成
        {
            // 手动读取ADC数据寄存器的值，存入ADCConvertedValue
            ADCConvertedValue = ADC_GetDat(ADC1);  // 核心：手动读数据 并没有使用DMA
            USART_Print(USART1, "ADC值（无DMA）：%d\r\n", ADCConvertedValue);
            ADC_ClearFlag(ADC1, ADC_FLAG_EOC_ANY);  // 清除标志，准备下次转换
        }
    }

}
