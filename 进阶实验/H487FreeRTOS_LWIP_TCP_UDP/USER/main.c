#include "main.h"

int main(void)
{   // �����ж����ȼ�����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
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
// -------------------------- ��ʼ���񣺴��������������� --------------------------
// ���ã��ȴ����Լ��������������������ơ������ڽ��ա������硱����������Լ������ݡ�
void Start_Task(void *pvParameters)
{
    while (1)
    {
         // ���롰�ٽ���������������ʱ����ֹ���������ϣ���ֹ����
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
        //�����������ʼ�����񡿣���ʼ��LWIP����N32��������            
        xTaskCreate((TaskFunction_t )Net_Task,
                    (const char*    )"Net_Task",
                    (uint16_t       )NET_STK,
                    (void*          )NULL,
                    (UBaseType_t    )NET_PRTO,
                    (TaskHandle_t*  )&Net_Handler);
        //������TCP���񡿣���ΪTCP�ͻ��ˣ������������շ����ݣ�            
        xTaskCreate((TaskFunction_t )TCP_Task,
                    (const char*    )"TCP_Task",
                    (uint16_t       )TCP_STK,
                    (void*          )NULL,
                    (UBaseType_t    )TCP_PRTO,
                    (TaskHandle_t*  )&TCP_Handler);
        //������UDP���񡿣���ΪUDP�ͻ��ˣ��շ����ݣ�             
        xTaskCreate((TaskFunction_t )UDP_Task,
                    (const char*    )"UDP_Task",
                    (uint16_t       )UDP_STK,
                    (void*          )NULL,
                    (UBaseType_t    )UDP_PRTO,
                    (TaskHandle_t*  )&UDP_Handler);
        //��ͣTCP��UDP�����Ȳ��ܣ��������ʼ����ɺ���������            
        vTaskSuspend(TCP_Handler);
        vTaskSuspend(UDP_Handler);
        // ɾ������ʼ���񡿣����񶼴������ˣ��Լ�û���ˣ�ɾ��ʡ�ڴ棩            
        vTaskDelete(Start_Handler);
        taskEXIT_CRITICAL();            //�˳��ٽ���       
    }
}

// -------------------------- ������������LED������ --------------------------
// ���ã�ÿ��500���뷭תһ��LED״̬�������������...����֤��ϵͳ����������
void Breath_Task(void *pvParameters)
{
    while (1)
    {
        LED(LED_DEBUG, CHANGE);
        
        vTaskDelay(500);
    }
}
// -------------------------- ���ڽ������񣺴��������� --------------------------
// ���ã��ȴ����յ����ݺ󣬰����ݴ�ӡ����
void Recv_Task(void *pvParameters)
{
    while (1)
    {
        // �ȴ�����־���ڡ����ź�����
        // - �ź����ɴ����ж��ͷţ��������յ���������ʱ��
        // - portMAX_DELAY�����޵ȣ�ֱ���õ��ź��������ᳬʱ��
        if (xSemaphoreTake(xSemap[USART_ID_LOG], portMAX_DELAY) == pdTRUE)
        {
            LOG("[%s]%s\r\n", g_USART_Name[USART_ID_LOG], g_USART_Buf[USART_ID_LOG]);
        }
    }
}
// -------------------------- �����ʼ������ --------------------------
// ���ã���ʼ��LWIPЭ��ջ����ɺ�����TCP��UDP����
void Net_Task(void *pvParameters)
{
    USART_Print(USART_LOG, "��������\r\n");
    // ��ʼ�����磨�ؼ�����������IP������DHCP����ʼ����̫��Ӳ����
    NetworkInitialize();
    // �����ʼ����ɺ󣬻ָ���������֮ǰ��ͣ��TCP��UDP����
    vTaskResume(TCP_Handler);
    vTaskResume(UDP_Handler);
    vTaskDelete(Net_Handler);
}

void UDP_Task(void *pvParameters)
{
    int Sock = -1;              // UDP socket�������������UDPͨ�ţ�
    int Len = 0;                // �շ����ݵĳ���
    struct sockaddr_in RemoteAddr;  // Զ���豸��ַ��������Ե�IP�Ͷ˿ڣ�
    struct sockaddr_in LocalAddr;   // �����豸��ַ��STM32��IP�Ͷ˿ڣ�
    socklen_t AddrSize;         // ��ַ�ṹ��Ĵ�С
    
    // ���߱�������pvParametersû�á������⾯�棬���ֲ��ùܣ�
    LWIP_UNUSED_ARG(pvParameters);
    
    // �ѵ�ַ�ṹ����գ���ʼ�������������ݣ�
    memset(&RemoteAddr, 0, sizeof(RemoteAddr));
    memset(&LocalAddr, 0, sizeof(LocalAddr));
    
    // ���ñ��ص�ַ��Ϣ
    LocalAddr.sin_family = AF_INET;         // ��ַ���ͣ�IPv4
    LocalAddr.sin_addr.s_addr = INADDR_ANY; // ����IP���Զ���ȡ���������õľ�̬IP��
    LocalAddr.sin_port = htons(LOCAL_PORT); // ���ض˿ڣ�LOCAL_PORT�Ǻ궨�壨����8080��
    
    // ��ӡ���ض˿ڣ��������
    LOG("Local port: %d\n", LOCAL_PORT);
    
    // UDP������ѭ����һֱ�ܣ�
    while (1)
    {
        // 1. ����UDP socket��AF_INET=IPv4��SOCK_DGRAM=UDP���ͣ�
        Sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (Sock < 0)  // ����ʧ�ܣ������ڴ治����
        {
            LOG("socket error\n");  // ��ӡ������Ϣ
            vTaskDelay(1000);       // ��1��������
            continue;
        }
        
        // 2. ��socket�����ض˿ڣ���socket��STM32��8080�˿ڹ�����
        if (bind(Sock, (struct sockaddr *)&LocalAddr, sizeof(struct sockaddr)) != 0)
        {
            LOG("bind failed\n");   // ��ʧ�ܣ�����˿ڱ�ռ�ã�
            closesocket(Sock);      // �ر�socket��������Դй¶
            vTaskDelay(1000);       // ��1������
            continue;
        }
        
        // 3. ѭ�����պͻط����ݣ��󶨳ɹ���һֱ�����ݣ�
        while (1)
        {
            char RecvBuff[MEDIUM_BUFFER_LEN] = {0}; // ���յ������ݣ������С�Ǻ궨�壬����1024��
            char SendBuff[MEDIUM_BUFFER_LEN] = {0}; // ��Ҫ���͵�����
            
            // �������ݣ���RemoteAddr��Զ���豸���յ����ݣ�����RecvBuff��
            Len = recvfrom(Sock, RecvBuff, sizeof(RecvBuff), 0, 
                          (struct sockaddr*)&RemoteAddr, &AddrSize);
            if (Len < 0)  // ����ʧ��
            {
                LOG("recvfrom error\n");
            }
            else  // ���ճɹ�����ӡԶ���豸��Ϣ������
            {
                // ��ӡ�����ĸ�IP:�˿��յ����ݡ���inet_ntoaתIPΪ�ַ�����ntohsת�˿�Ϊ�����ֽ���
                LOG("Receive from %s:%d\n", inet_ntoa(RemoteAddr.sin_addr), ntohs(RemoteAddr.sin_port));
                
                // ����ط����ݣ����յ�������ǰ�ӡ�[you say]:��
                Len = snprintf(SendBuff, sizeof(SendBuff), "[you say]:%s", RecvBuff);
                
                // �ѹ���õ����ݷ��ظ�Զ���豸
                Len = sendto(Sock, SendBuff, Len, 0, 
                            (struct sockaddr*)&RemoteAddr, AddrSize);
                if (Len < 0)  // ����ʧ��
                {
                    LOG("sendto error\n");
                }
            }
        }
    }
}

// -------------------------- TCP������TCPЭ�������������շ����� --------------------------
// ���ã���ΪTCP�ͻ��ˣ���ָ���ķ���������������ϵ�TCP���ߣ����յ����ݺ�
void TCP_Task(void *pvParameters)
{
    int Sock = -1;              // TCP socket���
    int Len = 0;                // �շ����ݳ���
    ip4_addr_t IpAddr;          // ������IP��ַ�ṹ��
    struct sockaddr_in ServerAddr;  // ��������ַ��IP+�˿ڣ�
    struct sockaddr_in ClientAddr;  // ���ؿͻ��˵�ַ��STM32��IP+�˿ڣ�
    socklen_t AddrSize = sizeof(ClientAddr);  // ��ַ�ṹ���С
    
    // ���߱�������pvParametersû�á�
    LWIP_UNUSED_ARG(pvParameters);
    // ��յ�ַ�ṹ��
    memset(&ServerAddr, 0, sizeof(ServerAddr));
    memset(&ClientAddr, 0, sizeof(ClientAddr));
    
     // 1. ����Ҫ���ӵķ�����IP��REMOTE_IP_ADDR0~3�Ǻ궨�壬����192.168.1.100��
    IP4_ADDR(&IpAddr, REMOTE_IP_ADDR0, REMOTE_IP_ADDR1, REMOTE_IP_ADDR2, REMOTE_IP_ADDR3);
    LOG("Server IP: %d.%d.%d.%d\r\n", REMOTE_IP_ADDR0, REMOTE_IP_ADDR1, REMOTE_IP_ADDR2, REMOTE_IP_ADDR3);
    LOG("Server Port: %d\r\n", REMOTE_PORT);// ��ӡ�������˿ڣ�����8080��
    
     // 2. ���÷�������ַ��Ϣ
    ServerAddr.sin_family = AF_INET;         // IPv4����
    ServerAddr.sin_addr.s_addr = IpAddr.addr; // ������IP
    ServerAddr.sin_port = htons(REMOTE_PORT); // �������˿ڣ�htons��ת�������ֽ���
    
    while (1)
    {
        // 3. ����TCP socket��SOCK_STREAM=TCP���ͣ�
        Sock = socket(AF_INET, SOCK_STREAM, 0);
        if (Sock < 0)
        {
            LOG("socket error\r\n");
            vTaskDelay(1000);
            continue;
        }
        // 4. ���ӷ�������TCP���������ӣ�����ͨ�ţ�
        if (connect(Sock, (struct sockaddr *)&ServerAddr, sizeof(struct sockaddr)) != 0)
        {
            LOG("connect failed!\r\n");
            closesocket(Sock);
            vTaskDelay(1000);
            continue;
        }
        // 5. ��ȡ���ؿͻ��˵�ַ������ӡ
        getsockname(Sock, (struct sockaddr *)&ClientAddr, &AddrSize);
        LOG("The server is connected from local %s:%d\r\n", inet_ntoa(ClientAddr.sin_addr), ntohs(ClientAddr.sin_port));
        // 6. ���ӳɹ���ѭ���շ�����
        while (1)
        {
            char RecvBuff[MEDIUM_BUFFER_LEN] = {0};
            char SendBuff[MEDIUM_BUFFER_LEN] = {0};
            // ���շ���������������
            Len = recv(Sock, RecvBuff, sizeof(RecvBuff), 0);
            if (Len > 0)
            {
                Len = snprintf(SendBuff, sizeof(SendBuff), "[you say]:%s", RecvBuff);
                //�������ݸ�������
                Len = send(Sock, SendBuff, Len, 0);
                if (Len <= 0)
                {
                    LOG("send error\r\n");
                }
            }
            else if (Len == 0)
            {
                LOG("Server disconnected\r\n");
                closesocket(Sock);
                vTaskDelay(1000);
                break;
            }
            else
            {
                LOG("recv error\r\n");
            }
        }
    }
}
// -------------------------- FreeRTOS���Ӻ����������� --------------------------
// 1. �ڴ����ʧ�ܹ��ӣ���FreeRTOS�����ڴ�ʧ��ʱ������ջ����������ִ���������
void vApplicationMallocFailedHook(void)
{
    while (1)
    {
        LOG("Malloc Failed\r\n");
    }
}
// 2. ջ������ӣ���ĳ�������ջ�����ã�����ݹ����̫�ࣩ����ִ���������
void vApplicationStackOverflowHook(TaskHandle_t handle, char *para)
{
    while (1)
    {
        LOG("Stack Overflow\r\n");
    }
}

// 3. ���������ӣ���������������ʱ��FreeRTOS��ִ�������������
void vApplicationIdleHook(void)
{
    while (1)
    {
        
    }
}



