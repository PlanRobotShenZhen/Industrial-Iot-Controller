#include "main.h"
#include "usart.h"
#include "adc.h"
#include "string.h"
int main(void)
{
    USART1_init();
    
    ADC_init();
    //这里在ADC_init 中配置了DMA 此处是通过DMA转发串口来打印的
    USART_Print(USART1, " ADCConvertedValue= %d\r\n",  ADCConvertedValue); 

}
