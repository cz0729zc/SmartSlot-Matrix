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

// 修改相位数组为对应PA0-PA3的位掩码
uint16_t phasecw[4] = {
    (0x0001 << 3),  // PA3 (D) 
    (0x0001 << 2),  // PA2 (C)
    (0x0001 << 1),  // PA1 (B)
    (0x0001 << 0)   // PA0 (A)
}; // 反转 D-C-B-A

uint16_t phaseccw[4] = {
    (0x0001 << 0),  // PA0 (A)
    (0x0001 << 1),  // PA1 (B)
    (0x0001 << 2),  // PA2 (C)
    (0x0001 << 3)   // PA3 (D)
}; // 正转 A-B-C-D

/*---------------------------------电机模块与单片机连接引脚---------------------------------------------------*/
/* IN4: PB9  d */
/* IN3: PB8  c */
/* IN2: PB7  b */ 
/* IN1: PB6  a */

void Moto_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
        
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    // 配置PA8为推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 |GPIO_Pin_2 |GPIO_Pin_3 );
//	 GPIO_InitTypeDef GPIO_InitStructure;
//	 RCC_APB2PeriphClockCmd(MOTOR_CLK,ENABLE);
//	
//	 GPIO_InitStructure.GPIO_Pin = MOTOR_IN1_PIN | MOTOR_IN2_PIN | MOTOR_IN3_PIN | MOTOR_IN4_PIN ;//引脚按着INT1顺序接就行了
//	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	 GPIO_Init(GPIOA,&GPIO_InitStructure);
//	 u1_printf("MOTOR_Init\r\n");
//	 GPIO_ResetBits(MOTOR_PORT, MOTOR_IN1_PIN | MOTOR_IN2_PIN |MOTOR_IN3_PIN |MOTOR_IN4_PIN );
	
}
// 正转
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
// 反转
void Motor_Reverse_Rotation(void)  
{  
	uint8_t i;  
    for(i=0;i<4;i++)  
    {  
        GPIO_Write(MOTOR_PORT, phasecw[i]);  
        delay_ms(1);  
    }  
}
//停止
void MotorStop(void) 
{  
    GPIO_Write(MOTOR_PORT, 0x0000);
	//u1_printf("**************停止*****************\r\n");
}

//控制电机正转还是反转某个角度
//direction方向，1为正转，0为反转
//angle角度，可为0-360具有实际意义
void Motor_Ctrl_Direction_Angle(int direction, int angle)
{
	uint16_t i;

	if(direction == 1)
	{								
		for(i = 0; i < 64*angle/45; i++) 
		{
			/* 正转 */
			Motor_Forward_Ration();
			
		}
		MotorStop();//停止
  	}
	else
	{
		for(i = 0; i < 64*angle/45; i++) 
		{
			/* 反转 */
			Motor_Reverse_Rotation(); 
		}
		 MotorStop();//停止
	}
}


