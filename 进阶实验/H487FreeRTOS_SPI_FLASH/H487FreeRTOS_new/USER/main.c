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
	
    USART_Print(USART_LOG, "CIALLO\r\n");
    
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
        // ������Flash��д���񡿣�����W25Q64оƬ�������ݡ������ݣ�
        xTaskCreate((TaskFunction_t )W25Q64_SPI_TASK,
                    (const char*    )"W25Q64_SPI_TASK",
                    (uint16_t       )W25Q64_SPI_STK,
                    (void*          )NULL,
                    (UBaseType_t    )W25Q64_SPI_PRTO,
                    (TaskHandle_t*  )&W25Q64_SPI_Handler);
        vTaskDelete(Start_Handler);
        taskEXIT_CRITICAL();            //�˳��ٽ���       
    }
}

// -------------------------- ���������񣺿���LED���� --------------------------
// ���ã�ÿ��500ms��תһ��LED״̬�������������...����֤��ϵͳ������
void Breath_Task(void *pvParameters)
{
    while (1)
    {
        LED(LED_DEBUG, CHANGE);
        
        vTaskDelay(500);    // FreeRTOS��ʱ����������������500ms���ڼ�CPU��ȥ����������
    }
}
// -------------------------- ���ڽ������񣺴��������� --------------------------
// ���ã��ȴ����ڽ��������ݣ�Ȼ����յ������ݴ�ӡ����
void Recv_Task(void *pvParameters)
{
    while (1)
    {
        // �ȴ�����־���ڣ�USART_ID_LOG�������ź�����
        // - �ź����ɴ����ж��ͷţ��������յ���������ʱ��
        // - portMAX_DELAY�����޵ȴ���ֱ���õ��ź��������ᳬʱ��
        if (xSemaphoreTake(xSemap[USART_ID_LOG], portMAX_DELAY) == pdTRUE)
        {
            USART_Printf(USART_LOG, "[%s]%s\r\n", g_USART_Name[USART_ID_LOG], g_USART_Buf[USART_ID_LOG]);
        }
    }
}
// -------------------------- Flash��д���񣺲���W25Q64оƬ --------------------------
// ���ã�����W25Q64 Flash��ͨ�źͶ�д���ܣ��桰hello flash�����ٶ�������֤��
void W25Q64_SPI_TASK(void *pvParameters)
{
// ���������Դ���ʾ��
    uint8_t mfg_id;
    uint16_t dev_id;
    W25Q64_SPI_Init();
    const char *write_str = "hello flash";  // Ҫд����ַ���
    uint16_t str_len = strlen(write_str);   // �ַ������ȣ�������ֹ��'\0'��
    char read_str[32] = {0};                // �洢��ȡ���ַ���

    // 1. ��֤Flashͨ��
    W25Q64_ReadID(&mfg_id, &dev_id);
    LOG("1. Flash ID ��֤��\n");
    LOG("   MFG ID: 0x%02X, Dev ID: 0x%04X\n", mfg_id, dev_id);
    if (mfg_id != 0xEF || dev_id != 0x4017) {
        LOG("   ����Flashͨ��ʧ�ܣ�ֹͣ���ԣ�\n");
        return;
    }

    // 2. ����Ŀ��������ȷ��д��ɹ�����ַ0x000000��
    LOG("\n2. ������ַ0x000000��������...\n");
    W25Q64_SectorErase(0x000000);
    LOG("   �����������\n");

    // 3. ��ӡҪд����ַ���
    LOG("\n3. ׼��д����ַ�����\n");
    LOG("   ���ݣ�\"%s\"\n", write_str);
    LOG("   ���ȣ�%d�ֽڣ�����ֹ����\n", str_len);
    LOG("   ʮ�����ƣ�");
    for (uint16_t i = 0; i < str_len; i++) {
        LOG("0x%02X ", write_str[i]);
    }
    LOG("\n");

    // 4. д���ַ�����Flash
    LOG("\n4. ���ַ0x000000д������...\n");
    W25Q64_PageProgram(0x000000, (uint8_t*)write_str, str_len);
    LOG("   д�����\n");

    // 5. ��Flash��ȡ�ַ���
    LOG("\n5. �ӵ�ַ0x000000��ȡ����...\n");
    W25Q64_ReadData(0x000000, (uint8_t*)read_str, str_len);
    LOG("   ��ȡ���\n");

    // 6. ��ӡ��ȡ���ַ���
    LOG("\n6. ��ȡ�����ַ�����\n");
    LOG("   ���ݣ�\"%s\"\n", read_str);
    LOG("   ���ȣ�%d�ֽڣ�����ֹ����\n", strlen(read_str));
    LOG("   ʮ�����ƣ�");
    for (uint16_t i = 0; i < str_len; i++) {
        LOG("0x%02X ", read_str[i]);
    }
    LOG("\n");

    // 7. ��֤д�����ȡ�Ƿ�һ��
    if (strcmp(write_str, read_str) == 0) {
        LOG("\n7. ��֤��������ɹ���д�����ȡ��ȫһ�£�\n");
    } else {
        LOG("\n7. ��֤�������ʧ�ܡ����ݲ�ƥ�䣡\n");
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



