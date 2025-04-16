#ifndef __KEYS_H
#define __KEYS_H

#include "stm32f10x.h"

// ���Ŷ���
#define KEY1_PIN        GPIO_Pin_13  // PB13
#define KEY1_GPIO_PORT  GPIOB
#define KEY1_GPIO_CLK   RCC_APB2Periph_GPIOB

#define KEY2_PIN        GPIO_Pin_14  // PB14
#define KEY2_GPIO_PORT  GPIOB
#define KEY2_GPIO_CLK   RCC_APB2Periph_GPIOB

// �������ܶ���
typedef enum {
    KEY_NONE = 0,
    KEY1_PRESSED,    // PB13��������(ע��)
    KEY2_PRESSED     // PB14��������(ע��)
} Key_Value;

void Key_Init(void);
Key_Value Key_Scan(void);

#endif

