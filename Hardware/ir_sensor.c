#include "ir_sensor.h"
#include "delay.h"

// 传感器硬件映射表
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
  * @brief 初始化红外传感器GPIO
  */
void IR_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // 使能GPIOB时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // 配置为上拉输入模式
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    
    for(uint8_t i = 0; i < IR_SENSOR_COUNT; i++){
        GPIO_InitStruct.GPIO_Pin = IR_Map[i].GPIO_Pin;
        GPIO_Init(IR_Map[i].GPIOx, &GPIO_InitStruct);
    }
}

/**
  * @brief 读取单个传感器状态
  * @param sensor_id 传感器编号
  * @return 0:无遮挡 1:有遮挡
  */
uint8_t IR_Read(IR_SensorID sensor_id)
{
    if(sensor_id >= IR_SENSOR_COUNT) 
        return 1; // 错误检测
    
    return GPIO_ReadInputDataBit(IR_Map[sensor_id].GPIOx, 
                                IR_Map[sensor_id].GPIO_Pin) == Bit_RESET;
}

/**
  * @brief 带滤波的障碍物检测
  * @param sensor_id 传感器编号
  * @return 1:确认有障碍物 0:无障碍物
  */
uint8_t IR_CheckObstacle(IR_SensorID sensor_id)
{
    uint8_t stable_cnt = 0;
    for(uint8_t i = 0; i < 3; i++){
        if(IR_Read(sensor_id)){
            stable_cnt++;
            delay_ms(2); // 采样间隔
        }else{
            break; // 中途检测到无障碍立即退出
        }
    }
    return (stable_cnt >= 2) ? 1 : 0; // 3次采样中2次有效即确认
}

