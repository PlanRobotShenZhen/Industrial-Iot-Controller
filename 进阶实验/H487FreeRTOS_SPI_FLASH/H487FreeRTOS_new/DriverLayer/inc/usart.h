#ifndef __USART_H__
#define __USART_H__

#include "n32h47x_48x.h"
#include <stddef.h>

typedef enum{SendMode, RecvMode}e_RS485_Mode;

typedef struct
{
	USART_Module*	    USART_M;
	char*		        RXBufAddr;
	size_t		        BufSize;
    volatile size_t	    Len;
}USART_Use_t;

typedef enum{USART1_ID = 0, UART7_ID = 1, USART4_ID = 2, UART6_ID = 3, USART2_ID = 4}e_USART_ID;

void USART_xSemapxMutex_Configuration(void);
void USART1_Configuration(void);
void UART7_Configuration(void);
void USART4_Configuration(void);
void UART6_Configuration(void);
void RS485_Mode(USART_Module* USARTx, e_RS485_Mode Mode);

void USART_SendStr(USART_Module* USARTx, const char* Str, size_t Len);
void USART_Print(USART_Module* USARTx, const char* Format, ...);
void USART_Printf(USART_Module* USARTx, const char* Format, ...);

#endif
