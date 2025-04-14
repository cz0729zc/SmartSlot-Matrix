#ifndef __KEYS_H
#define __KEYS_H

#include "stm32f10x.h"

// 按键引脚定义
#define KEY_ADD_PIN     GPIO_Pin_12
#define KEY_SUB_PIN     GPIO_Pin_13
#define KEY_VIEW_PIN    GPIO_Pin_14
#define KEY_EXIT_PIN    GPIO_Pin_15
#define KEY_GPIO_PORT   GPIOB
#define KEY_GPIO_CLK    RCC_APB2Periph_GPIOB

// 按键值定义
typedef enum {
    KEY_NONE = 0,
    KEY_ADD,    // 增加阈值/上一条记录
    KEY_SUB,    // 减少阈值/下一条记录
    KEY_VIEW,   // 查看历史数据
    KEY_EXIT    // 退出历史模式
} Key_Value;

// 函数声明
void Key_Init(void);
Key_Value Key_Scan(void);

#endif

