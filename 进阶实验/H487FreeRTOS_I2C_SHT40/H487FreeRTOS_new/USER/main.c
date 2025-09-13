#include "main.h"

int main(void)
{   // �����ж����ȼ�����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
    DWT_INIT();
	LED_Configuration();
    
    USART_xSemapxMutex_Configuration();
	// RS485-1 PA4-TX PA5-RX
	USART1_Configuration();
	// RS485-2 PC4-TX PC5-RX
	UART7_Configuration();
	// RS485-3 PB0-TX PB1-RX
	USART4_Configuration();
	// RS485-4 PB2-TX PE7-RX
	UART6_Configuration();
    
    //������ʼ����
    xTaskCreate(
    (TaskFunction_t )Start_Task,    // ����������Ҫִ�еĴ�����Start_Task��
        (const char*    )"Start_Task",  // ������������ʱ���������ã������
        (uint16_t       )START_STK,     // ����ջ��С�������������ڴ棨��λ���֣�1��=4�ֽڣ�
        (void*          )NULL,          // �����񴫵Ĳ����������ò�������NULL
        (UBaseType_t    )START_PRIO,    // �������ȼ�������Խ�����ȼ�Խ�ߣ�������������ظ���
        (TaskHandle_t*  )&Start_Handler // �����������ڿ������񣨱���ɾ������
                );	
	//����FreeRTOS������������ؼ�������������ŻῪʼִ�У�        
    vTaskStartScheduler();
    while (1)
    {
        
    }
}

void Start_Task(void *pvParameters)
{
    while (1)
    {
        // �����ٽ�������������ʱ����ֹ���������ϣ���ֹ����
        taskENTER_CRITICAL();
        
        xTaskCreate((TaskFunction_t )Breath_Task,
                    (const char*    )"Breath_Task",
                    (uint16_t       )BREATH_STK,
                    (void*          )NULL,
                    (UBaseType_t    )BREATH_PRIO,
                    (TaskHandle_t*  )&Breath_Handler);
                    
        xTaskCreate((TaskFunction_t )Recv_Task,
                    (const char*    )"Recv_Task",
                    (uint16_t       )RECV_STK,
                    (void*          )NULL,
                    (UBaseType_t    )RECV_PRTO,
                    (TaskHandle_t*  )&Recv_Handler);
         // ����I2C����                       
        xTaskCreate((TaskFunction_t )I2C_Task,
                    (const char*    )"Recv_Task",
                    (uint16_t       )I2C_STK,
                    (void*          )NULL,
                    (UBaseType_t    )I2C_PRTO,
                    (TaskHandle_t*  )&I2C_Handler);
        vTaskDelete(Start_Handler);
        taskEXIT_CRITICAL();            //�˳��ٽ���       
    }
}
/*
����ʵ���õ���ģ������ʪ��STH40������ ����VCC GND  SDA  SCL ����ͨ��RS485����3����ͨ��
*/

void I2C_Task(void *pvParameters)
{    
    
      SI2C1_Configuration();  // I2C1��ʼ��������SCL=PA12��SDA=PA11��
    float temp = 0.0f, humi = 0.0f;
    
      // 2. ѭ����ȡ��ʪ�Ȳ���ӡ��ÿ��1���һ�Σ�
    while (1) {
        if (Read_TempHumi_I2C(&temp, &humi)) {
            // 3. ��USART_Print��ӡ��USART1����ʽ���¶ȱ���1λС����ʪ�ȱ���1λС����
            USART_Print(USART_LOG, "Temp: %.1f ��, Humi: %.1f %%RH\r\n", temp, humi);
        } else {
            USART_Print(USART_LOG, "Read TempHumi Error!\r\n");
        }

    }
    
}
// -------------------------- ���������񣺿���LED���� --------------------------
// ���ã�ÿ��500ms��תһ��LED״̬�������������...����֤��ϵͳ������
void Breath_Task(void *pvParameters)
{
    while (1)
    {
        LED(LED_DEBUG, CHANGE);
        
        vTaskDelay(500);
    }
}
// -------------------------- ���ڽ������񣺴��������� --------------------------
// ���ã��ȴ����ڽ��������ݣ�Ȼ����յ������ݴ�ӡ����
void Recv_Task(void *pvParameters)
{
    while (1)
    {
        if (xSemaphoreTake(xSemap[USART_ID_LOG], portMAX_DELAY) == pdTRUE)
        {
            USART_Printf(USART_LOG, "[%s]%s\r\n", g_USART_Name[USART_ID_LOG], g_USART_Buf[USART_ID_LOG]);
        }
    }
}

// -------------------------- FreeRTOS���Ӻ����������� --------------------------
// 1. �ڴ����ʧ�ܹ��ӣ���FreeRTOS�����ڴ�ʧ��ʱ������ջ����������ִ���������
void vApplicationMallocFailedHook(void)
{
    while (1)
    {
        USART_Print(USART_LOG, "Malloc Failed\r\n");
    }
}
// 2. ջ������ӣ���ĳ�������ջ�����ã�����ݹ����̫�ࣩ����ִ���������
void vApplicationStackOverflowHook(TaskHandle_t handle, char *para)
{
    while (1)
    {
        USART_Print(USART_LOG, "Stack Overflow\r\n");
    }
}

// 3. ���������ӣ���������������ʱ��FreeRTOS��ִ�������������
void vApplicationIdleHook(void)
{
    while (1)
    {
        
    }
}



