#include "stm32f10x.h"
#include "ir_sensor.h"

int main(void)
{
    // ��ʼ��
    IR_Init();
    
    while(1){
        // ��ȡ������״̬
        if(IR_CheckObstacle(IR_SENSOR_11)){
            // ��⵽[1,1]��λ���ϰ���
        }
        
        // �������ʾ��
        for(uint8_t i=0; i<IR_SENSOR_COUNT; i++){
            if(IR_Read(i)){
                printf("Sensor %d triggered!\r\n", i+1);
            }
        }
    }
}