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
    (1u << 0),  // 只操作PA0
    (1u << 1),  // 只操作PA1
    (1u << 2),  // 只操作PA2
    (1u << 3)   // 只操作PA3
}; // 正转 D-C-B-A

uint16_t phaseccw[4] = {
    (1u << 3),  // 只操作PA3
    (1u << 2),  // 只操作PA2
    (1u << 1),  // 只操作PA1
    (1u << 0)   // 只操作PA0
}; // 反转 A-B-C-D

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
	
	// 安全写法：使用位带操作确保只复位目标引脚
    GPIOA->ODR &= ~(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
	//GPIO_ResetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 |GPIO_Pin_2 |GPIO_Pin_3 );
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
    for(int i=0; i<4; i++)  
    {  
        // 先清除目标引脚状态，再设置新状态
        GPIOA->ODR = (GPIOA->ODR & ~(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3)) | phaseccw[i];
        delay_ms(5);  
    }    
}
// 反转
void Motor_Reverse_Rotation(void)  
{  
    for(int i=0; i<4; i++)  
    {  
        GPIOA->ODR = (GPIOA->ODR & ~(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3)) | phasecw[i];
        delay_ms(5);  
    }
}
//停止
void MotorStop(void) 
{  
    GPIOA->ODR &= ~(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
	//u1_printf("**************停止*****************\r\n");
}

//控制电机正转还是反转某个角度
//direction方向，1为正转，0为反转
//angle角度，可为0-360具有实际意义
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


