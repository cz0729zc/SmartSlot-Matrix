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

// 系统状态定义
typedef enum {
    MODE_NORMAL,
    MODE_REGISTER
} SystemMode;

// 停车场数据结构
#define MAX_CARDS 50
#define PARKING_RATE 1  // 每小时5元

typedef struct {
    u8 cardID[4];
    u8 entryHour;
    u8 entryMin;
} ParkingRecord;

// 全局变量
SystemMode g_mode = MODE_NORMAL;
ParkingRecord g_registeredCards[MAX_CARDS];
u8 g_registeredCount = 0;
u8 g_availableSpots = 4;  // 初始车位总数
u8 g_currentMinute = 0;
u8 g_lastOccupied = 0;  // 记录上一次检测到的已占车位数

// 语音指令
const uint8_t WelcomeMsg[] = {0x7E, 0x04, 0x03, 0x00, 0X01, 0xEF};  // 欢迎语音
const uint8_t GoodbyeMsg[] = {0x7E, 0x04, 0x03, 0x00, 0X02, 0xEF};  // 离开语音
const uint8_t RegSuccess[] = {0x7E, 0x04, 0x03, 0x00, 0X03, 0xEF}; // 注册成功提示音

// 函数声明
void UpdateDisplay(void);
void ProcessRegistration(u8* cardID);
void ProcessParking(u8* cardID);
void CalculateFee(u8 entryHour, u8 entryMin);

int main(void)
{
    // 初始化外设
    IR_Init();
    OLED_Init();
    BEEP_Init();
    Moto_Init();
    Usart1_Init(9600);
    ds1302_gpio_init();
    ds1032_init();
    Key_Init();
    // 读取初始时间
    ds1032_read_realTime();
    g_currentMinute = TimeData.minute;
    
    // 显示初始化界面
    OLED_Clear();
    UpdateDisplay();

    while(1) {
        // 时间更新处理
        ds1032_read_realTime();
        if(g_currentMinute != TimeData.minute) {
            g_currentMinute = TimeData.minute;
            UpdateDisplay();
        }

		// 车位检测（使用4个红外传感器）
		u8 occupied = 0;
		for(u8 i=0; i<4; i++) {
			if(IR_CheckObstacle(i)) occupied++;
		}
		
		// 只有当车位状态变化时才更新
		if(occupied != g_lastOccupied) {
			g_availableSpots = 4 - occupied;
			g_lastOccupied = occupied;
			UpdateDisplay();
		}

        // RFID检测
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
            
            delay_ms(1000); // 防抖动
        }
    }
}

// 更新显示内容
void UpdateDisplay(void)
{
	//显示时间
	OLED_Clear();
	OLED_ShowString(2,3,":");
	OLED_ShowNum(2,4,TimeData.minute,2);
	OLED_ShowNum(2,1,TimeData.hour,2);		
	OLED_ShowNum(1,1,TimeData.year,4);
	OLED_ShowNum(1,6,TimeData.month,2);
	OLED_ShowNum(1,9,TimeData.day,2);
	OLED_ShowString(1,5,"-");
	OLED_ShowString(1,8,"-");
    
    // 第二行显示车位信息
    OLED_ShowString(3,1,"Spots:");
    OLED_ShowNum(3,7,g_availableSpots,2);
    
//    // 第三行显示系统状态
//    if(g_mode == MODE_REGISTER) {
//        OLED_ShowString(3,1,"[Reg Mode]");
//    } else {
//        OLED_ShowString(3,1,"[Normal Mode]");
//    }
}

// 处理车辆入场/出场
void ProcessParking(u8* cardID)
{
    // 检查是否注册卡
    u8 isRegistered = 0;
    for(u8 i=0; i<g_registeredCount; i++) {
        if(memcmp(cardID, g_registeredCards[i].cardID, 4) == 0) {
            isRegistered = 1;
            // 计算停留时间
            u8 hours = TimeData.hour - g_registeredCards[i].entryHour;
            u8 mins = TimeData.minute - g_registeredCards[i].entryMin;
            float fee = mins*PARKING_RATE;
            
            // 显示缴费信息
            OLED_ShowString(4,1,"Fee:");
            OLED_ShowNum(4,5,fee,4);
            //OLED_ShowString(4,1,"Press PB13 to pay");
            
            // 等待PB13按下
            while(Key_Scan() != KEY_PRESSED) {
                delay_ms(10);
            }
            
            // 开闸放行
            Motor_Ctrl_Direction_Angle(1,90);
            u1_TxData(GoodbyeMsg, sizeof(GoodbyeMsg));
			delay_ms(3000);
            Motor_Ctrl_Direction_Angle(0,90);
			
            // 从注册列表中移除
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
        // 新车辆入场
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

// 处理刷卡注册
void ProcessRegistration(u8* cardID)
{
    if(g_registeredCount < MAX_CARDS) {
        memcpy(g_registeredCards[g_registeredCount].cardID, cardID, 4);
        g_registeredCount++;
        u1_TxData(RegSuccess, sizeof(RegSuccess));
        OLED_ShowString(4,1,"Reg Success!");
    }
}

