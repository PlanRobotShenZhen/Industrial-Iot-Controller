#ifndef __MAIN_H__
#define __MAIN_H__

#include "n32h47x_48x.h"
#include "easyinit.h"
#include "usart.h"
#include "led.h"
#include "adc.h"
#include "CRC.h"
#include "userdef.h"
#include "eth.h"

#include <lwip/opt.h>
#include <lwip/arch.h>
#include <lwip/sockets.h>
#include "tcpip.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "ethernetif.h"
#include "netif/ethernet.h"
#include "lwip/def.h"
#include "lwip/stats.h"
#include "lwip/etharp.h"
#include "lwip/ip.h"
#include "lwip/timeouts.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

// �������ȼ�
/*****************************************************************/
#define START_STK       128
#define BREATH_STK      128
#define RECV_STK		128
#define NET_STK		    256
#define TCP_STK		    1024
#define UDP_STK		    1024
/*****************************************************************/
// ����ջ�ռ�
/*****************************************************************/
#define START_PRIO      5
#define BREATH_PRIO     1
#define RECV_PRTO		1
#define NET_PRTO		1
#define TCP_PRTO		1
#define UDP_PRTO		1
/*****************************************************************/
// ������
/*****************************************************************/
TaskHandle_t Start_Handler;
TaskHandle_t Breath_Handler;
TaskHandle_t Recv_Handler;
TaskHandle_t Net_Handler;
TaskHandle_t TCP_Handler;
TaskHandle_t UDP_Handler;
/*****************************************************************/
// ��������
/*****************************************************************/
void Start_Task(void *pvParameters);
void Breath_Task(void *pvParameters);
void Recv_Task(void *pvParameters);
void Net_Task(void *pvParameters);
void TCP_Task(void *pvParameters);
void UDP_Task(void *pvParameters);
/*****************************************************************/
#endif



