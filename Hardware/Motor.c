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
    (1u << 0),  // ֻ����PA0
    (1u << 1),  // ֻ����PA1
    (1u << 2),  // ֻ����PA2
    (1u << 3)   // ֻ����PA3
}; // ��ת D-C-B-A

uint16_t phaseccw[4] = {
    (1u << 3),  // ֻ����PA3
    (1u << 2),  // ֻ����PA2
    (1u << 1),  // ֻ����PA1
    (1u << 0)   // ֻ����PA0
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
	
	// ��ȫд����ʹ��λ������ȷ��ֻ��λĿ������
    GPIOA->ODR &= ~(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
	//GPIO_ResetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 |GPIO_Pin_2 |GPIO_Pin_3 );
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
    for(int i=0; i<4; i++)  
    {  
        // �����Ŀ������״̬����������״̬
        GPIOA->ODR = (GPIOA->ODR & ~(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3)) | phaseccw[i];
        delay_ms(5);  
    }    
}
// ��ת
void Motor_Reverse_Rotation(void)  
{  
    for(int i=0; i<4; i++)  
    {  
        GPIOA->ODR = (GPIOA->ODR & ~(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3)) | phasecw[i];
        delay_ms(5);  
    }
}
//ֹͣ
void MotorStop(void) 
{  
    GPIOA->ODR &= ~(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
	//u1_printf("**************ֹͣ*****************\r\n");
}

//���Ƶ����ת���Ƿ�תĳ���Ƕ�
//direction����1Ϊ��ת��0Ϊ��ת
//angle�Ƕȣ���Ϊ0-360����ʵ������
void Motor_Ctrl_Direction_Angle(int direction, int angle)
{
    uint16_t steps = 64 * angle / 45;
    
    if(direction == 1) {
        while(steps--) Motor_Forward_Ration();
    } else {
        while(steps--) Motor_Reverse_Rotation();
    }
    MotorStop();
}


