#ifndef __KEYS_H
#define __KEYS_H

#include "stm32f10x.h"

// �������Ŷ���
#define KEY_ADD_PIN     GPIO_Pin_12
#define KEY_SUB_PIN     GPIO_Pin_13
#define KEY_VIEW_PIN    GPIO_Pin_14
#define KEY_EXIT_PIN    GPIO_Pin_15
#define KEY_GPIO_PORT   GPIOB
#define KEY_GPIO_CLK    RCC_APB2Periph_GPIOB

// ����ֵ����
typedef enum {
    KEY_NONE = 0,
    KEY_ADD,    // ������ֵ/��һ����¼
    KEY_SUB,    // ������ֵ/��һ����¼
    KEY_VIEW,   // �鿴��ʷ����
    KEY_EXIT    // �˳���ʷģʽ
} Key_Value;

// ��������
void Key_Init(void);
Key_Value Key_Scan(void);

#endif

