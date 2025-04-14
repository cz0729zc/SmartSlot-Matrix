#include "WH_4G.h"
#include "usart3.h" //包含需要的头文件
#include "usart1.h"

#include <stdint.h>
SU03T Su03t;
//USART3
//TX---PB10 RX---PB11
void WH_4G_Config(void)
{
	Usart1_Init(115200);	
}
 

/**
 * @brief MODBUS CRC-16校验计算函数（符合GB/T 19582-2008标准）
 * @param data 数据缓冲区指针
 * @param len 数据长度（字节数）
 * @return 小端序CRC16校验值（低字节在前）
 */
uint16_t GetCRC16(const uint8_t *data, uint16_t len) 
{
    uint16_t crc = 0xFFFF;  // 初始值[7,8](@ref)
    const uint16_t poly = 0xA001; // 多项式反射值[7,8](@ref)
    
    while (len--) {
        crc ^= *data++;  // 逐字节异或[8](@ref)
        
        // 位处理（8次循环）
        for (uint8_t i = 0; i < 8; i++) 
        {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ poly;  // 带多项式异或的右移
            } else {
                crc >>= 1;  // 普通右移
            }
        }
    }
    
    // 结果字节交换（符合MODBUS小端序要求）[8](@ref)
    return (crc << 8) | (crc >> 8); 
}
/*
01 46 00 00 00 01 02 00 25 E2 9D
01 代表从设备地址
46 代表寄存器单元类型
00 00 为起始地址
00 01为寄存器单元长度
02 为数据长度
00 25为数据 转换成10进制就是37
E2 9D为CRC16校验（可在下方输入框中输入数据自动完成校验）
*/

//整数发送函数
void Wire4G_sendData(uint16_t adder,uint16_t value) 
{
	uint8_t Answer_buf[32] = {0x01, 0x46, 0x00,adder,0x00,0x01,0x02};
	
	// 初始化计数器和数据帧长度
	//uint8_t Answer1_buf[32] = {0x07, 0x46, 0x00,0x00,0x00,0x01,0x02,0x00,0x25,0xC9,0x3D};
	//UART3_SendBuff(Answer1_buf,11);
	uint8_t buf_count = 7;
	uint16_t CRC_send = 0;
	//u2_printf("1\r\n");
		// 将各个数据值拆分为高位和低位，并添加到发送缓冲区中
	Answer_buf[buf_count++] = value >> 8;
	Answer_buf[buf_count++] = value & 0XFF;
	uint16_t crc_ = GetCRC16(&Answer_buf[0], 9);
	//u2_printf("2\r\n");
	Answer_buf[buf_count++] = crc_ >> 8;
	Answer_buf[buf_count++] = crc_ & 0XFF;
	//u2_printf("3\r\n");
	u3_printf("buf_count:%d\r\n",buf_count);
	u1_TxData(Answer_buf, buf_count);
	//u2_printf("4\r\n");
	
//	
//	for(uint16_t a=0;a<buf_count;a++)
//	{
//		u2_printf("已发送%x\r\n",Answer_buf[a]);
//	}
	
}  
	
	//阈值报警发送函数
void Wire4G_yuzhiData(uint16_t *adder,uint16_t value) 
	{
	// 初始化发送缓冲区
	uint8_t Answer_buf[32] = {0x01, 0x46, 0x00,*adder,0x00,0x01,0x02};
	
	// 初始化计数器和数据帧长度
	//uint8_t Answer1_buf[32] = {0x07, 0x46, 0x00,0x00,0x00,0x01,0x02,0x00,0x25,0xC9,0x3D};
	//UART3_SendBuff(Answer1_buf,11);
	uint8_t buf_count = 7;
	uint16_t CRC_send = 0;
		// 将各个数据值拆分为高位和低位，并添加到发送缓冲区中
	uint8_t i=value;
	Answer_buf[buf_count++] = i >> 8;
	Answer_buf[buf_count++] = i & 0XFF;
	uint16_t crc_ = GetCRC16(&Answer_buf[0], 9);
	Answer_buf[buf_count++] = crc_ >> 8;
	Answer_buf[buf_count++] = crc_ & 0XFF;
	u1_TxData(Answer_buf, buf_count);
	
}
 
 
 

