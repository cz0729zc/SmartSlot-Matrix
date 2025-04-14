#ifndef __IR_SENSOR_H
#define __IR_SENSOR_H

#include "stm32f10x.h"

// ������IDö��
typedef enum {
    IR_SENSOR_11 = 0,  // 1��1�ų�λ
    IR_SENSOR_12,      // 1��2�ų�λ 
    IR_SENSOR_21,      // 2��1�ų�λ
    IR_SENSOR_22,      // 2��2�ų�λ
    IR_SENSOR_COUNT    // ����������
} IR_SensorID;

// ��������
void IR_Init(void);
uint8_t IR_Read(IR_SensorID sensor_id);
uint8_t IR_CheckObstacle(IR_SensorID sensor_id);

#endif /* __IR_SENSOR_H */
