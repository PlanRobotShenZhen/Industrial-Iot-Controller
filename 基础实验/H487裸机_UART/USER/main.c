#include "main.h"

USART_InitType USART_InitStructure;

/**
 * @brief  主函数：USART1 初始化 + 打印测试
 * @param  无
 * @return 无
 */
int main(void)
{  
    /* 步骤1：配置系统时钟 */
    RCC_Configuration();

    /* 步骤2：配置 GPIO（PA9/RX、PA10/TX 为 USART1 复用功能） */
    GPIO_Configuration();
    
    /* 步骤3：配置 USART1 参数 */
    USART_StructInit(&USART_InitStructure);  // 结构体默认初始化
    USART_InitStructure.BaudRate            = 115200;    // 波特率：115200（与串口工具一致）
    USART_InitStructure.WordLength          = USART_WL_8B;// 数据位：8位
    USART_InitStructure.StopBits            = USART_STPB_1;// 停止位：1位
    USART_InitStructure.Parity              = USART_PE_NO; // 校验位：无
    USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE; // 无硬件流控
    USART_InitStructure.OverSampling        = USART_16OVER;// 过采样：16倍（提高稳定性）
    USART_InitStructure.Mode                = USART_MODE_RX | USART_MODE_TX; // 使能 RX + TX

    /* 步骤4：初始化 USART1 外设 */
    USART_Init(USARTx, &USART_InitStructure);

    /* 步骤5：使能 USART1（必须，否则无法收发） */
    USART_Enable(USARTx, ENABLE);

    /* 步骤6：通过 printf 打印测试 */
    printf("USART1 Test: PA9(RX) & PA10(TX) Config Success!\r\n");
    printf("Input any character to echo...\r\n");

    /* 步骤7：主循环（可选：添加接收回显功能，验证 RX 引脚） */
    while (1)
    {
        uint8_t recv_data;
        // 等待接收数据（检测 RX 引脚是否有数据）
        if (USART_GetFlagStatus(USARTx, USART_FLAG_RXDNE) != RESET)
        {
            recv_data = USART_ReceiveData(USARTx);  // 读取接收数据
            USART_SendData(USARTx, recv_data);      // 回显数据（发送到 TX 引脚）
            // 等待发送完成
            while (USART_GetFlagStatus(USARTx, USART_FLAG_TXC) == RESET);
        }
    }
}

/**
 * @brief  配置系统时钟（适配 USART1 和 GPIOA）
 * @param  无
 * @return 无
 */
void RCC_Configuration(void)
{
    /* 1. 启用 GPIOA 时钟（PA9/PA10 所属 GPIO 组） */
    RCC_EnableAHB1PeriphClk(USARTx_GPIO_CLK, ENABLE);  // AHB 总线时钟使能

    /* 2. 启用 USART1 时钟（APB2 总线） */
    USART_APBxClkCmd(USARTx_CLK, ENABLE);

    /* 3. 启用 AFIO 时钟（复用功能必须，官方例程保留） */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);
}


/**
 * @brief  配置 GPIO：PA9（RX）、PA10（TX）为 USART1 复用功能
 * @param  无
 * @return 无
 */
void GPIO_Configuration(void)
{
    GPIO_InitType GPIO_InitStructure;

    /* 1. 初始化 GPIO 结构体为默认值 */
    GPIO_InitStruct(&GPIO_InitStructure);
    
    /************************** 配置 PA9 为 USART1 RX（复用推挽输入） **************************/
    GPIO_InitStructure.Pin            = USARTx_RXPin;          // 引脚：PA9
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;       // 复用推挽（USART 接收引脚标准模式）
    GPIO_InitStructure.GPIO_Alternate = USARTx_RX_GPIO_AF;     // 复用号：AF5（USART1）
    GPIO_InitStructure.GPIO_Pull      = GPIO_PULL_UP;          // 上拉（避免空闲时电平不稳定）
    GPIO_InitPeripheral(USARTx_GPIO, &GPIO_InitStructure);     // 初始化 GPIOA

    /************************** 配置 PA10 为 USART1 TX（复用推挽输出） **************************/
    GPIO_InitStructure.Pin            = USARTx_TXPin;          // 引脚：PA10
    GPIO_InitStructure.GPIO_Alternate = USARTx_TX_GPIO_AF;     // 复用号：AF5（USART1）
    // 其他参数（Mode、Pull、Speed）与 RX 一致，无需重复设置
    GPIO_InitPeripheral(USARTx_GPIO, &GPIO_InitStructure);     // 初始化 GPIOA
}

/**
 * @brief  重定向 printf 到 USART1
 * @param  ch：要发送的字符
 * @param  f：文件指针（标准库默认参数）
 * @return 发送的字符
 */
int fputc(int ch, FILE* f)
{
    /* 等待发送完成（TX 寄存器为空） */
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TXC) == RESET);
    
    /* 发送 1 字节数据到 USART1 TX 引脚（PA10） */
    USART_SendData(USARTx, (uint8_t)ch);

    return (ch);
}
