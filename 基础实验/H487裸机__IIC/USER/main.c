#include "main.h"
// 包含串口（USART）驱动头文件（提供串口初始化、数据发送功能）
#include "usart.h"
// 包含软件I2C驱动头文件（提供通过GPIO模拟I2C总线的读写函数）
#include "si2c.h"

/**
 * @brief  CRC8校验函数（专门适配SHT40温湿度传感器）
 * @param  data：需要校验的16位原始数据（SHT40的温度/湿度原始值都是16位）
 * @return 计算出的8位CRC校验值（用于和传感器返回的校验值对比）
 * @note   校验规则（SHT40手册规定）：
 *         1. 多项式：0x31（固定公式，传感器厂家规定）
 *         2. 初始值：0xFF（计算开始前的初始校验值）
 *         3. 无数据反向（数据传输顺序不反转）
 */
uint8_t CRC8_Caculate(uint16_t data) {
     // 1. 初始化CRC校验值（按SHT40手册要求，初始值必须是0xFF）
    uint8_t crc = 0xFF; 
    
    // 2. 把16位的原始数据拆成2个8位字节（高位字节+低位字节）
    //    例：data=0x1234 → buf[0]=0x12（高位），buf[1]=0x34（低位）
    uint8_t buf[2] = {0};
    buf[0] = (uint8_t)(data >> 8);  // 高位字节
    buf[1] = (uint8_t)(data & 0xFF); // 低位字节

    // 3. 对每个字节（共2个）进行8次校验计算（CRC核心逻辑）
    for (int i = 0; i < 2; i++) {
        crc ^= buf[i];  // 第一步：将当前字节与CRC值进行“异或”运算（混合数据）
        for (int j = 0; j < 8; j++) {
            // 判断CRC的最高位（第7位）是否为1
            // 如果最高位是1：CRC左移1位，再与多项式0x31异或
            // 如果最高位是0：CRC直接左移1位（无需异或）
            crc = (crc & 0x80) ? (crc << 1) ^ 0x31 : (crc << 1); // 多项式0x31
        }
    }
    return crc;
}

/**
 * @brief  从SHT40传感器读取温湿度数据（基于软件I2C驱动）
 * @param  pTemp：指向“存储温度值”的变量地址（读取成功后，温度会存在这里）
 * @param  pHumi：指向“存储湿度值”的变量地址（读取成功后，湿度会存在这里）
 * @return true：读取成功且数据有效；false：读取失败或数据无效
 * @note   SHT40工作流程：
 *         1. I2C总线发送传感器地址（0x44）→ 告诉传感器“要和你通信”
 *         2. 发送读取命令（0xFD）→ 告诉传感器“请返回温湿度数据”
 *         3. 接收6字节数据 → 包含：温度原始值(2字节)+温度校验(1字节)+湿度原始值(2字节)+湿度校验(1字节)
 */
bool Read_TempHumi_I2C(float *pTemp, float *pHumi) {
    uint8_t recv_buf[6] = {0};  // 存储SHT40返回的6字节数据
    uint16_t temp_raw = 0, humi_raw = 0;

    //    通过软件I2C读取SHT40数据（调用si2c.h中的现成函数）
    //    参数说明：
    //    - 0：I2C总线编号（你的代码中用的是第0路I2C）
    //    - 0x44：SHT40的I2C从机地址（固定值，手册查得）
    //    - 0xFD：SHT40的“高精度读取命令”（固定值，手册查得，还有其他命令如低精度，这里用高精度）
    //    - recv_buf：接收数据的数组（传感器返回的6字节存在这里）
    //    - 6：要读取的字节数（SHT40返回6字节，固定）
    if (SI2C_ReadData(0, 0x44, 0xFD, recv_buf, 6) != 0) {
        USART_Print(USART1, "I2C Read Failed!\r\n");      // 如果读取失败（比如I2C接线错、传感器没上电），通过串口打印错误信息
        return false;
    }

    //    从接收数组中拆分出“温度原始值”和“湿度原始值”
    //    SHT40返回的6字节格式：[temp高8位][temp低8位][temp校验][humi高8位][humi低8位][humi校验]
    temp_raw = (recv_buf[0] << 8) | recv_buf[1];  // 温度高8位左移8位 + 温度低8位 → 拼接成16位
    humi_raw = (recv_buf[3] << 8) | recv_buf[4];  // 湿度高8位左移8位 + 湿度低8位 → 拼接成16位

    //    校验数据是否正确（调用前面写的CRC8_Caculate函数）
    //    对比：计算出的温度校验值 vs 传感器返回的温度校验值（recv_buf[2]）
    //    对比：计算出的湿度校验值 vs 传感器返回的湿度校验值（recv_buf[5]）
    if (recv_buf[2] != CRC8_Caculate(temp_raw) || recv_buf[5] != CRC8_Caculate(humi_raw)) {
        USART_Print(USART1, "Data CRC Error!\r\n");  // CRC错误打印
        return false;
    }

    //    把16位原始数据转换成“实际能看懂的温湿度值”（公式来自SHT40官方手册）
    //    温度公式：temp = 原始值 × 175.0 / 65535.0 - 45.0
    //    说明：65535是16位无符号数的最大值（0~65535），175是温度量程（-45℃~130℃，共175℃），减45是偏移量
    if (pTemp != NULL) {
        *pTemp = temp_raw * 175.0f / 65535.0f - 45.0f;  // 温度范围：-45℃ ~ 130℃
    }
    //    湿度公式：humi = 原始值 × 125.0 / 65535.0 - 6.0
    //    说明：125是湿度量程（0%RH~100%RH，共125%RH），减6是偏移量（修正传感器误差）
    if (pHumi != NULL) {
        *pHumi = humi_raw * 125.0f / 65535.0f - 6.0f;   // 湿度范围：0%RH ~ 100%RH
    }

    return true;
}

/**
 * @brief  主函数（程序入口，所有代码从这里开始执行）
 * @param  无
 * @return 无（嵌入式程序通常不返回，一直循环）
 */
int main(void)
{
    // 定义变量，用于存储读取到的温湿度（float类型：支持小数，如25.5℃）
    float temp = 0.0f, humi = 0.0f;
    int a = 123;
    char str[] = "Hello USART!";

    // 初始化硬件：先初始化USART1（串口1），用于打印信息到电脑
    USART1_init();

    // 测试串口是否正常
    printf("a = %d, str = %s\r\n", a, str);
    
    //注：这个函数在si2c.h中定义，会配置I2C的SCL（时钟）和SDA（数据）引脚
    //你的硬件中SCL=PA12，SDA=PA11（初始化时会把这两个引脚配置成I2C模式）
    SI2C1_Configuration();  // 你的I2C1初始化函数（SCL=PA12，SDA=PA11）

    // 2. 循环读取温湿度并打印（每隔1秒读一次）
    while (1) {
        if (Read_TempHumi_I2C(&temp, &humi)) {
            // 3. 用USART_Print打印到USART1（格式：温度保留1位小数，湿度保留1位小数）
            USART_Print(USART1, "Temp: %.1f ℃, Humi: %.1f %%RH\r\n", temp, humi);
        } else {
            USART_Print(USART1, "Read TempHumi Error!\r\n");
        }

    }
}
