#include "motor.h"
#include "delay.h"
#include "usart1.h"

// 电机1相位数组
uint16_t motor1_phasecw[4] = {
    (1u << 0),  // PA0
    (1u << 1),  // PA1
    (1u << 2),  // PA2
    (1u << 3)   // PA3
}; // 正转 D-C-B-A

uint16_t motor1_phaseccw[4] = {
    (1u << 3),  // PA3
    (1u << 2),  // PA2
    (1u << 1),  // PA1
    (1u << 0)   // PA0
}; // 反转 A-B-C-D

// 电机2相位数组
uint16_t motor2_phasecw[4] = {
    (1u << 13),  // PC13
    (1u << 14),  // PC14
    (1u << 15),  // PC15
    (1u << 8)    // PA8
}; // 正转

uint16_t motor2_phaseccw[4] = {
    (1u << 8),   // PA8
    (1u << 15),  // PC15
    (1u << 14),  // PC14
    (1u << 13)   // PC13
}; // 反转

void Moto_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 初始化电机1 (PA0-PA3)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIOA->ODR &= ~(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
    
    // 初始化电机2 (PC13-PC15和PA8)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA, ENABLE);
    
    // 配置PC13-PC15
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    // 配置PA8
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 复位所有引脚
    GPIOC->ODR &= ~(GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
    GPIOA->ODR &= ~GPIO_Pin_8;
}

// 电机1正转
void Motor1_Forward_Ration(void)  
{  
    for(int i=0; i<4; i++)  
    {  
        GPIOA->ODR = (GPIOA->ODR & ~(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3)) | motor1_phaseccw[i];
        delay_ms(5);  
    }    
}

// 电机1反转
void Motor1_Reverse_Rotation(void)  
{  
    for(int i=0; i<4; i++)  
    {  
        GPIOA->ODR = (GPIOA->ODR & ~(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3)) | motor1_phasecw[i];
        delay_ms(5);  
    }
}

// 电机2正转
void Motor2_Forward_Ration(void)  
{  
    for(int i=0; i<4; i++)  
    {  
        // 处理PC13-PC15
        GPIOC->ODR = (GPIOC->ODR & ~(GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15)) | (motor2_phaseccw[i] & 0xE000);
        // 处理PA8
        GPIOA->ODR = (GPIOA->ODR & ~GPIO_Pin_8) | (motor2_phaseccw[i] & 0x0100);
        delay_ms(5);  
    }    
}

// 电机2反转
void Motor2_Reverse_Rotation(void)  
{  
    for(int i=0; i<4; i++)  
    {  
        // 处理PC13-PC15
        GPIOC->ODR = (GPIOC->ODR & ~(GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15)) | (motor2_phasecw[i] & 0xE000);
        // 处理PA8
        GPIOA->ODR = (GPIOA->ODR & ~GPIO_Pin_8) | (motor2_phasecw[i] & 0x0100);
        delay_ms(5);  
    }
}

// 停止指定电机 (1:电机1, 2:电机2)
void MotorStop(uint8_t motor_num) 
{  
    if(motor_num == 1) {
        GPIOA->ODR &= ~(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
    } else if(motor_num == 2) {
        GPIOC->ODR &= ~(GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
        GPIOA->ODR &= ~GPIO_Pin_8;
    }
}

// 控制指定电机正转还是反转某个角度
// motor_num: 1-电机1, 2-电机2
// direction: 1为正转，0为反转
// angle: 角度，可为0-360具有实际意义
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