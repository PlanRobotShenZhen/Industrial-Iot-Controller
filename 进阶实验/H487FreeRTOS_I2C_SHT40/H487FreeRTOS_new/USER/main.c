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
         // 创建I2C任务                       
        xTaskCreate((TaskFunction_t )I2C_Task,
                    (const char*    )"Recv_Task",
                    (uint16_t       )I2C_STK,
                    (void*          )NULL,
                    (UBaseType_t    )I2C_PRTO,
                    (TaskHandle_t*  )&I2C_Handler);
        vTaskDelete(Start_Handler);
        taskEXIT_CRITICAL();            //退出临界区       
    }
}
/*
本次实验用到的模块是温湿度STH40传感器 接入VCC GND  SDA  SCL 即可通过RS485——3进行通信
*/

void I2C_Task(void *pvParameters)
{    
    
      SI2C1_Configuration();  // I2C1初始化函数（SCL=PA12，SDA=PA11）
    float temp = 0.0f, humi = 0.0f;
    
      // 2. 循环读取温湿度并打印（每隔1秒读一次）
    while (1) {
        if (Read_TempHumi_I2C(&temp, &humi)) {
            // 3. 用USART_Print打印到USART1（格式：温度保留1位小数，湿度保留1位小数）
            USART_Print(USART_LOG, "Temp: %.1f ℃, Humi: %.1f %%RH\r\n", temp, humi);
        } else {
            USART_Print(USART_LOG, "Read TempHumi Error!\r\n");
        }

    }
    
}
// -------------------------- 呼吸灯任务：控制LED闪灯 --------------------------
// 作用：每隔500ms翻转一次LED状态（亮→灭→亮→...），证明系统在运行
void Breath_Task(void *pvParameters)
{
    while (1)
    {
        LED(LED_DEBUG, CHANGE);
        
        vTaskDelay(500);
    }
}
// -------------------------- 串口接收任务：处理串口数据 --------------------------
// 作用：等待串口接收完数据，然后把收到的数据打印出来
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



