#include "main.h"
#include "usart.h"
#include "adc.h"
#include "string.h"
int main(void)
{
    USART1_init();
    
    ADC_init();
    //������ADC_init ��������DMA �˴���ͨ��DMAת����������ӡ��
    USART_Print(USART1, " ADCConvertedValue= %d\r\n",  ADCConvertedValue); 

}
