#include "LCD12864.h"
#include "Lcd12864Char.h"
#include <STM32F10X.h>

#define uint  unsigned int
#define uchar	unsigned  char

#define LCD_DATA_GPIO  GPIOA

#define LCD_COM_GPIO  GPIOC
#define RD_GPIO   GPIO_Pin_13
#define RW_GPIO   GPIO_Pin_14
#define EN_GPIO   GPIO_Pin_15
#define LCD_BF    GPIO_Pin_7

#define    LCD_RD(x)   x ? GPIO_SetBits(LCD_COM_GPIO,RD_GPIO):  GPIO_ResetBits(LCD_COM_GPIO,RD_GPIO)
#define    LCD_RW(x)   x ? GPIO_SetBits(LCD_COM_GPIO,RW_GPIO):  GPIO_ResetBits(LCD_COM_GPIO,RW_GPIO)
#define    LCD_EN(x)   x ? GPIO_SetBits(LCD_COM_GPIO,EN_GPIO):  GPIO_ResetBits(LCD_COM_GPIO,EN_GPIO)

unsigned  char Display_Picture[64][16];//�����걻ѹ�����ģ�128λ��ÿ�ֽ�8λ��һ��16�ֽڣ�ûλ��ʾһ����

void InitPort_Write_LCD12864()
{
	GPIO_InitTypeDef   GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = RD_GPIO|RW_GPIO|EN_GPIO;
	GPIO_Init(LCD_COM_GPIO,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_Init(LCD_DATA_GPIO,&GPIO_InitStructure);
}

void InitPort_Read_LCD12864()
{
	GPIO_InitTypeDef   GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_Init(LCD_DATA_GPIO,&GPIO_InitStructure);
}

static void Delay(uchar ys)
{
	uint ys_a;
	for(;ys>0;ys--)
	for(ys_a=150;ys_a>0;ys_a--);
}
//u8 Busy_LCD12864()
//{
//	u8 LCDState=0;
//	LCD_RD(0);
//	LCD_RW(1);
//	GPIO_WriteBit(LCD_DATA_GPIO,LCD_BF,Bit_SET);
//	InitPort_Read_LCD12864();
//	Delay(10);
//	LCDState = GPIO_ReadInputDataBit(LCD_DATA_GPIO,LCD_BF);
//	LCDState=0;
//	InitPort_Write_LCD12864();
//	return LCDState;
//}
void WriteCommand_12864(uchar command_12864)
{	
	GPIO_Write(LCD_DATA_GPIO,command_12864);
	
//	while(Busy_LCD12864()==1);//æ���	
	Delay(10);
	LCD_EN(0);Delay(1);
	LCD_RD(0);Delay(1);
	LCD_RW(0);

	Delay(30);
	/*�ڴ˴�������ȡ��*/

	LCD_EN(1);
	Delay(10);
	LCD_EN(0);

}

void WriteData_12864(uchar data_12864)
{
	GPIO_Write(LCD_DATA_GPIO,data_12864);
//	while(BF&0x80);//æ���	
	Delay(10);
	LCD_EN(0);Delay(1);
	LCD_RD(1);Delay(1);
	LCD_RW(0);

	Delay(30);
	/*�ڴ˴�������ȡ��*/
	Delay(10);
	LCD_EN(1);
	Delay(10);
	LCD_EN(0);

}

unsigned int ReadData_12864()
{
	u16 read_data_12864;
	Delay(100);
	LCD_EN(0);
	LCD_RD(1);
	LCD_RW(1);
	Delay(10);
	LCD_EN(1);
	Delay(10);

	read_data_12864=GPIO_ReadInputData(LCD_DATA_GPIO);	
	LCD_EN(0);
	Delay(1);
	return read_data_12864;
}

/*============         ����ָ��趨        =========*/
/*============  WriteCommand_12864(0x34);//����ָ�����ͼ��ʾ��	============*/		 
/*============  WriteCommand_12864(0x36);//����ָ�����ͼ��ʾ��	 	============*/
/*============  WriteCommand_12864(0x32);//����ָ�����ͼ��ʾ��	============*/

void InitLCD_12864()
{
//	PSB=1;
//	RST=1;
	LCD_Init();
//	WriteCommand_12864(0x01);//����ʾ
//	WriteCommand_12864(0x30);//8λ����,����ָ�
//	WriteCommand_12864(0x0c); //����ʾ
//	WriteCommand_12864(0x01);//����ʾ
}


/*�����������ں�����ʼ��������� WriteCommand_12864(0x34);����������//����ָ�����ͼ��ʾ��*/
/*�����������ں���������������� WriteCommand_12864(0x36);����������//����ָ�����ͼ��ʾ��*/


void ClearDot_12864()	   //��������������	   uchar xcl,uchar ycl
{	 
	auto unsigned int  Count_X = 0,Count_Y = 0; //X�������Y��������������������

		WriteCommand_12864(0x34);			//����ָ�����ͼ��ʾ��	
		for(Count_Y=0;Count_Y<32;Count_Y++)  //���ϰ���
		{
			for(Count_X=0;Count_X<8;Count_X++)
			{
				WriteCommand_12864(0x80+Count_Y);	//�ı�Y��   ��ֱ��ַ 0x80+(0~31)��Ӧ�����꣬ȷ����һ��
				WriteCommand_12864(0x80+Count_X); 	//�ı�X��		ˮƽ��ַ 0x80+(0~7)��Ӧ��һ��,0x98+(0~7)��Ӧ�°���
				WriteData_12864(0x00);
				WriteData_12864(0x00);
			}
		}
				
		for(Count_Y=0;Count_Y<32;Count_Y++) 	//���°���
		{
			for(Count_X=0;Count_X<8;Count_X++)
			{
				WriteCommand_12864(0x80+Count_Y);	//�ı�Y��   ��ֱ��ַ 0x80+(0~31)��Ӧ�����꣬ȷ����һ��
				WriteCommand_12864(0x98+Count_X); 	//�ı�X��		ˮƽ��ַ 0x80+(0~7)��Ӧ��һ��,0x98+(0~7)��Ӧ�°���
				WriteData_12864(0x00);
				WriteData_12864(0x00);
			}
		}
		
		WriteCommand_12864(0x36);//����ָ�����ͼ��ʾ��	
}

void DrawDot_Picture()   //��ͼ,�̶���һ����άͼ��Display_Picture[][];
{
	unsigned int  Count_X = 0,Count_Y = 0; //X�������Y��������������������

		WriteCommand_12864(0x34);			//����ָ�����ͼ��ʾ��	

		for(Count_Y=0;Count_Y<32;Count_Y++)  //�ϰ���
		{
			for(Count_X=0;Count_X<8;Count_X++) //ԭ��С��<8,���ǳ��ԣ�������<9
			{
				WriteCommand_12864(0x80+Count_Y);	//�ı�Y��   ��ֱ��ַ 0x80+(0~31)��Ӧ�����꣬ȷ����һ��
				WriteCommand_12864(0x80+Count_X); 	//�ı�X��		ˮƽ��ַ 0x80+(0~7)��Ӧ��һ��,0x98+(0~7)��Ӧ�°���
				
				WriteData_12864(Display_Picture[Count_Y][Count_X*2]);
				WriteData_12864(Display_Picture[Count_Y][Count_X*2+1]);
			}
		}
				
		for(Count_Y=0;Count_Y<32;Count_Y++) 	//�°���
		{
			for(Count_X=0;Count_X<8;Count_X++)//ԭ��С��<8,���ǳ��ԣ�������<9
			{
				WriteCommand_12864(0x80+Count_Y);	//�ı�Y��   ��ֱ��ַ 0x80+(0~31)��Ӧ�����꣬ȷ����һ��
				WriteCommand_12864(0x98+Count_X); 	//�ı�X��		ˮƽ��ַ 0x80+(0~7)��Ӧ��һ��,0x98+(0~7)��Ӧ�°���
				WriteData_12864(Display_Picture[Count_Y+32][Count_X*2]);
				WriteData_12864(Display_Picture[Count_Y+32][Count_X*2+1]);
			}
		}
		
		WriteCommand_12864(0x36);//����ָ�����ͼ��ʾ��
}

/*���ܣ�����X�����Y���꣬������ɫ���ͣ����Զ���A���B������*/
/*Color: 1��ʾ��ʾ��0��ʾ����ʾ*/
#if 1
void DrawDot_12864(uchar X,uchar Y,uchar Color)//����ת��	 X�����ȡֵ��Ϊ��0,127��,Y������ȡֵ��Ϊ��0,64��
{
		
//	uchar Modulo_X = 0;		//�����X�ĵ�xλ
//	uchar Data_H = 0;
//	uchar Data_L = 0;
	
	X = X/16;		//����ڼ���x����
//	Modulo_X = X%16;
	
	if(Y<32)   //������ϰ�����ʾ���ֱ����Ϊ0x80
	{
		Y = Y + 0x80;		//ȷ�����ϰ����ĵڼ���
		X = 0x80+X;			//�ӵ�һ���°�����ʼ
	}
	else 
	{
		Y = Y-32 + 0x80;		//ȷ�����°����ĵڼ���
		X = 0x98+X;					//�ӵڶ����°�����ʼ
	}
	
	
	
	
	
	
	
	
	WriteCommand_12864(0x34);			//����ָ�����ͼ��ʾ��
	
	WriteCommand_12864(Y);	//�ı�Y��   ��ֱ��ַ 0x80+(0~31)��Ӧ�����꣬ȷ����һ��
	WriteCommand_12864(X); 	//�ı�X��		ˮƽ��ַ 0x80+(0~7)��Ӧ��һ��,0x98+(0~7)��Ӧ�°���
	
	WriteData_12864(0xaf);
	WriteData_12864(0xaf);

	WriteCommand_12864(0x36);//����ָ�����ͼ��ʾ��

}	
#endif





#if 0																				 
void DrawDot_12864(uchar DX,uchar y)//����ת��	 X�����ȡֵ��Ϊ��0,127��,Y������ȡֵ��Ϊ��0,64��//�ĳ�X����Ϊ��0��,128��
{
	uchar x;	
	uchar H_data;
	uchar L_data;
	uchar x1;		//����ת��coordinate_conversion
	uchar n;		//nΪѡ��LCD��7��X�����ڵ���һλ
	x=DX/15;	 	//ѡ�С�0,7������ϵ����һ��   !!!!!!!!!!!!!!!��ѡ��LCD��X���꣡����������
	n=DX%15;		//ѡ��X��������ڵ�16λ�е�һλ
	if(n<8)
	{
		 H_data=0x80>>n; //ȷ��������X���������λ��
		 L_data=0x00;//|LCD//��ʱ������;//*����LCD�öε����ݣ�Ȼ�����룬Ȼ��X����x.Ȼ��д��LCD /
		
	}
	if(n>7)
	{
		 H_data=0x00;//|LCD//��ʱ������;/*����LCD�öε����ݣ�Ȼ�����룬Ȼ��X����x.Ȼ��д��LCD  /
		 L_data=0x80>>n;

	}
	
/**************LCD�ڲ�Y����ת������ӦX�������ת***************/	
	if(y<32)
	{
		x1=8;
		y=31-y;			  //�����Ӧ�����Լ�����Y����Ϊ0ʱ����Ӧ��LCD������ӦΪX=8��Y=31.���Լ�����Y����Ϊ31ʱ����Ӧ��LCD������ӦΪX=8��Y=0.
	}
	
	if(y>=32)
	{	
		x1=0;
		y=y-32;		  //���Լ�����Y����Ϊ32ʱ����Ӧ��LCD������ӦΪY=31.���Լ�����Y����Ϊ64ʱ����Ӧ��LCD������ӦΪY=0.
		y=31-y;
	}
	x=x+x1;

	 WriteCommand_12864(0x34);//����ָ�����ͼ��ʾ��
	 Delay(5);
	 WriteCommand_12864(0x80+y);	//ע�⣺���ͺ����꣬���������� ��������
	 WriteCommand_12864(0x80+x);
	 WriteData_12864(H_data);   //��8λ����
	 WriteData_12864(L_data); 	  //��8λ����
	  		
	 WriteCommand_12864(0x36);//����ָ�����ͼ��ʾ��
	 Delay(5);
}
#endif
