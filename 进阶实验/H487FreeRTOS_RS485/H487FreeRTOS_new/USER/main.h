#ifndef __MAIN_H__
#define __MAIN_H__

#include "n32h47x_48x.h"
#include "init.h"
#include "usart.h"
#include "led.h"
#include "adc.h"
#include "CRC.h"
#include "userdef.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

// 任务优先级
/*****************************************************************/
#define START_STK       128
#define BREATH_STK      128
#define RECV_STK		128
/*****************************************************************/
// 任务栈空间
/*****************************************************************/
#define START_PRIO      5
#define BREATH_PRIO     1
#define RECV_PRTO		1
/*****************************************************************/
// 任务句柄
/*****************************************************************/
TaskHandle_t Start_Handler;
TaskHandle_t Breath_Handler;
TaskHandle_t Recv_Handler;
/*****************************************************************/
// 任务声明
/*****************************************************************/
void Start_Task(void *pvParameters);
void Breath_Task(void *pvParameters);
void Recv_Task(void *pvParameters);
/*****************************************************************/
#endif
