#ifndef __MAIN_H__
#define __MAIN_H__

#define LOG(...) USART_Printf(USART_LOG, __VA_ARGS__)


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
#include "spi.h"

// �������ȼ�
/*****************************************************************/
#define START_STK       128
#define BREATH_STK      128
#define RECV_STK		128
#define W25Q64_SPI_STK		128
/*****************************************************************/
// ����ջ�ռ�
/*****************************************************************/
#define START_PRIO      5
#define BREATH_PRIO     1
#define RECV_PRTO		1
#define W25Q64_SPI_PRTO		1
/*****************************************************************/
// ������
/*****************************************************************/
TaskHandle_t Start_Handler;
TaskHandle_t Breath_Handler;
TaskHandle_t Recv_Handler;
TaskHandle_t W25Q64_SPI_Handler;
/*****************************************************************/
// ��������
/*****************************************************************/
void Start_Task(void *pvParameters);
void Breath_Task(void *pvParameters);
void Recv_Task(void *pvParameters);
void W25Q64_SPI_TASK(void *pvParameters);
/*****************************************************************/
#endif
