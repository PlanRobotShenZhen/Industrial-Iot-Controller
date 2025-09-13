#include "main.h"

int main(void)
{   // 配置中断优先级分组
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

void Start_Task(void *pvParameters)
{
    while (1)
    {
        // 进入临界区：创建任务时，禁止其他任务打断（防止出错）
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
        // 创建【Flash读写任务】（操作W25Q64芯片，存数据、读数据）
        xTaskCreate((TaskFunction_t )W25Q64_SPI_TASK,
                    (const char*    )"W25Q64_SPI_TASK",
                    (uint16_t       )W25Q64_SPI_STK,
                    (void*          )NULL,
                    (UBaseType_t    )W25Q64_SPI_PRTO,
                    (TaskHandle_t*  )&W25Q64_SPI_Handler);
        vTaskDelete(Start_Handler);
        taskEXIT_CRITICAL();            //退出临界区       
    }
}

// -------------------------- 呼吸灯任务：控制LED闪灯 --------------------------
// 作用：每隔500ms翻转一次LED状态（亮→灭→亮→...），证明系统在运行
void Breath_Task(void *pvParameters)
{
    while (1)
    {
        LED(LED_DEBUG, CHANGE);
        
        vTaskDelay(500);    // FreeRTOS延时函数：让任务休眠500ms（期间CPU会去跑其他任务）
    }
}
// -------------------------- 串口接收任务：处理串口数据 --------------------------
// 作用：等待串口接收完数据，然后把收到的数据打印出来
void Recv_Task(void *pvParameters)
{
    while (1)
    {
        // 等待“日志串口（USART_ID_LOG）”的信号量：
        // - 信号量由串口中断释放（当串口收到完整数据时）
        // - portMAX_DELAY：无限等待，直到拿到信号量（不会超时）
        if (xSemaphoreTake(xSemap[USART_ID_LOG], portMAX_DELAY) == pdTRUE)
        {
            USART_Printf(USART_LOG, "[%s]%s\r\n", g_USART_Name[USART_ID_LOG], g_USART_Buf[USART_ID_LOG]);
        }
    }
}
// -------------------------- Flash读写任务：操作W25Q64芯片 --------------------------
// 作用：测试W25Q64 Flash的通信和读写功能（存“hello flash”，再读出来验证）
void W25Q64_SPI_TASK(void *pvParameters)
{
// 主函数测试代码示例
    uint8_t mfg_id;
    uint16_t dev_id;
    W25Q64_SPI_Init();
    const char *write_str = "hello flash";  // 要写入的字符串
    uint16_t str_len = strlen(write_str);   // 字符串长度（包含终止符'\0'）
    char read_str[32] = {0};                // 存储读取的字符串

    // 1. 验证Flash通信
    W25Q64_ReadID(&mfg_id, &dev_id);
    LOG("1. Flash ID 验证：\n");
    LOG("   MFG ID: 0x%02X, Dev ID: 0x%04X\n", mfg_id, dev_id);
    if (mfg_id != 0xEF || dev_id != 0x4017) {
        LOG("   错误：Flash通信失败，停止测试！\n");
        return;
    }

    // 2. 擦除目标扇区（确保写入成功，地址0x000000）
    LOG("\n2. 擦除地址0x000000所在扇区...\n");
    W25Q64_SectorErase(0x000000);
    LOG("   扇区擦除完成\n");

    // 3. 打印要写入的字符串
    LOG("\n3. 准备写入的字符串：\n");
    LOG("   内容：\"%s\"\n", write_str);
    LOG("   长度：%d字节（含终止符）\n", str_len);
    LOG("   十六进制：");
    for (uint16_t i = 0; i < str_len; i++) {
        LOG("0x%02X ", write_str[i]);
    }
    LOG("\n");

    // 4. 写入字符串到Flash
    LOG("\n4. 向地址0x000000写入数据...\n");
    W25Q64_PageProgram(0x000000, (uint8_t*)write_str, str_len);
    LOG("   写入完成\n");

    // 5. 从Flash读取字符串
    LOG("\n5. 从地址0x000000读取数据...\n");
    W25Q64_ReadData(0x000000, (uint8_t*)read_str, str_len);
    LOG("   读取完成\n");

    // 6. 打印读取的字符串
    LOG("\n6. 读取到的字符串：\n");
    LOG("   内容：\"%s\"\n", read_str);
    LOG("   长度：%d字节（含终止符）\n", strlen(read_str));
    LOG("   十六进制：");
    for (uint16_t i = 0; i < str_len; i++) {
        LOG("0x%02X ", read_str[i]);
    }
    LOG("\n");

    // 7. 验证写入与读取是否一致
    if (strcmp(write_str, read_str) == 0) {
        LOG("\n7. 验证结果：【成功】写入与读取完全一致！\n");
    } else {
        LOG("\n7. 验证结果：【失败】数据不匹配！\n");
    }  
}

// -------------------------- FreeRTOS钩子函数：调试用 --------------------------
// 1. 内存分配失败钩子：当FreeRTOS申请内存失败时（比如栈不够），会执行这个函数
void vApplicationMallocFailedHook(void)
{
    while (1)
    {
        USART_Print(USART_LOG, "Malloc Failed\r\n");
    }
}
// 2. 栈溢出钩子：当某个任务的栈不够用（比如递归调用太多），会执行这个函数
void vApplicationStackOverflowHook(TaskHandle_t handle, char *para)
{
    while (1)
    {
        USART_Print(USART_LOG, "Stack Overflow\r\n");
    }
}

// 3. 空闲任务钩子：当所有任务都休眠时，FreeRTOS会执行这个空闲任务
void vApplicationIdleHook(void)
{
    while (1)
    {
        
    }
}



