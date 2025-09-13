#include "main.h"

int main(void)
{   // 配置中断优先级分组
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
    
    //创建开始任务
    xTaskCreate(
     (TaskFunction_t )Start_Task,    // 任务函数名：要执行的代码在Start_Task里
        (const char*    )"Start_Task",  // 任务名：调试时区分任务用（随便起）
        (uint16_t       )START_STK,     // 任务栈大小：给任务分配的内存（单位：字，1字=4字节）
        (void*          )NULL,          // 给任务传的参数：这里用不到，填NULL
        (UBaseType_t    )START_PRIO,    // 任务优先级：数字越大，优先级越高（别和其他任务重复）
        (TaskHandle_t*  )&Start_Handler // 任务句柄：用于控制任务（比如删除任务）
                );
    //启动FreeRTOS任务调度器（关键！启动后，任务才会开始执行）   
    vTaskStartScheduler();
    while (1)
    {
        
    }
}
// -------------------------- 开始任务：创建其他所有任务 --------------------------
// 作用：先创建自己，再批量创建“呼吸灯”“串口接收”“网络”等任务，最后自己“退休”
void Start_Task(void *pvParameters)
{
    while (1)
    {
         // 进入“临界区”：创建任务时，禁止其他任务打断（防止出错）
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
        //创建【网络初始化任务】（初始化LWIP，让N32能连网）            
        xTaskCreate((TaskFunction_t )Net_Task,
                    (const char*    )"Net_Task",
                    (uint16_t       )NET_STK,
                    (void*          )NULL,
                    (UBaseType_t    )NET_PRTO,
                    (TaskHandle_t*  )&Net_Handler);
        //创建【TCP任务】（作为TCP客户端，连服务器、收发数据）            
        xTaskCreate((TaskFunction_t )TCP_Task,
                    (const char*    )"TCP_Task",
                    (uint16_t       )TCP_STK,
                    (void*          )NULL,
                    (UBaseType_t    )TCP_PRTO,
                    (TaskHandle_t*  )&TCP_Handler);
        //创建【UDP任务】（作为UDP客户端，收发数据）             
        xTaskCreate((TaskFunction_t )UDP_Task,
                    (const char*    )"UDP_Task",
                    (uint16_t       )UDP_STK,
                    (void*          )NULL,
                    (UBaseType_t    )UDP_PRTO,
                    (TaskHandle_t*  )&UDP_Handler);
        //暂停TCP和UDP任务（先不跑，等网络初始化完成后再启动）            
        vTaskSuspend(TCP_Handler);
        vTaskSuspend(UDP_Handler);
        // 删除【开始任务】（任务都创建完了，自己没用了，删掉省内存）            
        vTaskDelete(Start_Handler);
        taskEXIT_CRITICAL();            //退出临界区       
    }
}

// -------------------------- 呼吸灯任务：让LED闪起来 --------------------------
// 作用：每隔500毫秒翻转一次LED状态（亮→灭→亮→...），证明系统在正常运行
void Breath_Task(void *pvParameters)
{
    while (1)
    {
        LED(LED_DEBUG, CHANGE);
        
        vTaskDelay(500);
    }
}
// -------------------------- 串口接收任务：处理串口数据 --------------------------
// 作用：等串口收到数据后，把数据打印出来
void Recv_Task(void *pvParameters)
{
    while (1)
    {
        // 等待“日志串口”的信号量：
        // - 信号量由串口中断释放（当串口收到完整数据时）
        // - portMAX_DELAY：无限等，直到拿到信号量（不会超时）
        if (xSemaphoreTake(xSemap[USART_ID_LOG], portMAX_DELAY) == pdTRUE)
        {
            LOG("[%s]%s\r\n", g_USART_Name[USART_ID_LOG], g_USART_Buf[USART_ID_LOG]);
        }
    }
}
// -------------------------- 网络初始化任务 --------------------------
// 作用：初始化LWIP协议栈，完成后启动TCP和UDP任务
void Net_Task(void *pvParameters)
{
    USART_Print(USART_LOG, "网络任务\r\n");
    // 初始化网络（关键！比如配置IP、启动DHCP、初始化以太网硬件）
    NetworkInitialize();
    // 网络初始化完成后，恢复（启动）之前暂停的TCP和UDP任务
    vTaskResume(TCP_Handler);
    vTaskResume(UDP_Handler);
    vTaskDelete(Net_Handler);
}

