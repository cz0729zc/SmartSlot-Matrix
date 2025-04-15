#include "ir_sensor.h"
#include "delay.h"

// ������Ӳ��ӳ���
static const struct {
    GPIO_TypeDef* GPIOx;
    uint16_t GPIO_Pin;
} IR_Map[IR_SENSOR_COUNT] = {
    {GPIOA, GPIO_Pin_4}, // IR11
    {GPIOA, GPIO_Pin_5}, // IR12
    {GPIOA, GPIO_Pin_6}, // IR21
    {GPIOA, GPIO_Pin_7}  // IR22
};

/**
  * @brief ��ʼ�����⴫����GPIO
  */
void IR_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // ʹ��GPIOBʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // ����Ϊ��������ģʽ
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    
    for(uint8_t i = 0; i < IR_SENSOR_COUNT; i++){
        GPIO_InitStruct.GPIO_Pin = IR_Map[i].GPIO_Pin;
        GPIO_Init(IR_Map[i].GPIOx, &GPIO_InitStruct);
    }
}

/**
  * @brief ��ȡ����������״̬
  * @param sensor_id ���������
  * @return 0:���ڵ� 1:���ڵ�
  */
uint8_t IR_Read(IR_SensorID sensor_id)
{
    if(sensor_id >= IR_SENSOR_COUNT) 
        return 1; // ������
    
    return GPIO_ReadInputDataBit(IR_Map[sensor_id].GPIOx, 
                                IR_Map[sensor_id].GPIO_Pin) == Bit_RESET;
}

/**
  * @brief ���˲����ϰ�����
  * @param sensor_id ���������
  * @return 1:ȷ�����ϰ��� 0:���ϰ���
  */
uint8_t IR_CheckObstacle(IR_SensorID sensor_id)
{
    uint8_t stable_cnt = 0;
    for(uint8_t i = 0; i < 3; i++){
        if(IR_Read(sensor_id)){
            stable_cnt++;
            delay_ms(2); // �������
        }else{
            break; // ��;��⵽���ϰ������˳�
        }
    }
    return (stable_cnt >= 2) ? 1 : 0; // 3�β�����2����Ч��ȷ��
}

