#include "i2c.h"
#include "init.h"
SI2C_Init_t SI2C_Init_Type[1];
/**
 * @brief  微秒级延时函数（软件延时，依赖CPU主频）
 * @param  us：需要延时的微秒数（比如5代表延时5μs）
 * @return 无
 * @note   原理：根据CPU主频（168MHz）计算需要执行的空指令（__NOP()）次数
 *         168MHz = 168,000,000条指令/秒 → 1秒=1,000,000μs → 1μs≈168条指令
 *         这里除以5是因为：while循环+__NOP()约占5条指令，确保延时精度接近1μs
 */
void delay_loop_us(uint32_t us) {
    uint32_t cycles = us * (168000000UL / 1000000UL / 5);  // 5个指令周期约1us
    while (cycles--) {
        __NOP();  // 空指令，消耗1个周期
    }
}
/**
 * @brief  I2C1总线初始化（指定SCL/SDA引脚，配置硬件参数）
 * @param  无
 * @return 无
 * @note   这里配置的是第0路I2C（x=0），实际对应硬件的I2C1
 */
void SI2C1_Configuration(void)
{
	#define x 0
	// 给I2C初始化结构体赋值：指定SCL和SDA的GPIO端口、引脚
	SI2C_Init_Type[x] = (SI2C_Init_t)
	{
		.SCL_M = GPIOA,       // SCL线的GPIO端口：GPIOA
		.SDA_M = GPIOA,       // SDA线的GPIO端口：GPIOA
		.SCL_Pin =GPIO_PIN_12,// SCL线的引脚：PA12
		.SDA_Pin =GPIO_PIN_11,// SDA线的引脚：PA11
	};
    // 调用I2C初始化核心函数（在init.h中定义）：
	// 实际会配置GPIO为开漏输出（I2C协议要求）、使能GPIO时钟等底层操作
	SI2C_INIT(SI2C_Init_Type[x]);
	
	#undef x
}
/**
 * @brief  将SDA引脚设置为输入模式（用于接收数据）
 * @param  x：I2C编号（比如0代表第0路I2C）
 * @return 无
 */
void SDA_InputMode(uint8_t x)
{ 
    GPIO_InitType GPIO_InitStruct;// GPIO初始化参数结构体（在init.h中定义）
    // 配置输入模式参数
	GPIO_InitStruct.GPIO_Mode = GPIO_MODE_INPUT;  // 模式：输入模式
	GPIO_InitStruct.GPIO_Pull = GPIO_PULL_DOWN;   // 下拉电阻：防止引脚悬空时电平不稳定
	GPIO_InitStruct.GPIO_Current =GPIO_DC_12mA;   // 电流驱动能力：12mA（可根据硬件调整）
	GPIO_InitStruct.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;//  slew率：快速（提升数据传输速度）
	GPIO_InitStruct.Pin = SI2C_Init_Type[x].SDA_Pin ;// 引脚：使用第x路I2C配置的SDA引脚（PA11）
    // 调用底层函数，将配置应用到SDA引脚（GPIOA端口）   
    GPIO_InitPeripheral(SI2C_Init_Type[x].SDA_M ,&GPIO_InitStruct);
}
/**
 * @brief  将SDA引脚设置为输出模式（用于发送数据）
 * @param  x：I2C编号（比如0代表第0路I2C）
 * @return 无
 * @note   I2C协议要求SDA为开漏输出，但这里用推挽输出（GPIO_MODE_OUTPUT_PP）也能工作
 *         （开漏输出需外部上拉电阻，推挽输出内部已处理，简化硬件）
 */
