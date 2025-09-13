#ifndef __USERDEF_H__
#define __USERDEF_H__

#include <stddef.h>
#include "n32h47x_48x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "usart.h"

#define USART_LOG       USART4
#define USART_ID_LOG    USART4_ID

extern SemaphoreHandle_t xSemap[5];// 串口信号量
extern SemaphoreHandle_t xMutex[5];// 串口互斥锁
extern USART_Use_t USART_Use_Type[4];

extern const char* const g_USART_Name[5];
extern const char* const g_USART_Buf[5];

#define REMOTE_PORT                 ((uint16_t)7787U)
#define LOCAL_PORT                  ((uint16_t)7687U)
#define REMOTE_IP_ADDR0             ((uint8_t)192U)
#define REMOTE_IP_ADDR1             ((uint8_t)168U)
#define REMOTE_IP_ADDR2             ((uint8_t)0U)
#define REMOTE_IP_ADDR3             ((uint8_t)101U)
/* Static IP ADDRESS: IP_ADDR0.IP_ADDR1.IP_ADDR2.IP_ADDR3 */
#define IP_ADDR0                    ((uint8_t)192U)
#define IP_ADDR1                    ((uint8_t)168U)
#define IP_ADDR2                    ((uint8_t)0U)
#define IP_ADDR3                    ((uint8_t)100U)
/* NETMASK: NETMASK_ADDR0.NETMASK_ADDR1.NETMASK_ADDR2.NETMASK_ADDR3 */
#define NETMASK_ADDR0               ((uint8_t)255U)
#define NETMASK_ADDR1               ((uint8_t)255U)
#define NETMASK_ADDR2               ((uint8_t)255U)
#define NETMASK_ADDR3               ((uint8_t)0U)
/* Gateway Address: GW_ADDR0.GW_ADDR1.GW_ADDR2.GW_ADDR3 */
#define GW_ADDR0                    ((uint8_t)192U)
#define GW_ADDR1                    ((uint8_t)168U)
#define GW_ADDR2                    ((uint8_t)0U)
#define GW_ADDR3                    ((uint8_t)1U)

#define SMALL_BUFFER_LEN    (64)
#define MEDIUM_BUFFER_LEN   (128)
#define LARGE_BUFFER_LEN    (512)

#endif

