#ifndef __LM2904_H
#define __LM2904_H

#include "stm32f10x.h"


#define LM2904_GPIO_PORT    GPIOB
#define LM2904_GPIO_PIN     GPIO_Pin_0
#define LM2904_ADC_CHANNEL  ADC_Channel_8 // PB0

void LM2904_Init(void);
uint16_t LM2904_ReadValue(void);
float ConvertToDecibel(uint16_t adc_value);
#endif
