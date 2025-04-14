#include "LM2904.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_gpio.h"
#include "delay.h"

#define ADC1_DR_Address    ((uint32_t)0x4001244C)  // ADC1���ݼĴ�����ַ

static volatile uint16_t ADC_ConvertedValue = 0;

void LM2904_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    // 1. ʹ��ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // 2. ����PB0Ϊģ������
    GPIO_InitStructure.GPIO_Pin = LM2904_GPIO_PIN;  // GPIO_Pin_0
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;   // ģ������ģʽ
    GPIO_Init(LM2904_GPIO_PORT, &GPIO_InitStructure);

    // 3. ����DMA��ͨ��1����ADC1��
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 1;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel1, ENABLE);

    // 4. ����ADC1��ͨ��8��ӦPB0��
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;     // ��ͨ������ɨ��
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_RegularChannelConfig(ADC1, LM2904_ADC_CHANNEL, 1, ADC_SampleTime_239Cycles5); // ͨ��8
    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    // 5. ADCУ׼
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));

    // 6. ����ת��
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

uint16_t LM2904_ReadValue(void) {
    return ADC_ConvertedValue;
}

float ConvertToDecibel(uint16_t adc_value) {
	//db=����ź�/�������ѹ-1.7��*100+30
    return (float)LM2904_ReadValue() * (3.3 / 4096) / 3.3 * 100 + 30; // ÿADC��λ��Ӧ0.2dB
} 	