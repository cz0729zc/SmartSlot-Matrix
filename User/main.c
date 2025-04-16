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
    MODE_REGISTER,    // ע��ģʽ(PB13����)
    MODE_UNREGISTER   // ע��ģʽ(PB14����)
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

u8 g_showParkingStatus = 0;  // �Ƿ���ʾ��λ״̬��־
u32 g_statusDisplayTime = 0; // ��λ״̬��ʾ��ʼʱ��

// ����ָ��
//7E 04 03 00 01 EF
const uint8_t WelcomeMsg[] = {0x7E, 0x04, 0x03, 0x00, 0X01, 0xEF};  // ��ӭ����
const uint8_t GoodbyeMsg[] = {0x7E, 0x04, 0x03, 0x00, 0X02, 0xEF};  // �뿪����
const uint8_t NeedRegister[] = {0x7E, 0x04, 0x03, 0x00, 0X03, 0xEF}; // ��Ҫע����ʾ��
const uint8_t RegSuccess[] = {0x7E, 0x04, 0x03, 0x00, 0X04, 0xEF}; // ע��ɹ���ʾ��
const uint8_t UnregSuccess[] = {0x7E, 0x04, 0x03, 0x00, 0X05, 0xEF}; // ע���ɹ���ʾ��

// ��������
void UpdateDisplay(void);
void ProcessRegistration(u8* cardID);
void ProcessParking(u8* cardID);
void CalculateFee(u8 entryHour, u8 entryMin);
void ProcessUnregistration(u8* cardID);

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
	
//	u1_TxData(WelcomeMsg, sizeof(WelcomeMsg));
//    delay_ms(500); // ���Ż�ӭ����
//    u1_TxData(RegSuccess, sizeof(RegSuccess));
//    delay_ms(500); // ����ע��ɹ�����
//    u1_TxData(UnregSuccess, sizeof(UnregSuccess));
//    delay_ms(500); // ����ע���ɹ�����
//    u1_TxData(GoodbyeMsg, sizeof(GoodbyeMsg));
//    delay_ms(500); // �����뿪����
//    u1_TxData(NeedRegister, sizeof(NeedRegister));
//    delay_ms(500); // ������Ҫע������

    while(1) {
        // ʱ����´���
        ds1032_read_realTime();
        if(g_currentMinute != TimeData.minute) {
            g_currentMinute = TimeData.minute;
            UpdateDisplay();
        }

//		// ��λ��⣨ʹ��4�����⴫������
//		u8 occupied = 0;
//		for(u8 i=0; i<4; i++) {
//			if(IR_CheckObstacle(i)) occupied++;
//		}
//		
//		// ֻ�е���λ״̬�仯ʱ�Ÿ���
//		if(occupied != g_lastOccupied) {
//			g_availableSpots = 4 - occupied;
//			g_lastOccupied = occupied;
//			UpdateDisplay();
//		}

		
		// �������
		Key_Value key = Key_Scan();
		if(key == KEY1_PRESSED) {
			g_mode = MODE_REGISTER;
			OLED_ShowString(4,1,"[Reg Mode]");
			delay_ms(500); // ������
		}
		else if(key == KEY2_PRESSED) {
			g_mode = MODE_UNREGISTER;
			OLED_ShowString(4,1,"[Unreg Mode]");
			delay_ms(500); // ������
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
            
			switch(g_mode) {
				case MODE_REGISTER:
					ProcessRegistration(cardID);
					break;
				case MODE_UNREGISTER:
					ProcessUnregistration(cardID);
					break;
                case MODE_NORMAL:
					ProcessParking(cardID);
			}
			
			delay_ms(1000); // ������
			g_mode = MODE_NORMAL; // ������ͨģʽ
        }
    }
}


// ע������
void ProcessUnregistration(u8* cardID) {
    for(u8 i=0; i<g_registeredCount; i++) {
        if(memcmp(cardID, g_registeredCards[i].cardID, 4) == 0) {
            // ��ע���б����Ƴ�
            for(u8 j=i; j<g_registeredCount-1; j++) {
                memcpy(&g_registeredCards[j], &g_registeredCards[j+1], sizeof(ParkingRecord));
            }
            g_registeredCount--;
            u1_TxData(UnregSuccess, sizeof(UnregSuccess));
            OLED_ShowString(4,1,"Unreg Success!");
            return;
        }
    }
    OLED_ShowString(4,1,"Card Not Found!");
}


// ������ʾ����
void UpdateDisplay(void)
{
    OLED_Clear();
    
    if(g_showParkingStatus) {
        // ��ʾ��λ״̬����
        //OLED_ShowString(1,1,"Parking Status");
        
        // ��ʾ4����λ״̬
        OLED_ShowString(1,1,"(1,1):");
        OLED_ShowString(1,7,IR_CheckObstacle(0) ? "Occupied" : "Free");
        
        OLED_ShowString(2,1,"(1,2):");
        OLED_ShowString(2,7,IR_CheckObstacle(1) ? "Occupied" : "Free");
        
        OLED_ShowString(3,1,"(2,1):");
        OLED_ShowString(3,7,IR_CheckObstacle(2) ? "Occupied" : "Free");
        
        OLED_ShowString(4,1,"(2,2):");
        OLED_ShowString(4,7,IR_CheckObstacle(3) ? "Occupied" : "Free");
		
		delay_ms(3000);
		g_showParkingStatus = 0;
		UpdateDisplay();
    }
    else {
        // ԭ����������ʾ
        //��ʾʱ��
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
    }
}

