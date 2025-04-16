#include "stm32f10x.h"
#include "delay.h"
#include "usart1.h"
#include "ir_sensor.h"
#include "RC522.h"
#include "OLED.h"
#include "beep.h"
#include "ds1302.h"
#include "motor.h"
#include "24c02.h"
#include "keys.h"

// ϵͳ״̬����
typedef enum {
    MODE_NORMAL,
    MODE_REGISTER
} SystemMode;

// ͣ�������ݽṹ
#define MAX_CARDS 50
#define PARKING_RATE 1  // ÿСʱ5Ԫ

typedef struct {
    u8 cardID[4];
    u8 entryHour;
    u8 entryMin;
} ParkingRecord;

// ȫ�ֱ���
SystemMode g_mode = MODE_NORMAL;
ParkingRecord g_registeredCards[MAX_CARDS];
u8 g_registeredCount = 0;
u8 g_availableSpots = 4;  // ��ʼ��λ����
u8 g_currentMinute = 0;
u8 g_lastOccupied = 0;  // ��¼��һ�μ�⵽����ռ��λ��

// ����ָ��
const uint8_t WelcomeMsg[] = {0x7E, 0x04, 0x03, 0x00, 0X01, 0xEF};  // ��ӭ����
const uint8_t GoodbyeMsg[] = {0x7E, 0x04, 0x03, 0x00, 0X02, 0xEF};  // �뿪����
const uint8_t RegSuccess[] = {0x7E, 0x04, 0x03, 0x00, 0X03, 0xEF}; // ע��ɹ���ʾ��

// ��������
void UpdateDisplay(void);
void ProcessRegistration(u8* cardID);
void ProcessParking(u8* cardID);
void CalculateFee(u8 entryHour, u8 entryMin);

int main(void)
{
    // ��ʼ������
    IR_Init();
    OLED_Init();
    BEEP_Init();
    Moto_Init();
    Usart1_Init(9600);
    ds1302_gpio_init();
    ds1032_init();
    Key_Init();
    // ��ȡ��ʼʱ��
    ds1032_read_realTime();
    g_currentMinute = TimeData.minute;
    
    // ��ʾ��ʼ������
    OLED_Clear();
    UpdateDisplay();

    while(1) {
        // ʱ����´���
        ds1032_read_realTime();
        if(g_currentMinute != TimeData.minute) {
            g_currentMinute = TimeData.minute;
            UpdateDisplay();
        }

		// ��λ��⣨ʹ��4�����⴫������
		u8 occupied = 0;
		for(u8 i=0; i<4; i++) {
			if(IR_CheckObstacle(i)) occupied++;
		}
		
		// ֻ�е���λ״̬�仯ʱ�Ÿ���
		if(occupied != g_lastOccupied) {
			g_availableSpots = 4 - occupied;
			g_lastOccupied = occupied;
			UpdateDisplay();
		}

        // RFID���
        u8 status, cardID[4];
        RC522_Init(Card1);
        status = PcdRequest(PICC_REQALL, cardID);
        status = PcdAnticoll(cardID);
        
        if(status == MI_OK) {
            BEEP_Alarm(1);
            delay_ms(50);
            BEEP_Alarm(0);
            
            if(g_mode == MODE_REGISTER) {
                ProcessRegistration(cardID);
            } else {
                ProcessParking(cardID);
            }
            
            delay_ms(1000); // ������
        }
    }
}

// ������ʾ����
void UpdateDisplay(void)
{
	//��ʾʱ��
	OLED_Clear();
	OLED_ShowString(2,3,":");
	OLED_ShowNum(2,4,TimeData.minute,2);
	OLED_ShowNum(2,1,TimeData.hour,2);		
	OLED_ShowNum(1,1,TimeData.year,4);
	OLED_ShowNum(1,6,TimeData.month,2);
	OLED_ShowNum(1,9,TimeData.day,2);
	OLED_ShowString(1,5,"-");
	OLED_ShowString(1,8,"-");
    
    // �ڶ�����ʾ��λ��Ϣ
    OLED_ShowString(3,1,"Spots:");
    OLED_ShowNum(3,7,g_availableSpots,2);
    
//    // ��������ʾϵͳ״̬
//    if(g_mode == MODE_REGISTER) {
//        OLED_ShowString(3,1,"[Reg Mode]");
//    } else {
//        OLED_ShowString(3,1,"[Normal Mode]");
//    }
}

// �������볡/����
void ProcessParking(u8* cardID)
{
    // ����Ƿ�ע�Ῠ
    u8 isRegistered = 0;
    for(u8 i=0; i<g_registeredCount; i++) {
        if(memcmp(cardID, g_registeredCards[i].cardID, 4) == 0) {
            isRegistered = 1;
            // ����ͣ��ʱ��
            u8 hours = TimeData.hour - g_registeredCards[i].entryHour;
            u8 mins = TimeData.minute - g_registeredCards[i].entryMin;
            float fee = mins*PARKING_RATE;
            
            // ��ʾ�ɷ���Ϣ
            OLED_ShowString(4,1,"Fee:");
            OLED_ShowNum(4,5,fee,4);
            //OLED_ShowString(4,1,"Press PB13 to pay");
            
            // �ȴ�PB13����
            while(Key_Scan() != KEY_PRESSED) {
                delay_ms(10);
            }
            
            // ��բ����
            Motor_Ctrl_Direction_Angle(1,90);
            u1_TxData(GoodbyeMsg, sizeof(GoodbyeMsg));
			delay_ms(3000);
            Motor_Ctrl_Direction_Angle(0,90);
			
            // ��ע���б����Ƴ�
            for(u8 j=i; j<g_registeredCount-1; j++) {
                memcpy(&g_registeredCards[j], &g_registeredCards[j+1], sizeof(ParkingRecord));
            }
            g_registeredCount--;
            g_availableSpots++;
            
            UpdateDisplay();
            return;
        }
    }
    
    if(!isRegistered) {
        // �³����볡
        if(g_availableSpots > 0) {
            memcpy(g_registeredCards[g_registeredCount].cardID, cardID, 4);
            g_registeredCards[g_registeredCount].entryHour = TimeData.hour;
            g_registeredCards[g_registeredCount].entryMin = TimeData.minute;
            g_registeredCount++;
            
            Motor_Ctrl_Direction_Angle(1,90);
            u1_TxData(WelcomeMsg, sizeof(WelcomeMsg));
			delay_ms(3000);
			Motor_Ctrl_Direction_Angle(0,90);
            //g_availableSpots--;
            UpdateDisplay();
        } else {
            OLED_ShowString(4,1,"Parking Full!");
        }
    }
}

// ����ˢ��ע��
void ProcessRegistration(u8* cardID)
{
    if(g_registeredCount < MAX_CARDS) {
        memcpy(g_registeredCards[g_registeredCount].cardID, cardID, 4);
        g_registeredCount++;
        u1_TxData(RegSuccess, sizeof(RegSuccess));
        OLED_ShowString(4,1,"Reg Success!");
    }
}

