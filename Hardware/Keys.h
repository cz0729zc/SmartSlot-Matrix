#ifndef __KEYS_H
#define __KEYS_H

#include "stm32f10x.h"

// ���Ŷ���
#define KEY_PIN         GPIO_Pin_13
#define KEY_GPIO_PORT   GPIOB
#define KEY_GPIO_CLK    RCC_APB2Periph_GPIOB

// �������ܶ���
typedef enum {
    KEY_NONE = 0,
    KEY_PRESSED    // ��������
} Key_Value;

void Key_Init(void);
Key_Value Key_Scan(void);

#endif

