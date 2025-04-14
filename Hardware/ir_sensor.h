#ifndef __IR_SENSOR_H
#define __IR_SENSOR_H

#include "stm32f10x.h"

// 传感器ID枚举
typedef enum {
    IR_SENSOR_11 = 0,  // 1层1号车位
    IR_SENSOR_12,      // 1层2号车位 
    IR_SENSOR_21,      // 2层1号车位
    IR_SENSOR_22,      // 2层2号车位
    IR_SENSOR_COUNT    // 传感器总数
} IR_SensorID;

// 函数声明
void IR_Init(void);
uint8_t IR_Read(IR_SensorID sensor_id);
uint8_t IR_CheckObstacle(IR_SensorID sensor_id);

#endif /* __IR_SENSOR_H */