void SDA_OutputMode(uint8_t x)
{
    GPIO_InitType GPIO_InitStruct;// GPIO初始化参数结构体
    // 配置输出模式参数
	GPIO_InitStruct.GPIO_Mode = GPIO_MODE_OUTPUT_PP; // 模式：推挽输出（适合双向通信）
	GPIO_InitStruct.GPIO_Pull = GPIO_PULL_DOWN; // 下拉电阻：防止空闲时电平漂移
	GPIO_InitStruct.GPIO_Current =GPIO_DC_12mA;// 电流驱动能力：12mA
	GPIO_InitStruct.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;// 快速slew率
	GPIO_InitStruct.Pin = SI2C_Init_Type[x].SDA_Pin ;// 引脚：第x路I2C的SDA引脚（PA11）
    // 将配置应用到SDA引脚
    GPIO_InitPeripheral(SI2C_Init_Type[x].SDA_M ,&GPIO_InitStruct);
}
/**
 * @brief  I2C延时函数（封装底层延时，统一延时接口）
 * @param  us：延时微秒数
 * @return 无
 * @note   后续所有I2C时序的延时，都调用这个函数，方便统一调整
 */
void SI2C_delay(uint32_t us)
{
	delay_loop_us(us);
}
/**
 * @brief  将SDA线置为高电平（表示数据1或空闲）
 * @param  x：I2C编号
 * @return 无
 */
void SDA_H(uint8_t x)
{
    // GPIO_WriteBits：底层函数，设置指定引脚的电平（Bit_SET=高电平）
	GPIO_WriteBits(SI2C_Init_Type[x].SDA_M, SI2C_Init_Type[x].SDA_Pin, Bit_SET);
}
/**
 * @brief  将SCL线置为高电平（时钟高电平，用于锁存数据）
 * @param  x：I2C编号
 * @return 无
 * @note   SCL高电平时，SDA的电平不能变（否则会被识别为开始/停止信号）
 */
void SCL_H(uint8_t x)
{
	GPIO_WriteBits(SI2C_Init_Type[x].SCL_M, SI2C_Init_Type[x].SCL_Pin, Bit_SET);
}
/**
 * @brief  将SDA线置为低电平（表示数据0或开始/停止信号）
 * @param  x：I2C编号
 * @return 无
 */
void SDA_L(uint8_t x)
{
	GPIO_WriteBits(SI2C_Init_Type[x].SDA_M, SI2C_Init_Type[x].SDA_Pin, Bit_RESET);
}
/**
 * @brief  读取SDA线的当前电平（用于接收数据或应答信号）
 * @param  x：I2C编号
 * @return 0：SDA低电平；1：SDA高电平
 * @note   读取前必须先将SDA设为输入模式，读完后切回输出模式（保证后续能发送数据）
 */
uint8_t SDA_R(uint8_t x)
{
    SDA_InputMode(x);// 1. 切换SDA为输入模式（才能读取外部电平）
    // 2. 读取SDA引脚电平（GPIO_ReadInputDataBit：底层函数，读取指定引脚输入值）
    uint8_t ret = GPIO_ReadInputDataBit(SI2C_Init_Type[x].SDA_M, SI2C_Init_Type[x].SDA_Pin);
    SDA_OutputMode(x);// 3. 切回输出模式（供后续发送数据用）
	return ret;// 返回读取到的电平值
}
/**
 * @brief  将SCL线置为低电平（时钟低电平，用于准备下一位数据）
 * @param  x：I2C编号
 * @return 无
 * @note   SCL低电平时，SDA的电平可以变化（准备下一个数据位）
 */
void SCL_L(uint8_t x)
{
	GPIO_WriteBits(SI2C_Init_Type[x].SCL_M, SI2C_Init_Type[x].SCL_Pin, Bit_RESET);
}
/**
 * @brief  生成I2C开始信号（启动通信）
 * @param  x：I2C编号
 * @return 无
 * @note   I2C协议规定的开始信号：SCL高电平时，SDA从高电平拉到低电平
 *         时序步骤：SDA高→SCL高（延时）→SDA低（延时）→SCL低（准备发送数据）
 */
