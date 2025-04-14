#include "delay.h"
// ��ȷ��ʱ������72MHz��Ƶ��
void delay_us(uint32_t us) {
    us = us * 9; // 72MHz / 8 = 9MHz
    while(us--) {
        __NOP(); __NOP(); __NOP(); __NOP(); 
        __NOP(); __NOP(); __NOP(); __NOP();
    }
}

void delay_ms(uint32_t ms) {
    while(ms--) {
        delay_us(1000);
    }
}
