#include "stm32f10x.h"
#include "delay.h"
#include "usart1.h"

#include "ir_sensor.h"
#include "RC522.h"
#include "OLED.h"
#include "beep.h"
#include "ds1302.h"
#include "motor.h"
// ȫ�ֱ���
unsigned char g_ucTempbuf[4];
unsigned char status;

// ָ���
const uint8_t First[] = {0x7E, 0x04, 0x03, 0x00, 0X01, 0xEF}; // ��һ��ָ��
const uint8_t Next[]  = {0x7E, 0x04, 0x03, 0x00, 0X02, 0xEF}; // �ڶ���ָ��
const uint8_t Add[]   = {0x7E, 0x02, 0x04, 0xEF};            // ����+ָ��
const uint8_t Sub[]   = {0x7E, 0x02, 0x05, 0xEF};            // ������ָ��



int main(void)
{
	u8 min=0;
	
	
    // ��ʼ��
    IR_Init();
    OLED_Init();
    OLED_Clear();
	BEEP_Init();
	Moto_Init();
    Usart1_Init(9600);
	
	ds1302_gpio_init();delay_ms(5); //��ʼ��ds1302�˿�
	ds1032_init();delay_ms(5);      //ds1302��ʼ��
	ds1032_read_realTime();         //��ȡʵʱ����
	min=TimeData.minute;            //��¼��ʱ����ֵ

	//��OLED������ʾʵʱʱ��
	OLED_Clear();
	OLED_ShowString(2,3,":");
	OLED_ShowNum(2,4,TimeData.minute,2);
	OLED_ShowNum(2,1,TimeData.hour,2);		
	OLED_ShowNum(1,1,TimeData.year,4);
	OLED_ShowNum(1,6,TimeData.month,2);
	OLED_ShowNum(1,9,TimeData.day,2);
	OLED_ShowString(1,5,"-");
	OLED_ShowString(1,8,"-");


	//u1_printf("Start\r\n");
    while(1){	
		
		delay_ms(1000);
		ds1032_read_realTime();    //��ȡ��ʱʱ��
		if(min==TimeData.minute)//�жϷ������Ƿ��и���
		{
			delay_ms(5);//û�и��±���ʱ5ms
		}
		else
		{
			min=TimeData.minute;//�������и��£��򽫱���min����
		
			//������������ʹOLED��������ʾ
			OLED_Clear();
			OLED_ShowString(2,3,":");
			OLED_ShowNum(2,4,TimeData.minute,2);
			OLED_ShowNum(2,1,TimeData.hour,2);		
			OLED_ShowNum(1,1,TimeData.year,4);
			OLED_ShowNum(1,6,TimeData.month,2);
			OLED_ShowNum(1,9,TimeData.day,2);
			OLED_ShowString(1,5,"-");
			OLED_ShowString(1,8,"-");
		}
		
		
		
        //u1_printf("******************\r\n");
        RC522_Init(Card1);
        status = PcdRequest(PICC_REQALL, g_ucTempbuf);
        status = PcdAnticoll(g_ucTempbuf);

        if(status==MI_OK) // ��Ӧ����
        {
			BEEP_Alarm(1); // ����������
            delay_ms(50);
            BEEP_Alarm(0); // �رշ�����
            //OLED_Clear();

//			u1_printf("******************\r\n");
//			u1_printf("g_ucTempbuf[0]:%02X\r\n", g_ucTempbuf[0]);
//            u1_printf("g_ucTempbuf[1]:%02X\r\n", g_ucTempbuf[1]);
//            u1_printf("g_ucTempbuf[2]:%02X\r\n", g_ucTempbuf[2]);
//            u1_printf("g_ucTempbuf[3]:%02X\r\n", g_ucTempbuf[3]);
//            u1_printf("Card detected!\r\n");
            
            OLED_ShowString(3, 1, "Card detected!");
            OLED_ShowHexNum(4, 1, g_ucTempbuf[0], 2);
			OLED_ShowString(4, 3, " ");
            OLED_ShowHexNum(4, 4, g_ucTempbuf[1], 2);
			OLED_ShowString(4, 6, " ");
            OLED_ShowHexNum(4, 7, g_ucTempbuf[2], 2);
			OLED_ShowString(4, 9, " ");
            OLED_ShowHexNum(4, 10, g_ucTempbuf[3], 2);
			
			for(int i;i<120;i++)
			{
				Motor_Forward_Ration();
			}
			u1_TxData(First, sizeof(First)); // ���ŵ�һ��
			
        }
        else
        {
            OLED_ShowString(3, 1, "No card!");
        }
//		//OLED_ShowString(1, 1, "triggered!");
//        // ��ȡ������״̬
//        if(IR_CheckObstacle(IR_SENSOR_11)){
//            // ��⵽[1,1]��λ���ϰ���
//        }
//        
//        // �������ʾ��
//        for(uint8_t i=0; i<IR_SENSOR_COUNT; i++){
//            if(IR_Read(i)){
//                //printf("Sensor %d triggered!\r\n", i+1);
//                OLED_ShowString(1, 1, "triggered!");
//				OLED_ShowNum(1, 11, i, 2);
//            }
//        }
    }
	
	
}