#include "keys.h"
#include "delay.h"

// 按键初始化
void Key_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(KEY_GPIO_CLK, ENABLE); 
    
    GPIO_InitStructure.GPIO_Pin = KEY_ADD_PIN | KEY_SUB_PIN | KEY_VIEW_PIN | KEY_EXIT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(KEY_GPIO_PORT, &GPIO_InitStructure);
}

// 按键扫描（支持短按）
Key_Value Key_Scan(void) {
    static uint8_t key_lock = 0;
    
    // 检测按键按下
    if(GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_ADD_PIN) == Bit_RESET ||
       GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_SUB_PIN) == Bit_RESET ||
       GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_VIEW_PIN) == Bit_RESET ||
       GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_EXIT_PIN) == Bit_RESET) 
    {
        if(!key_lock) {
            delay_ms(10); // 消抖
            if(GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_ADD_PIN) == Bit_RESET) {
                return KEY_ADD;
            } 
            else if(GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_SUB_PIN) == Bit_RESET) {
                return KEY_SUB;
            }
            else if(GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_VIEW_PIN) == Bit_RESET) {
                return KEY_VIEW;
            }
            else if(GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_EXIT_PIN) == Bit_RESET) {
                return KEY_EXIT;
            }
            key_lock = 1;
        }
    } 
    else {
        key_lock = 0;
    }
    
    return KEY_NONE;
}