void SI2C_Start(uint8_t x)
{
    SDA_H(x);  // 1. SDA先置高（空闲状态）
	SCL_H(x);  // 2. SCL置高（时钟高电平，此时SDA变化会被识别为开始信号）
	SI2C_delay(5);  // 延时5μs，确保电平稳定
	SDA_L(x);  // 3. SDA从高拉低（关键！生成开始信号）
	SI2C_delay(5);  // 延时5μs，确保传感器能识别到
	SCL_L(x);  // 4. SCL置低（进入数据传输阶段，SDA可变化）
}
/**
 * @brief  生成I2C停止信号（结束通信）
 * @param  x：I2C编号
 * @return 无
 * @note   I2C协议规定的停止信号：SCL高电平时，SDA从低电平拉到高电平
 *         时序步骤：SDA低→SCL高（延时）→SDA高（延时）→完成停止
 */
void SI2C_Stop(uint8_t x)
{
	SDA_L(x);  // 1. SDA先置低（数据传输状态）
	SCL_H(x);  // 2. SCL置高（时钟高电平，此时SDA变化会被识别为停止信号）
	SI2C_delay(5);  // 延时5μs，确保电平稳定
	SDA_H(x);  // 3. SDA从低拉高（关键！生成停止信号）
	SI2C_delay(5);  // 延时5μs，确保传感器能识别到
}
/**
 * @brief  向I2C从设备发送1个字节（8位数据）
 * @param  x：I2C编号
 * @param  Byte：要发送的8位数据（比如传感器地址、命令）
 * @return 无
 * @note   发送顺序：从最高位（bit7）到最低位（bit0），每bit都需要SCL时钟锁存
 *         时序步骤：准备bit→SCL高（锁存）→SCL低（准备下一个bit）
 */
void SI2C_SendByte(uint32_t x, uint8_t Byte)
{
    uint8_t i;
    // 循环8次，发送8个bit（从bit7到bit0）
    for (i = 0; i < 8; i++)
	{
         // 判断当前要发送的bit是否为1（Byte & 0x80：取最高位）
        if (Byte & 0x80)
		{
            SDA_H(x); // 若为1，SDA置高
        }
		else
		{
            SDA_L(x); // 若为0，SDA置低
        }
        Byte <<= 1;// 数据左移1位，准备下一个bit（比如0x12→0x24，下一位是bit6）
        SI2C_delay(2);// 延时2μs，确保SDA电平稳定
        SCL_H(x); // SCL置高（锁存数据：传感器在此时读取SDA的电平）
        SI2C_delay(5); // 延时5μs，确保传感器读取完成
        SCL_L(x); //  SCL置低（准备下一个bit的SDA电平）
        SI2C_delay(2);// 延时2μs，确保SDA电平稳定
    }
}

/**
 * @brief  从I2C从设备接收1个字节（8位数据）
 * @param  x：I2C编号
 * @return 接收到的8位数据
 * @note   接收顺序：从最高位（bit7）到最低位（bit0），每bit都需要SCL时钟采样
 *         接收前必须将SDA设为输入模式（才能读取传感器发送的电平）
 */
uint8_t SI2C_ReceiveByte(uint32_t x)
{
    uint8_t i, Byte = 0; // Byte初始化为0，用于存储接收的数据
    
    SDA_InputMode(x);    // 1. 切换SDA为输入模式（准备接收数据）
    // 循环8次，接收8个bit（从bit7到bit0）
    for (i = 0; i < 8; i++)
    {
        SCL_H(x);// 2. SCL置高（传感器在此时输出当前bit的电平，单片机采样）
        SI2C_delay(5);// 延时5μs，确保传感器输出的电平稳定
        Byte <<= 1;// 3. 数据左移1位，腾出最低位（准备存储当前bit）
        // 4. 读取SDA电平：若为1，将Byte的最低位置1；若为0，保持0
        if (SDA_R(x))
        {
            Byte |= 0x01;// 最低位置1
        }
        SCL_L(x);// 5. SCL置低（传感器准备下一个bit的电平）
        SI2C_delay(2);// 延时2μs，确保SCL电平稳定
    }

    return Byte;  // 6. 返回接收到的8位数据
}
/**
 * @brief  发送应答信号（ACK）：告诉传感器“我收到数据了，继续发”
 * @param  x：I2C编号
 * @return 无
 * @note   应答信号：SCL高电平时，SDA为低电平
 */
