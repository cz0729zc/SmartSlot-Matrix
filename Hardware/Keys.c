#include "keys.h"
#include "delay.h"

void Key_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    
    // ≥ı ºªØPB13∫ÕPB14
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitStruct.GPIO_Pin = KEY1_PIN | KEY2_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
}

Key_Value Key_Scan(void) {
    static u8 key1_lock = 0, key2_lock = 0;
    Key_Value result = KEY_NONE;
    
    // ºÏ≤‚PB13
    if(GPIO_ReadInputDataBit(KEY1_GPIO_PORT, KEY1_PIN) == RESET) {
        if(!key1_lock) {
            delay_ms(15);
            if(GPIO_ReadInputDataBit(KEY1_GPIO_PORT, KEY1_PIN) == RESET) {
                result = KEY1_PRESSED;
            }
            key1_lock = 1;
        }
    }
    else {
        key1_lock = 0;
    }
    
    // ºÏ≤‚PB14
    if(GPIO_ReadInputDataBit(KEY2_GPIO_PORT, KEY2_PIN) == RESET) {
        if(!key2_lock) {
            delay_ms(15);
            if(GPIO_ReadInputDataBit(KEY2_GPIO_PORT, KEY2_PIN) == RESET) {
                result = KEY2_PRESSED;
            }
            key2_lock = 1;
        }
    }
    else {
        key2_lock = 0;
    }
    
    return result;
}
