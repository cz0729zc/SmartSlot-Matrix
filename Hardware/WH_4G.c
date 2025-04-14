#include "WH_4G.h"
#include "usart3.h" //������Ҫ��ͷ�ļ�
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
 * @brief MODBUS CRC-16У����㺯��������GB/T 19582-2008��׼��
 * @param data ���ݻ�����ָ��
 * @param len ���ݳ��ȣ��ֽ�����
 * @return С����CRC16У��ֵ�����ֽ���ǰ��
 */
uint16_t GetCRC16(const uint8_t *data, uint16_t len) 
{
    uint16_t crc = 0xFFFF;  // ��ʼֵ[7,8](@ref)
    const uint16_t poly = 0xA001; // ����ʽ����ֵ[7,8](@ref)
    
    while (len--) {
        crc ^= *data++;  // ���ֽ����[8](@ref)
        
        // λ����8��ѭ����
        for (uint8_t i = 0; i < 8; i++) 
        {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ poly;  // ������ʽ��������
            } else {
                crc >>= 1;  // ��ͨ����
            }
        }
    }
    
    // ����ֽڽ���������MODBUSС����Ҫ��[8](@ref)
    return (crc << 8) | (crc >> 8); 
}
/*
01 46 00 00 00 01 02 00 25 E2 9D
01 ������豸��ַ
46 ����Ĵ�����Ԫ����
00 00 Ϊ��ʼ��ַ
00 01Ϊ�Ĵ�����Ԫ����
02 Ϊ���ݳ���
00 25Ϊ���� ת����10���ƾ���37
E2 9DΪCRC16У�飨�����·�����������������Զ����У�飩
*/

//�������ͺ���
void Wire4G_sendData(uint16_t adder,uint16_t value) 
{
	uint8_t Answer_buf[32] = {0x01, 0x46, 0x00,adder,0x00,0x01,0x02};
	
	// ��ʼ��������������֡����
	//uint8_t Answer1_buf[32] = {0x07, 0x46, 0x00,0x00,0x00,0x01,0x02,0x00,0x25,0xC9,0x3D};
	//UART3_SendBuff(Answer1_buf,11);
	uint8_t buf_count = 7;
	uint16_t CRC_send = 0;
	//u2_printf("1\r\n");
		// ����������ֵ���Ϊ��λ�͵�λ������ӵ����ͻ�������
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
//		u2_printf("�ѷ���%x\r\n",Answer_buf[a]);
//	}
	
}  
	
	//��ֵ�������ͺ���
void Wire4G_yuzhiData(uint16_t *adder,uint16_t value) 
	{
	// ��ʼ�����ͻ�����
	uint8_t Answer_buf[32] = {0x01, 0x46, 0x00,*adder,0x00,0x01,0x02};
	
	// ��ʼ��������������֡����
	//uint8_t Answer1_buf[32] = {0x07, 0x46, 0x00,0x00,0x00,0x01,0x02,0x00,0x25,0xC9,0x3D};
	//UART3_SendBuff(Answer1_buf,11);
	uint8_t buf_count = 7;
	uint16_t CRC_send = 0;
		// ����������ֵ���Ϊ��λ�͵�λ������ӵ����ͻ�������
	uint8_t i=value;
	Answer_buf[buf_count++] = i >> 8;
	Answer_buf[buf_count++] = i & 0XFF;
	uint16_t crc_ = GetCRC16(&Answer_buf[0], 9);
	Answer_buf[buf_count++] = crc_ >> 8;
	Answer_buf[buf_count++] = crc_ & 0XFF;
	u1_TxData(Answer_buf, buf_count);
	
}
 
 
 

