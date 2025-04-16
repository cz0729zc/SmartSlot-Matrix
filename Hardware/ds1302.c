#include "ds1302.h"
#include "sys.h"
#include "delay.h"
 
struct TIMEData TimeData;
u8 read_time[7];
 
void ds1302_gpio_init()//CE,SCLK�˿ڳ�ʼ��
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PC.11  CE
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//�������
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOC.11
	GPIO_ResetBits(GPIOB,GPIO_Pin_11); 
	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //PC.12  SCLK
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//�������
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOC.12
	GPIO_ResetBits(GPIOB,GPIO_Pin_1); 
}
 
void ds1032_DATAOUT_init()//����˫��I/O�˿�Ϊ���̬
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //PC.10  DATA
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOC.10
	GPIO_ResetBits(GPIOB,GPIO_Pin_0);
}
 
void ds1032_DATAINPUT_init()//����˫��I/O�˿�Ϊ����̬
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //PC.10 DATA
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOC.10
}
 
 
void ds1302_write_onebyte(u8 data)//��DS1302����һ�ֽ�����
{
	ds1032_DATAOUT_init();
	u8 count=0;
	SCLK_L;
	for(count=0;count<8;count++)
	{	
		SCLK_L;
		if(data&0x01)
		{DATA_H;}
		else{DATA_L;}//��׼���������ٷ���
		SCLK_H;//����ʱ���ߣ���������
		data>>=1;
	}
}
 
void ds1302_wirte_rig(u8 address,u8 data)//��ָ���Ĵ�����ַ��������
{
	u8 temp1=address;
	u8 temp2=data;
	CE_L;SCLK_L;delay_us(1);
	CE_H;delay_us(2);
	ds1302_write_onebyte(temp1);
	ds1302_write_onebyte(temp2);
	CE_L;SCLK_L;delay_us(2);
}
 
u8 ds1302_read_rig(u8 address)//��ָ����ַ��ȡһ�ֽ�����
{
	u8 temp3=address;
	u8 count=0;
	u8 return_data=0x00;
	CE_L;SCLK_L;delay_us(3);
	CE_H;delay_us(3);
	ds1302_write_onebyte(temp3);
	ds1032_DATAINPUT_init();//����I/O��Ϊ����
	delay_us(2);
	for(count=0;count<8;count++)
	{
		delay_us(2);//ʹ��ƽ����һ��ʱ��
		return_data>>=1;
		SCLK_H;delay_us(4);//ʹ�ߵ�ƽ����һ��ʱ��
		SCLK_L;delay_us(14);//��ʱ14us����ȥ��ȡ��ѹ������׼ȷ
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0))
		{
			return_data=return_data|0x80;
		}
	}
	delay_us(2);
	CE_L;DATA_L;
	return return_data;
}
 
void ds1032_init()
{
//    // �ر�д����������д�룩
//    ds1302_wirte_rig(0x8e, 0x00);
//    
//    // ����ʱ��Ϊ13:01:00��24Сʱ�ƣ�
//    ds1302_wirte_rig(0x80, 0x00);  // 00�� (BCD��: 0x00)
//    ds1302_wirte_rig(0x82, 0x42);  // 01�� (BCD��: 0x01)
//    ds1302_wirte_rig(0x84, 0x11);  // 13ʱ (BCD��: 0x13)
//    
//    // ��������Ϊ2025��4��15�գ����ڶ�
//    ds1302_wirte_rig(0x86, 0x15);  // 15�� (BCD��: 0x15)
//    ds1302_wirte_rig(0x88, 0x04);  // 4�� (BCD��: 0x04)
//    ds1302_wirte_rig(0x8a, 0x02);  // ���ڶ� (1=������,2=����һ,...,7=������)
//    ds1302_wirte_rig(0x8c, 0x25);  // 2025�� (BCD��: 0x25��ʾ25��)
//    
//    // ��������д����
//    ds1302_wirte_rig(0x8e, 0x80);
}
 
void ds1032_read_time()
{
	read_time[0]=ds1302_read_rig(0x81);//����
	read_time[1]=ds1302_read_rig(0x83);//����
	read_time[2]=ds1302_read_rig(0x85);//��ʱ
	read_time[3]=ds1302_read_rig(0x87);//����
	read_time[4]=ds1302_read_rig(0x89);//����
	read_time[5]=ds1302_read_rig(0x8B);//������
	read_time[6]=ds1302_read_rig(0x8D);//����
}
 
void ds1032_read_realTime()
{
	ds1032_read_time();  //BCD��ת��Ϊ10����
	TimeData.second=(read_time[0]>>4)*10+(read_time[0]&0x0f);
	TimeData.minute=((read_time[1]>>4)&(0x07))*10+(read_time[1]&0x0f);
	TimeData.hour=(read_time[2]>>4)*10+(read_time[2]&0x0f);
	TimeData.day=(read_time[3]>>4)*10+(read_time[3]&0x0f);
	TimeData.month=(read_time[4]>>4)*10+(read_time[4]&0x0f);
	TimeData.week=read_time[5];
	TimeData.year=(read_time[6]>>4)*10+(read_time[6]&0x0f)+2000;
}

