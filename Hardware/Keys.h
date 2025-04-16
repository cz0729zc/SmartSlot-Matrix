#ifndef __KEYS_H
#define __KEYS_H

#include "stm32f10x.h"

// 引脚定义
#define KEY_PIN         GPIO_Pin_13
#define KEY_GPIO_PORT   GPIOB
#define KEY_GPIO_CLK    RCC_APB2Periph_GPIOB

// 按键功能定义
typedef enum {
    KEY_NONE = 0,
    KEY_PRESSED    // 按键按下
} Key_Value;

void Key_Init(void);
Key_Value Key_Scan(void);

#endif

