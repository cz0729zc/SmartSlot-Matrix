#include "stm32f10x.h"
#include "delay.h"
#include "usart1.h"
#include "ir_sensor.h"
#include "RC522.h"
#include "OLED.h"
#include "beep.h"

// 全局变量
unsigned char g_ucTempbuf[4];
unsigned char status;


int main(void)
{
    // 硬件初始化
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD,ENABLE);
	
    // 初始化
    IR_Init();
    OLED_Init();
    OLED_Clear();
	BEEP_Init();
    Usart1_Init(9600);
    while(1){	

        //u1_printf("******************\r\n");
        RC522_Init(Card1);
        status = PcdRequest(PICC_REQALL, g_ucTempbuf);
        status = PcdAnticoll(g_ucTempbuf);

        if(status==MI_OK) // 感应到卡
        {
			BEEP_Alarm(1); // 蜂鸣器报警
            delay_ms(50);
            BEEP_Alarm(0); // 关闭蜂鸣器
            OLED_Clear();

			u1_printf("******************\r\n");
			u1_printf("g_ucTempbuf[0]:%02X\r\n", g_ucTempbuf[0]);
            u1_printf("g_ucTempbuf[1]:%02X\r\n", g_ucTempbuf[1]);
            u1_printf("g_ucTempbuf[2]:%02X\r\n", g_ucTempbuf[2]);
            u1_printf("g_ucTempbuf[3]:%02X\r\n", g_ucTempbuf[3]);
            u1_printf("Card detected!\r\n");
            
            OLED_ShowString(2, 1, "Card detected!");
            OLED_ShowHexNum(3, 1, g_ucTempbuf[0], 2);
			OLED_ShowString(3, 3, " ");
            OLED_ShowHexNum(3, 4, g_ucTempbuf[1], 2);
			OLED_ShowString(3, 6, " ");
            OLED_ShowHexNum(3, 7, g_ucTempbuf[2], 2);
			OLED_ShowString(3, 9, " ");
            OLED_ShowHexNum(3, 10, g_ucTempbuf[3], 2);
        }
        else
        {
            OLED_ShowString(1, 1, "No card!");
        }
//		//OLED_ShowString(1, 1, "triggered!");
//        // 读取传感器状态
//        if(IR_CheckObstacle(IR_SENSOR_11)){
//            // 检测到[1,1]车位有障碍物
//        }
//        
//        // 批量检测示例
//        for(uint8_t i=0; i<IR_SENSOR_COUNT; i++){
//            if(IR_Read(i)){
//                //printf("Sensor %d triggered!\r\n", i+1);
//                OLED_ShowString(1, 1, "triggered!");
//				OLED_ShowNum(1, 11, i, 2);
//            }
//        }
    }
	
	
}