void UDP_Task(void *pvParameters)
{
    int Sock = -1;              // UDP socket句柄（用来操作UDP通信）
    int Len = 0;                // 收发数据的长度
    struct sockaddr_in RemoteAddr;  // 远程设备地址（比如电脑的IP和端口）
    struct sockaddr_in LocalAddr;   // 本地设备地址（STM32的IP和端口）
    socklen_t AddrSize;         // 地址结构体的大小
    
    // 告诉编译器“pvParameters没用”（避免警告，新手不用管）
    LWIP_UNUSED_ARG(pvParameters);
    
    // 把地址结构体清空（初始化，避免脏数据）
    memset(&RemoteAddr, 0, sizeof(RemoteAddr));
    memset(&LocalAddr, 0, sizeof(LocalAddr));
    
    // 配置本地地址信息
    LocalAddr.sin_family = AF_INET;         // 地址类型：IPv4
    LocalAddr.sin_addr.s_addr = INADDR_ANY; // 本地IP：自动获取（或用配置的静态IP）
    LocalAddr.sin_port = htons(LOCAL_PORT); // 本地端口：LOCAL_PORT是宏定义（比如8080）
    
    // 打印本地端口，方便调试
    LOG("Local port: %d\n", LOCAL_PORT);
    
    // UDP任务主循环（一直跑）
    while (1)
    {
        // 1. 创建UDP socket（AF_INET=IPv4，SOCK_DGRAM=UDP类型）
        Sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (Sock < 0)  // 创建失败（比如内存不够）
        {
            LOG("socket error\n");  // 打印错误信息
            vTaskDelay(1000);       // 等1秒再重试
            continue;
        }
        
        // 2. 绑定socket到本地端口（把socket和STM32的8080端口关联）
        if (bind(Sock, (struct sockaddr *)&LocalAddr, sizeof(struct sockaddr)) != 0)
        {
            LOG("bind failed\n");   // 绑定失败（比如端口被占用）
            closesocket(Sock);      // 关闭socket，避免资源泄露
            vTaskDelay(1000);       // 等1秒重试
            continue;
        }
        
        // 3. 循环接收和回发数据（绑定成功后，一直等数据）
        while (1)
        {
            char RecvBuff[MEDIUM_BUFFER_LEN] = {0}; // 存收到的数据（数组大小是宏定义，比如1024）
            char SendBuff[MEDIUM_BUFFER_LEN] = {0}; // 存要发送的数据
            
            // 接收数据：从RemoteAddr（远程设备）收到数据，存在RecvBuff里
            Len = recvfrom(Sock, RecvBuff, sizeof(RecvBuff), 0, 
                          (struct sockaddr*)&RemoteAddr, &AddrSize);
            if (Len < 0)  // 接收失败
            {
                LOG("recvfrom error\n");
            }
            else  // 接收成功，打印远程设备信息和数据
            {
                // 打印“从哪个IP:端口收到数据”（inet_ntoa转IP为字符串，ntohs转端口为本地字节序）
                LOG("Receive from %s:%d\n", inet_ntoa(RemoteAddr.sin_addr), ntohs(RemoteAddr.sin_port));
                
                // 构造回发数据：在收到的数据前加“[you say]:”
                Len = snprintf(SendBuff, sizeof(SendBuff), "[you say]:%s", RecvBuff);
                
                // 把构造好的数据发回给远程设备
                Len = sendto(Sock, SendBuff, Len, 0, 
                            (struct sockaddr*)&RemoteAddr, AddrSize);
                if (Len < 0)  // 发送失败
                {
                    LOG("sendto error\n");
                }
            }
        }
    }
}

