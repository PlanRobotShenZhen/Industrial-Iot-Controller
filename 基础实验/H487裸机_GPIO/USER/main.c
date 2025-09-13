#include "main.h"

/*
PE0 ¡ª¡ª>LED_ERROR
PE1 ¡ª¡ª>LED_RUN
*/

int main(void)
{
	LED_Configuration();
    while (1)
    {
  
//	LED(LED_RUN, ON); //¿ªµÆ
//	LED(LED_ERROR, ON);
        
    LED(LED_RUN, OFF);//¹ØµÆ
	LED(LED_ERROR, OFF);
    }
}
