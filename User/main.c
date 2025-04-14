#include "stm32f10x.h"
#include "ir_sensor.h"

int main(void)
{
    // 初始化
    IR_Init();
    
    while(1){
        // 读取传感器状态
        if(IR_CheckObstacle(IR_SENSOR_11)){
            // 检测到[1,1]车位有障碍物
        }
        
        // 批量检测示例
        for(uint8_t i=0; i<IR_SENSOR_COUNT; i++){
            if(IR_Read(i)){
                printf("Sensor %d triggered!\r\n", i+1);
            }
        }
    }
}