/***************************************************************************************
$ @ Copyright    : 
$ @ Author       : Krone
$ @ Data         : Do not edit
$ @ LastEditor   : 
$ @ LastData     : 
$ @ Describe     : 
*****************************************************************************************/
#include "motor.h"
#include "delay.h"
#include "usart1.h"

// �޸���λ����Ϊ��ӦPA0-PA3��λ����
uint16_t phasecw[4] = {
    (0x0001 << 3),  // PA3 (D) 
    (0x0001 << 2),  // PA2 (C)
    (0x0001 << 1),  // PA1 (B)
    (0x0001 << 0)   // PA0 (A)
}; // ��ת D-C-B-A

uint16_t phaseccw[4] = {
    (0x0001 << 0),  // PA0 (A)
    (0x0001 << 1),  // PA1 (B)
    (0x0001 << 2),  // PA2 (C)
    (0x0001 << 3)   // PA3 (D)
}; // ��ת A-B-C-D

/*---------------------------------���ģ���뵥Ƭ����������---------------------------------------------------*/
/* IN4: PB9  d */
/* IN3: PB8  c */
/* IN2: PB7  b */ 
/* IN1: PB6  a */

void Moto_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
        
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    // ����PA8Ϊ�������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 |GPIO_Pin_2 |GPIO_Pin_3 );
//	 GPIO_InitTypeDef GPIO_InitStructure;
//	 RCC_APB2PeriphClockCmd(MOTOR_CLK,ENABLE);
//	
//	 GPIO_InitStructure.GPIO_Pin = MOTOR_IN1_PIN | MOTOR_IN2_PIN | MOTOR_IN3_PIN | MOTOR_IN4_PIN ;//���Ű���INT1˳��Ӿ�����
//	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	 GPIO_Init(GPIOA,&GPIO_InitStructure);
//	 u1_printf("MOTOR_Init\r\n");
//	 GPIO_ResetBits(MOTOR_PORT, MOTOR_IN1_PIN | MOTOR_IN2_PIN |MOTOR_IN3_PIN |MOTOR_IN4_PIN );
	
}
// ��ת
void Motor_Forward_Ration(void)  
{  
    int i;  
    for(i=0;i<4;i++)  
    {  
        GPIO_Write(MOTOR_PORT, phaseccw[i]);  
        delay_ms(1);  
		//u1_printf("phaseccw[%d]:%02X\r\n",i,phaseccw[i]);
    }   
}
// ��ת
void Motor_Reverse_Rotation(void)  
{  
	uint8_t i;  
    for(i=0;i<4;i++)  
    {  
        GPIO_Write(MOTOR_PORT, phasecw[i]);  
        delay_ms(1);  
    }  
}
//ֹͣ
void MotorStop(void) 
{  
    GPIO_Write(MOTOR_PORT, 0x0000);
	//u1_printf("**************ֹͣ*****************\r\n");
}

//���Ƶ����ת���Ƿ�תĳ���Ƕ�
//direction����1Ϊ��ת��0Ϊ��ת
//angle�Ƕȣ���Ϊ0-360����ʵ������
void Motor_Ctrl_Direction_Angle(int direction, int angle)
{
	uint16_t i;

	if(direction == 1)
	{								
		for(i = 0; i < 64*angle/45; i++) 
		{
			/* ��ת */
			Motor_Forward_Ration();
			
		}
		MotorStop();//ֹͣ
  	}
	else
	{
		for(i = 0; i < 64*angle/45; i++) 
		{
			/* ��ת */
			Motor_Reverse_Rotation(); 
		}
		 MotorStop();//ֹͣ
	}
}


