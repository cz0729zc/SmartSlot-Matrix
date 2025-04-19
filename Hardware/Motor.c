#include "motor.h"
#include "delay.h"
#include "usart1.h"

// 电机1相位数组保持不变
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

// 电机2相位数组修改：PA8改为PC14
uint16_t motor2_phasecw[4] = {
    (1u << 13),  // PC13
    (1u << 7),   // PB7
    (1u << 15),  // PA15
    (1u << 14)   // PC14 (原PA8)
}; // 正转

uint16_t motor2_phaseccw[4] = {
    (1u << 14),  // PC14 (原PA8)
    (1u << 15),  // PA15
    (1u << 7),   // PB7
    (1u << 13)   // PC13
}; // 反转

void Moto_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 初始化电机1 (PA0-PA3) - 保持不变
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIOA->ODR &= ~(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
    
    // 初始化电机2 (PC13, PB7, PA15和PC14)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA, ENABLE);
    
    // 配置PC13和PC14
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    // 配置PB7
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // 配置PA15
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 复位所有引脚
    GPIOC->ODR &= ~(GPIO_Pin_13 | GPIO_Pin_14);
    GPIOB->ODR &= ~GPIO_Pin_7;
    GPIOA->ODR &= ~GPIO_Pin_15;
}

// 电机1正转 - 保持不变
void Motor1_Forward_Ration(void)  
{  
    for(int i=0; i<4; i++)  
    {  
        GPIOA->ODR = (GPIOA->ODR & ~(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3)) | motor1_phaseccw[i];
        delay_ms(5);  
    }    
}

// 电机1反转 - 保持不变
void Motor1_Reverse_Rotation(void)  
{  
    for(int i=0; i<4; i++)  
    {  
        GPIOA->ODR = (GPIOA->ODR & ~(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3)) | motor1_phasecw[i];
        delay_ms(5);  
    }
}

// 电机2正转 - 修改为使用PC14代替PA8
void Motor2_Forward_Ration(void)  
{  
    for(int i=0; i<4; i++)  
    {  
        // 处理PC13
        GPIOC->ODR = (GPIOC->ODR & ~GPIO_Pin_13) | ((motor2_phaseccw[i] & (1u << 13)) ? GPIO_Pin_13 : 0);
        // 处理PB7
        GPIOB->ODR = (GPIOB->ODR & ~GPIO_Pin_7) | ((motor2_phaseccw[i] & (1u << 7)) ? GPIO_Pin_7 : 0);
        // 处理PA15
        GPIOA->ODR = (GPIOA->ODR & ~GPIO_Pin_15) | ((motor2_phaseccw[i] & (1u << 15)) ? GPIO_Pin_15 : 0);
        // 处理PC14 (原PA8)
        GPIOC->ODR = (GPIOC->ODR & ~GPIO_Pin_14) | ((motor2_phaseccw[i] & (1u << 14)) ? GPIO_Pin_14 : 0);
        delay_ms(5);  
    }    
}

// 电机2反转 - 修改为使用PC14代替PA8
void Motor2_Reverse_Rotation(void)  
{  
    for(int i=0; i<4; i++)  
    {  
        // 处理PC13
        GPIOC->ODR = (GPIOC->ODR & ~GPIO_Pin_13) | ((motor2_phasecw[i] & (1u << 13)) ? GPIO_Pin_13 : 0);
        // 处理PB7
        GPIOB->ODR = (GPIOB->ODR & ~GPIO_Pin_7) | ((motor2_phasecw[i] & (1u << 7)) ? GPIO_Pin_7 : 0);
        // 处理PA15
        GPIOA->ODR = (GPIOA->ODR & ~GPIO_Pin_15) | ((motor2_phasecw[i] & (1u << 15)) ? GPIO_Pin_15 : 0);
        // 处理PC14 (原PA8)
        GPIOC->ODR = (GPIOC->ODR & ~GPIO_Pin_14) | ((motor2_phasecw[i] & (1u << 14)) ? GPIO_Pin_14 : 0);
        delay_ms(5);  
    }
}

// 停止指定电机 (1:电机1, 2:电机2) - 修改电机2部分
void MotorStop(uint8_t motor_num) 
{  
    if(motor_num == 1) {
        GPIOA->ODR &= ~(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
    } else if(motor_num == 2) {
        GPIOC->ODR &= ~(GPIO_Pin_13 | GPIO_Pin_14);
        GPIOB->ODR &= ~GPIO_Pin_7;
        GPIOA->ODR &= ~GPIO_Pin_15;
    }
}

// 控制指定电机正转还是反转某个角度 - 保持不变
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
                                                    