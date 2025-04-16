#include "ds1302.h"
#include "sys.h"
#include "delay.h"
 
struct TIMEData TimeData;
u8 read_time[7];
 
void ds1302_gpio_init()//CE,SCLK端口初始化
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PC.11  CE
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOC.11
	GPIO_ResetBits(GPIOB,GPIO_Pin_11); 
	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //PC.12  SCLK
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOC.12
	GPIO_ResetBits(GPIOB,GPIO_Pin_1); 
}
 
void ds1032_DATAOUT_init()//配置双向I/O端口为输出态
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //PC.10  DATA
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOC.10
	GPIO_ResetBits(GPIOB,GPIO_Pin_0);
}
 
void ds1032_DATAINPUT_init()//配置双向I/O端口为输入态
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //PC.10 DATA
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOC.10
}
 
 
void ds1302_write_onebyte(u8 data)//向DS1302发送一字节数据
{
	ds1032_DATAOUT_init();
	u8 count=0;
	SCLK_L;
	for(count=0;count<8;count++)
	{	
		SCLK_L;
		if(data&0x01)
		{DATA_H;}
		else{DATA_L;}//先准备好数据再发送
		SCLK_H;//拉高时钟线，发送数据
		data>>=1;
	}
}
 
void ds1302_wirte_rig(u8 address,u8 data)//向指定寄存器地址发送数据
{
	u8 temp1=address;
	u8 temp2=data;
	CE_L;SCLK_L;delay_us(1);
	CE_H;delay_us(2);
	ds1302_write_onebyte(temp1);
	ds1302_write_onebyte(temp2);
	CE_L;SCLK_L;delay_us(2);
}
 
u8 ds1302_read_rig(u8 address)//从指定地址读取一字节数据
{
	u8 temp3=address;
	u8 count=0;
	u8 return_data=0x00;
	CE_L;SCLK_L;delay_us(3);
	CE_H;delay_us(3);
	ds1302_write_onebyte(temp3);
	ds1032_DATAINPUT_init();//配置I/O口为输入
	delay_us(2);
	for(count=0;count<8;count++)
	{
		delay_us(2);//使电平持续一段时间
		return_data>>=1;
		SCLK_H;delay_us(4);//使高电平持续一段时间
		SCLK_L;delay_us(14);//延时14us后再去读取电压，更加准确
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
//    // 关闭写保护（允许写入）
//    ds1302_wirte_rig(0x8e, 0x00);
//    
//    // 设置时间为13:01:00（24小时制）
//    ds1302_wirte_rig(0x80, 0x00);  // 00秒 (BCD码: 0x00)
//    ds1302_wirte_rig(0x82, 0x42);  // 01分 (BCD码: 0x01)
//    ds1302_wirte_rig(0x84, 0x11);  // 13时 (BCD码: 0x13)
//    
//    // 设置日期为2025年4月15日，星期二
//    ds1302_wirte_rig(0x86, 0x15);  // 15日 (BCD码: 0x15)
//    ds1302_wirte_rig(0x88, 0x04);  // 4月 (BCD码: 0x04)
//    ds1302_wirte_rig(0x8a, 0x02);  // 星期二 (1=星期日,2=星期一,...,7=星期六)
//    ds1302_wirte_rig(0x8c, 0x25);  // 2025年 (BCD码: 0x25表示25年)
//    
//    // 重新启用写保护
//    ds1302_wirte_rig(0x8e, 0x80);
}
 
void ds1032_read_time()
{
	read_time[0]=ds1302_read_rig(0x81);//读秒
	read_time[1]=ds1302_read_rig(0x83);//读分
	read_time[2]=ds1302_read_rig(0x85);//读时
	read_time[3]=ds1302_read_rig(0x87);//读日
	read_time[4]=ds1302_read_rig(0x89);//读月
	read_time[5]=ds1302_read_rig(0x8B);//读星期
	read_time[6]=ds1302_read_rig(0x8D);//读年
}
 
void ds1032_read_realTime()
{
	ds1032_read_time();  //BCD码转换为10进制
	TimeData.second=(read_time[0]>>4)*10+(read_time[0]&0x0f);
	TimeData.minute=((read_time[1]>>4)&(0x07))*10+(read_time[1]&0x0f);
	TimeData.hour=(read_time[2]>>4)*10+(read_time[2]&0x0f);
	TimeData.day=(read_time[3]>>4)*10+(read_time[3]&0x0f);
	TimeData.month=(read_time[4]>>4)*10+(read_time[4]&0x0f);
	TimeData.week=read_time[5];
	TimeData.year=(read_time[6]>>4)*10+(read_time[6]&0x0f)+2000;
}

