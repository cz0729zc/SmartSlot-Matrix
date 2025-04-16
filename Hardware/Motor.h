#ifndef MOTOR_H__
#define MOTOR_H__
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// ��һ���������
#define MOTOR1_CLK               RCC_APB2Periph_GPIOA
#define MOTOR1_PORT              GPIOA
#define MOTOR1_IN1_PIN           GPIO_Pin_0
#define MOTOR1_IN2_PIN           GPIO_Pin_1
#define MOTOR1_IN3_PIN           GPIO_Pin_2
#define MOTOR1_IN4_PIN           GPIO_Pin_3

// �ڶ����������
#define MOTOR2_CLK               (RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA)
#define MOTOR2_PORTC             GPIOC
#define MOTOR2_PORTA             GPIOA
#define MOTOR2_IN1_PIN           GPIO_Pin_13  // PC13
#define MOTOR2_IN2_PIN           GPIO_Pin_14  // PC14
#define MOTOR2_IN3_PIN           GPIO_Pin_15  // PC15
#define MOTOR2_IN4_PIN           GPIO_Pin_8   // PA8

void Moto_Init(void);
void Motor1_Forward_Ration(void);  // ���1��ת
void Motor1_Reverse_Rotation(void);  // ���1��ת
void Motor2_Forward_Ration(void);  // ���2��ת
void Motor2_Reverse_Rotation(void);  // ���2��ת
void MotorStop(uint8_t motor_num); // ָֹͣ�����
void Motor_Ctrl_Direction_Angle(uint8_t motor_num, int direction, int angle); // ����ָ�����

#endif