// ����δע�Ῠ
void HandleUnregisteredCard(void)
{
    u1_TxData(NeedRegister, sizeof(NeedRegister));  // ������Ҫע������
    
    OLED_Clear();
    OLED_ShowString(1, 1, "Unregister Card");
    OLED_ShowString(2, 1, "Please register");
    
    delay_ms(3000); // ��ʾ3��
    UpdateDisplay(); // ����������
}

// ������ע�Ῠ�볡
void HandleRegisteredEntry(u8 index)
{
    if(g_availableSpots > 0) {
        // ��¼�볡ʱ��
        g_registeredCards[index].entryHour = TimeData.hour;
        g_registeredCards[index].entryMin = TimeData.minute;
        g_availableSpots--;
		
        // ��ʾ�볡��Ϣ
        OLED_Clear();
        OLED_ShowString(1, 1, "Welcome!");
        OLED_ShowString(2, 1, "Entry Time:");
        OLED_ShowNum(3, 1, TimeData.hour, 2);
        OLED_ShowString(3, 3, ":");
        OLED_ShowNum(3, 4, TimeData.minute, 2);  
		
        // ��բ����
        Motor_Ctrl_Direction_Angle(1, 1, 90);  // ��բ
        u1_TxData(WelcomeMsg, sizeof(WelcomeMsg));  // ���Ż�ӭ����
        
        delay_ms(3000);  // ����բ������3��
        Motor_Ctrl_Direction_Angle(1, 0, 90);  // ��բ
        
        UpdateDisplay();
    } else {
        // ��λ����
        OLED_Clear();
        OLED_ShowString(1, 1, "Parking Full");
        OLED_ShowString(2, 1, "Please wait");
        delay_ms(2000);
        UpdateDisplay();
    }
}

// ������ע�Ῠ����
void HandleRegisteredExit(u8 index)
{
    // ����ͣ��ʱ��
    u8 hours = TimeData.hour - g_registeredCards[index].entryHour;
    u8 mins = TimeData.minute - g_registeredCards[index].entryMin;
    float fee = mins * PARKING_RATE;
    
    // ��ʾ�ɷ���Ϣ
    OLED_Clear();
    OLED_ShowString(1, 1, "Exit Process");
    OLED_ShowString(2, 1, "Parking Time:");
    OLED_ShowNum(3, 1, hours, 2);
    OLED_ShowString(3, 3, "h");
    OLED_ShowNum(3, 4, mins, 2);
    OLED_ShowString(3, 6, "m");
    
    OLED_ShowString(4, 1, "Fee: ");
    OLED_ShowNum(4, 6, fee, 4);
    OLED_ShowString(4, 9, "yuan");
    
    // ��բ����
    Motor_Ctrl_Direction_Angle(2, 1, 90);  // ��բ
    u1_TxData(GoodbyeMsg, sizeof(GoodbyeMsg));  // �����뿪����
    
    // �����볡��¼(0xFF��ʾδ�볡)
    g_registeredCards[index].entryHour = 0xFF;
    g_registeredCards[index].entryMin = 0xFF;
    g_availableSpots++;
    
    delay_ms(3000);  // ����բ������3��
    Motor_Ctrl_Direction_Angle(2, 0, 90);  // ��բ
    
    UpdateDisplay();
}


// �������볡/����
void ProcessParking(u8* cardID) 
{
    // ��鿨Ƭ״̬
    u8 cardStatus = 0; // 0=δע�� 1=��ע��δ�볡 2=��ע�����볡
    u8 cardIndex = 0;
    
    // ������ע�ῨƬ����
    for(u8 i = 0; i < g_registeredCount; i++) {
        if(memcmp(cardID, g_registeredCards[i].cardID, 4) == 0) {
            // ����Ƿ����볡��¼
            if(g_registeredCards[i].entryHour != 0xFF) { // 0xFF��ʾδ�볡
                cardStatus = 2; // ���볡
            } else {
                cardStatus = 1; // ��ע��δ�볡
            }
            cardIndex = i;
            break;
        }
    }

    switch(cardStatus) {
        case 0: // δע�Ῠ
            HandleUnregisteredCard();
            break;
            
        case 1: // ��ע��δ�볡
            HandleRegisteredEntry(cardIndex);
            break;
            
        case 2: // ��ע�����볡
            HandleRegisteredExit(cardIndex);
            break;
    }
}

// ����ˢ��ע��
void ProcessRegistration(u8* cardID)
{
    if(g_registeredCount < MAX_CARDS) {
        memcpy(g_registeredCards[g_registeredCount].cardID, cardID, 4);
		
		// �볡��¼(0xFF��ʾδ�볡)
		g_registeredCards[g_registeredCount].entryHour = 0xFF;
		g_registeredCards[g_registeredCount].entryMin = 0xFF;
		
        g_registeredCount++;
        u1_TxData(RegSuccess, sizeof(RegSuccess));
        OLED_ShowString(4,1,"Reg Success!");
    }
}