// -------------------------- TCP任务：用TCP协议连服务器、收发数据 --------------------------
// 作用：作为TCP客户端，连指定的服务器（比如电脑上的TCP工具），收到数据后
void TCP_Task(void *pvParameters)
{
    int Sock = -1;              // TCP socket句柄
    int Len = 0;                // 收发数据长度
    ip4_addr_t IpAddr;          // 服务器IP地址结构体
    struct sockaddr_in ServerAddr;  // 服务器地址（IP+端口）
    struct sockaddr_in ClientAddr;  // 本地客户端地址（STM32的IP+端口）
    socklen_t AddrSize = sizeof(ClientAddr);  // 地址结构体大小
    
    // 告诉编译器“pvParameters没用”
    LWIP_UNUSED_ARG(pvParameters);
    // 清空地址结构体
    memset(&ServerAddr, 0, sizeof(ServerAddr));
    memset(&ClientAddr, 0, sizeof(ClientAddr));
    
     // 1. 配置要连接的服务器IP（REMOTE_IP_ADDR0~3是宏定义，比如192.168.1.100）
    IP4_ADDR(&IpAddr, REMOTE_IP_ADDR0, REMOTE_IP_ADDR1, REMOTE_IP_ADDR2, REMOTE_IP_ADDR3);
    LOG("Server IP: %d.%d.%d.%d\r\n", REMOTE_IP_ADDR0, REMOTE_IP_ADDR1, REMOTE_IP_ADDR2, REMOTE_IP_ADDR3);
    LOG("Server Port: %d\r\n", REMOTE_PORT);// 打印服务器端口（比如8080）
    
     // 2. 配置服务器地址信息
    ServerAddr.sin_family = AF_INET;         // IPv4类型
    ServerAddr.sin_addr.s_addr = IpAddr.addr; // 服务器IP
    ServerAddr.sin_port = htons(REMOTE_PORT); // 服务器端口（htons：转成网络字节序）
    
    while (1)
    {
        // 3. 创建TCP socket（SOCK_STREAM=TCP类型）
        Sock = socket(AF_INET, SOCK_STREAM, 0);
        if (Sock < 0)
        {
            LOG("socket error\r\n");
            vTaskDelay(1000);
            continue;
        }
        // 4. 连接服务器（TCP必须先连接，才能通信）
        if (connect(Sock, (struct sockaddr *)&ServerAddr, sizeof(struct sockaddr)) != 0)
        {
            LOG("connect failed!\r\n");
            closesocket(Sock);
            vTaskDelay(1000);
            continue;
        }
        // 5. 获取本地客户端地址，并打印
        getsockname(Sock, (struct sockaddr *)&ClientAddr, &AddrSize);
        LOG("The server is connected from local %s:%d\r\n", inet_ntoa(ClientAddr.sin_addr), ntohs(ClientAddr.sin_port));
        // 6. 连接成功后，循环收发数据
        while (1)
        {
            char RecvBuff[MEDIUM_BUFFER_LEN] = {0};
            char SendBuff[MEDIUM_BUFFER_LEN] = {0};
            // 接收服务器发来的数据
            Len = recv(Sock, RecvBuff, sizeof(RecvBuff), 0);
            if (Len > 0)
            {
                Len = snprintf(SendBuff, sizeof(SendBuff), "[you say]:%s", RecvBuff);
                //发送数据给服务器
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
// -------------------------- FreeRTOS钩子函数：调试用 --------------------------
// 1. 内存分配失败钩子：当FreeRTOS申请内存失败时（比如栈不够），会执行这个函数
void vApplicationMallocFailedHook(void)
{
    while (1)
    {
        LOG("Malloc Failed\r\n");
    }
}
// 2. 栈溢出钩子：当某个任务的栈不够用（比如递归调用太多），会执行这个函数
void vApplicationStackOverflowHook(TaskHandle_t handle, char *para)
{
    while (1)
    {
        LOG("Stack Overflow\r\n");
    }
}

// 3. 空闲任务钩子：当所有任务都休眠时，FreeRTOS会执行这个空闲任务
void vApplicationIdleHook(void)
{
    while (1)
    {
        
    }
}



