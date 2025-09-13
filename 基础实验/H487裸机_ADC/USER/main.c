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
        // ��ʽ1����ѯADCת����ɱ�־��EOC��
        if (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC_ANY) == SET)  // EOC=ת�����
        {
            // �ֶ���ȡADC���ݼĴ�����ֵ������ADCConvertedValue
            ADCConvertedValue = ADC_GetDat(ADC1);  // ���ģ��ֶ������� ��û��ʹ��DMA
            USART_Print(USART1, "ADCֵ����DMA����%d\r\n", ADCConvertedValue);
            ADC_ClearFlag(ADC1, ADC_FLAG_EOC_ANY);  // �����־��׼���´�ת��
        }
    }

}