void SI2C_SendACK(uint32_t x)
{
    SDA_L(x);  // 1. SDA置低（表示应答）
    SI2C_delay(2);  // 延时2μs，确保电平稳定
    SCL_H(x);  // 2. SCL置高（传感器采样SDA电平，识别为应答）
    SI2C_delay(5);  // 延时5μs，确保传感器识别完成
    SCL_L(x);  // 3. SCL置低（结束应答）
    SI2C_delay(2);  // 延时2μs，确保电平稳定
    SDA_H(x);  // 4. SDA置高（恢复空闲状态，准备后续通信）
}

/**
 * @brief  发送非应答信号（NACK）：告诉传感器“我收完了，别发了”
 * @param  x：I2C编号
 * @return 无
 * @note   非应答信号：SCL高电平时，SDA为高电平
 *         通常在接收最后1字节时发送，避免传感器继续发无效数据
 */
void SI2C_SendNACK(uint32_t x)
{
    SDA_H(x);  // 1. SDA置高（表示非应答）
    SI2C_delay(2);  // 延时2μs，确保电平稳定
    SCL_H(x);  // 2. SCL置高（传感器采样SDA电平，识别为非应答）
    SI2C_delay(5);  // 延时5μs，确保传感器识别完成
    SCL_L(x);  // 3. SCL置低（结束非应答）
    SI2C_delay(2);  // 延时2μs，确保电平稳定
}

/**
 * @brief  等待从设备的应答信号（ACK）：确认传感器是否收到数据
 * @param  x：I2C编号
 * @return 0：收到应答（ACK，传感器成功接收）；1：未收到应答（NACK，接收失败）
 * @note   发送1字节后必须调用此函数，确认通信是否正常
 */
uint8_t SI2C_WaitACK(uint32_t x)
{
    uint8_t Ack;  // 存储应答结果（0=ACK，1=NACK）
	
    SCL_H(x);  // 1. SCL置高（传感器在此时输出应答信号）
    SI2C_delay(5);  // 延时5μs，确保传感器输出的应答信号稳定
    Ack = SDA_R(x);  // 2. 读取SDA电平（0=ACK，1=NACK）
    SCL_L(x);  // 3. SCL置低（结束应答等待）
    SI2C_delay(2);  // 延时2μs，确保电平稳定

    return Ack;
}

/**
 * @brief  向I2C从设备的指定寄存器写入多个字节数据
 * @param  x：I2C编号
 * @param  SlaveAddr：从设备地址（比如SHT40的地址是0x44）
 * @param  RegAddr：要写入的寄存器地址（比如传感器的配置寄存器）
 * @param  Data：要写入的数据数组（存储多个字节）
 * @param  Num：要写入的数据字节数
 * @return 0：写入成功；1：写入失败（未收到应答）
 * @note   写数据流程：开始→发从机地址→等应答→发寄存器地址→等应答→发数据→等应答→停止
 */
