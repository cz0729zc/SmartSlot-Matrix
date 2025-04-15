/***************************************************************************************
$ @ Copyright    : 
$ @ Author       : Krone
$ @ Data         : Do not edit
$ @ LastEditor   : 
$ @ LastData     : 
$ @ Describe     : 
*****************************************************************************************/
#ifndef MOTOR_H__
#define MOTOR_H__
#include <stdio.h>
#include <string.h>

#define  MOTOR_CLK               RCC_APB2Periph_GPIOA
#define  MOTOR_PORT              GPIOA
#define  MOTOR_IN1_PIN           GPIO_Pin_0
#define  MOTOR_IN2_PIN           GPIO_Pin_1
#define  MOTOR_IN3_PIN           GPIO_Pin_2
#define  MOTOR_IN4_PIN           GPIO_Pin_3

void Moto_Init(void);
void Motor_Forward_Ration(void);  //反转
void Motor_Reverse_Rotation(void);  //正转
void MotorStop(void); //停止
void Motor_Ctrl_Direction_Angle(int direction, int angle);//控制电机正转还是反转某个角度

#endif

