#include "motor.h"
#include "delay.h"
#include "usart1.h"

// ���1��λ����
uint16_t motor1_phasecw[4] = {
    (1u << 0),  // PA0
    (1u << 1),  // PA1
    (1u << 2),  // PA2
    (1u << 3)   // PA3
}; // ��ת D-C-B-A

uint16_t motor1_phaseccw[4] = {
    (1u << 3),  // PA3
    (1u << 2),  // PA2
    (1u << 1),  // PA1
    (1u << 0)   // PA0
}; // ��ת A-B-C-D

// ���2��λ����
uint16_t motor2_phasecw[4] = {
    (1u << 13),  // PC13
    (1u << 14),  // PC14
    (1u << 15),  // PC15
    (1u << 8)    // PA8
}; // ��ת

uint16_t motor2_phaseccw[4] = {
    (1u << 8),   // PA8
    (1u << 15),  // PC15
    (1u << 14),  // PC14
    (1u << 13)   // PC13
}; // ��ת

void Moto_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // ��ʼ�����1 (PA0-PA3)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIOA->ODR &= ~(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
    
    // ��ʼ�����2 (PC13-PC15��PA8)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA, ENABLE);
    
    // ����PC13-PC15
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    // ����PA8
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // ��λ��������
    GPIOC->ODR &= ~(GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
    GPIOA->ODR &= ~GPIO_Pin_8;
}

// ���1��ת
void Motor1_Forward_Ration(void)  
{  
    for(int i=0; i<4; i++)  
    {  
        GPIOA->ODR = (GPIOA->ODR & ~(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3)) | motor1_phaseccw[i];
        delay_ms(5);  
    }    
}

// ���1��ת
void Motor1_Reverse_Rotation(void)  
{  
    for(int i=0; i<4; i++)  
    {  
        GPIOA->ODR = (GPIOA->ODR & ~(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3)) | motor1_phasecw[i];
        delay_ms(5);  
    }
}

// ���2��ת
void Motor2_Forward_Ration(void)  
{  
    for(int i=0; i<4; i++)  
    {  
        // ����PC13-PC15
        GPIOC->ODR = (GPIOC->ODR & ~(GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15)) | (motor2_phaseccw[i] & 0xE000);
        // ����PA8
        GPIOA->ODR = (GPIOA->ODR & ~GPIO_Pin_8) | (motor2_phaseccw[i] & 0x0100);
        delay_ms(5);  
    }    
}

// ���2��ת
void Motor2_Reverse_Rotation(void)  
{  
    for(int i=0; i<4; i++)  
    {  
        // ����PC13-PC15
        GPIOC->ODR = (GPIOC->ODR & ~(GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15)) | (motor2_phasecw[i] & 0xE000);
        // ����PA8
        GPIOA->ODR = (GPIOA->ODR & ~GPIO_Pin_8) | (motor2_phasecw[i] & 0x0100);
        delay_ms(5);  
    }
}

// ָֹͣ����� (1:���1, 2:���2)
void MotorStop(uint8_t motor_num) 
{  
    if(motor_num == 1) {
        GPIOA->ODR &= ~(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
    } else if(motor_num == 2) {
        GPIOC->ODR &= ~(GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
        GPIOA->ODR &= ~GPIO_Pin_8;
    }
}

// ����ָ�������ת���Ƿ�תĳ���Ƕ�
// motor_num: 1-���1, 2-���2
// direction: 1Ϊ��ת��0Ϊ��ת
// angle: �Ƕȣ���Ϊ0-360����ʵ������
void Motor_Ctrl_Direction_Angle(uint8_t motor_num, int direction, int angle)
{
    uint16_t steps = 64 * angle / 45;
    
    if(motor_num == 1) {
        if(direction == 1) {
            while(steps--) Motor1_Forward_Ration();
        } else {
            while(steps--) Motor1_Reverse_Rotation();
        }
    } else if(motor_num == 2) {
        if(direction == 1) {
            while(steps--) Motor2_Forward_Ration();
        } else {
            while(steps--) Motor2_Reverse_Rotation();
        }
    }
    MotorStop(motor_num);
}