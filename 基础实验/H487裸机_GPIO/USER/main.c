#include "main.h"

/*
PE0 ����>LED_ERROR
PE1 ����>LED_RUN
*/

int main(void)
{
	LED_Configuration();
    while (1)
    {
  
//	LED(LED_RUN, ON); //����
//	LED(LED_ERROR, ON);
        
    LED(LED_RUN, OFF);//�ص�
	LED(LED_ERROR, OFF);
    }
}
