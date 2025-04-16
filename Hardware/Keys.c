#include "keys.h"
#include "delay.h"

void Key_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    
    RCC_APB2PeriphClockCmd(KEY_GPIO_CLK, ENABLE);
    
    GPIO_InitStruct.GPIO_Pin = KEY_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(KEY_GPIO_PORT, &GPIO_InitStruct);
}

Key_Value Key_Scan(void) {
    static u8 key_lock = 0;
    
    if(GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_PIN) == RESET) {
        if(!key_lock) {
            delay_ms(15);
            if(GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_PIN) == RESET) {
                return KEY_PRESSED;
            }
            key_lock = 1;
        }
    }
    else {
        key_lock = 0;
    }
    return KEY_NONE;
}

