#ifndef __ds1302_H 
#define __ds1302_H  
 
#include "sys.h" 
 
#define CE_L GPIO_ResetBits(GPIOB,GPIO_Pin_9)//����ʹ��λ
#define CE_H GPIO_SetBits(GPIOB,GPIO_Pin_9)//����ʹ��λ
#define SCLK_L GPIO_ResetBits(GPIOB,GPIO_Pin_1)//����ʱ����
#define SCLK_H  GPIO_SetBits(GPIOB,GPIO_Pin_1)//����ʱ����
#define DATA_L  GPIO_ResetBits(GPIOB,GPIO_Pin_0)//����������
#define DATA_H  GPIO_SetBits(GPIOB,GPIO_Pin_0)//����������
 
struct TIMEData
{
	u16 year;
	u8  month;
	u8  day;
	u8  hour;
	u8  minute;
	u8  second;
	u8  week;
};//����TIMEData�ṹ�巽��洢ʱ����������
extern struct TIMEData TimeData;//ȫ�ֱ���
void ds1302_gpio_init();//ds1302�˿ڳ�ʼ��
void ds1302_write_onebyte(u8 data);//��ds1302����һ�ֽ�����
void ds1302_wirte_rig(u8 address,u8 data);//��ָ���Ĵ���дһ�ֽ�����
u8 ds1302_read_rig(u8 address);//��ָ���Ĵ�����һ�ֽ�����
void ds1032_init();//ds1302��ʼ������
void ds1032_DATAOUT_init();//IO�˿�����Ϊ���
void ds1032_DATAINPUT_init();//IO�˿�����Ϊ����
void ds1032_read_time();//��ds1302��ȡʵʱʱ�䣨BCD�룩
void ds1032_read_realTime();//��BCD��ת��Ϊʮ��������
 
#endif


