#ifndef _LCD_12864_H_
#define _LCD_12864_H_

extern unsigned  char Display_Picture[64][16]; //�洢ͼƬ������

extern void InitPort_Write_LCD12864(void);//LCD12864�˿ڳ�ʼ��
extern void WriteCommand_12864(unsigned char command_12864); //дָ�����
extern void WriteData_12864(unsigned char data_12864);	  //д���ݳ���
extern unsigned int ReadData_12864();		//������
extern void InitLCD_12864(void);						  //��ʼ������
extern void ClearDot_12864(void);	   //����Ļ�����
extern void DrawDot_12864(unsigned char,unsigned char,unsigned char Color);	   //��Ļ�������,������ת��
extern void DrawDot_Picture(void);   //��ͼ,�̶���һ����άͼ��Display_Picture[][];
#endif
