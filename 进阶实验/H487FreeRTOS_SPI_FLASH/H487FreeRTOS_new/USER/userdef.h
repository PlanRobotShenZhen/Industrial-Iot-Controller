#ifndef __USERDEF_H__
#define __USERDEF_H__

#include <stddef.h>
#include "n32h47x_48x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

#define USART_LOG       USART4
#define USART_ID_LOG    USART4_ID

extern SemaphoreHandle_t xSemap[5];// 串口信号量
extern SemaphoreHandle_t xMutex[5];// 串口互斥锁
extern USART_Use_t USART_Use_Type[4];

extern const char* const g_USART_Name[5];
extern const char* const g_USART_Buf[5];

#endif