uint8_t SI2C_WriteData(uint8_t x, uint8_t SlaveAddr, uint8_t RegAddr, uint8_t* Data, uint32_t Num)
{
    SI2C_Start(x);  // 1. 发送开始信号（启动通信）
    // 2. 发送从机地址（ SlaveAddr << 1：左移1位，最低位0表示“写操作”）
    SI2C_SendByte(x, SlaveAddr << 1);
    if (SI2C_WaitACK(x))  // 3. 等待应答：若返回1（NACK），表示传感器没响应
    {
        SI2C_Stop(x);  // 发送停止信号，结束通信
        return 1;      // 返回1，标记失败
    }
    // 4. 发送要写入的寄存器地址（告诉传感器“我要写这个寄存器”）
    SI2C_SendByte(x, RegAddr);
    if (SI2C_WaitACK(x))  // 5. 等待应答：若失败，结束通信
    {
        SI2C_Stop(x);
        return 1;
    }
    // 6. 循环写入Num个字节数据（每个字节都要等应答）
	for (uint32_t i = 0; i < Num; i++)
	{
		SI2C_SendByte(x, Data[i]);  // 发送第i个字节
		if (SI2C_WaitACK(x))        // 等待应答：若失败，结束通信
		{
			SI2C_Stop(x);
			return 1;
		}
	}
    SI2C_Stop(x);  // 7. 发送停止信号，结束通信
    return 0;      // 返回0，标记成功
}

/**
 * @brief  从I2C从设备的指定寄存器读取多个字节数据
 * @param  x：I2C编号
 * @param  SlaveAddr：从设备地址（比如SHT40的地址是0x44）
 * @param  RegAddr：要读取的寄存器地址（比如传感器的温湿度数据寄存器）
 * @param  Data：存储读取数据的数组（读完后数据存在这里）
 * @param  Num：要读取的数据字节数
 * @return 0：读取成功；1：读取失败（未收到应答）
 * @note   读数据流程（I2C读多字节标准流程）：
 *         开始→发从机地址（写）→等应答→发寄存器地址→等应答→延时（传感器准备数据）→
 *         重新开始→发从机地址（读）→等应答→读数据（最后1字节发NACK）→停止
 */
uint8_t SI2C_ReadData(uint8_t x, uint8_t SlaveAddr, uint8_t RegAddr, uint8_t *Data, uint32_t Num)
{
    // 第一阶段：告诉传感器“我要读哪个寄存器”（写寄存器地址）
    SI2C_Start(x);  // 1. 发送开始信号
    
    // 2. 发送从机地址（写操作：最低位0）
    SI2C_SendByte(x, SlaveAddr << 1);
    if (SI2C_WaitACK(x))  // 3. 等待应答：失败则结束
    {
        SI2C_Stop(x);
        return 1;
    }
    
    // 4. 发送要读取的寄存器地址（告诉传感器“我要读这个寄存器”）
    SI2C_SendByte(x, RegAddr);
    if (SI2C_WaitACK(x))  // 5. 等待应答：失败则结束
    {
        SI2C_Stop(x);
        return 1;
    }
	
    // 关键延时：给传感器时间准备数据（比如SHT40需要时间测量温湿度，约10ms）
    SI2C_delay(10000);  // 10000μs = 10ms
    
    // 第二阶段：从传感器读取数据（读操作）
    SI2C_Start(x);  // 6. 发送重新开始信号（切换为读操作）
    
    // 7. 发送从机地址（读操作：最低位1）
    SI2C_SendByte(x, (SlaveAddr << 1) | 0x01);
    if (SI2C_WaitACK(x))  // 8. 等待应答：失败则结束
    {
        SI2C_Stop(x);
        return 1;
    }
    // 9. 循环读取Num个字节（前Num-1个字节发ACK，最后1个发NACK）
	for (uint32_t i = 0; i < Num - 1; i++)
	{
		Data[i] = SI2C_ReceiveByte(x);  // 读取第i个字节
		SI2C_SendACK(x);                // 发送ACK，告诉传感器“继续发”
	}
	
	// 10. 读取最后1个字节（发NACK，告诉传感器“别发了”）
	Data[Num - 1] = SI2C_ReceiveByte(x);
    SI2C_SendNACK(x);
	
    SI2C_Stop(x);  // 11. 发送停止信号，结束通信
    return 0;      // 返回0，标记成功
}
