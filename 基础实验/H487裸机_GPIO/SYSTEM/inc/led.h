#ifndef __LED_H__
#define __LED_H__

#include "n32h47x_48x.h"

typedef enum{LED_ERROR = 0, LED_RUN = 1, LED_DEBUG = 2}e_LED;
typedef enum{OFF = 0, ON = 1, CHANGE = 3}e_BitCmd;

void LED_Configuration(void);
void LED(e_LED x, e_BitCmd BitCmd);

#endif
