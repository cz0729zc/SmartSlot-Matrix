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
void Motor_Forward_Ration(void);  //��ת
void Motor_Reverse_Rotation(void);  //��ת
void MotorStop(void); //ֹͣ
void Motor_Ctrl_Direction_Angle(int direction, int angle);//���Ƶ����ת���Ƿ�תĳ���Ƕ�

#endif

