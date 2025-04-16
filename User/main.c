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
#include "iic.h"

// 系统状态定义
typedef enum {
    MODE_NORMAL,
    MODE_REGISTER,    // 注册模式(PB13按下)
    MODE_UNREGISTER   // 注销模式(PB14按下)
} SystemMode;

// 停车场数据结构
#define MAX_CARDS 50
#define PARKING_RATE 1  // 每小时5元
#define EEPROM_CARD_START_ADDR 0x00  // IC卡数据在24C02中的起始地址

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

u8 g_showParkingStatus = 0;  // 是否显示车位状态标志
u32 g_statusDisplayTime = 0; // 车位状态显示开始时间

// 语音指令
const uint8_t WelcomeMsg[] = {0x7E, 0x04, 0x03, 0x00, 0X01, 0xEF};  // 欢迎语音
const uint8_t GoodbyeMsg[] = {0x7E, 0x04, 0x03, 0x00, 0X02, 0xEF};  // 离开语音
const uint8_t NeedRegister[] = {0x7E, 0x04, 0x03, 0x00, 0X03, 0xEF}; // 需要注册提示音
const uint8_t RegSuccess[] = {0x7E, 0x04, 0x03, 0x00, 0X04, 0xEF}; // 注册成功提示音
const uint8_t UnregSuccess[] = {0x7E, 0x04, 0x03, 0x00, 0X05, 0xEF}; // 注销成功提示音

// 函数声明
void UpdateDisplay(void);
void ProcessRegistration(u8* cardID);
void ProcessParking(u8* cardID);
void ProcessUnregistration(u8* cardID);
void HandleUnregisteredCard(void);
void HandleRegisteredEntry(u8 index);
void HandleRegisteredExit(u8 index);
void LoadRegisteredCardsFromEEPROM(void);
void SaveCardToEEPROM(u8 index);
void DeleteCardFromEEPROM(u8 index);

// 从24C02加载已注册的IC卡数据
void LoadRegisteredCardsFromEEPROM(void)
{
    u8 data[8]; // 8字节数据块
    
    g_registeredCount = 0;
    
    for(u8 i = 0; i < MAX_CARDS; i++) {
        AT24C02_ReadPage(EEPROM_CARD_START_ADDR + i*8, data);
        
        // 检查是否为有效卡数据(第一个字节不为0xFF)
        if(data[0] != 0xFF) {
            memcpy(g_registeredCards[g_registeredCount].cardID, data, 4);
            g_registeredCards[g_registeredCount].entryHour = data[4];
            g_registeredCards[g_registeredCount].entryMin = data[5];
            g_registeredCount++;
        } else {
			OLED_Clear();
			OLED_ShowString(1,1,"ERROR 24c02");
            break; // 遇到空数据，停止读取
        }
    }
}

// 保存单个IC卡数据到24C02
void SaveCardToEEPROM(u8 index)
{
    u8 data[8] = {0};
    
    memcpy(data, g_registeredCards[index].cardID, 4);
    data[4] = g_registeredCards[index].entryHour;
    data[5] = g_registeredCards[index].entryMin;
    
    AT24C02_WritePage(EEPROM_CARD_START_ADDR + index*8, data);
}

// 删除EEPROM中的IC卡数据
void DeleteCardFromEEPROM(u8 index)
{
    u8 emptyData[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    AT24C02_WritePage(EEPROM_CARD_START_ADDR + index*8, emptyData);
}

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
    IIC_Init();
    
    // 从EEPROM加载已注册的IC卡
    LoadRegisteredCardsFromEEPROM();
    
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

        // 按键检测
        Key_Value key = Key_Scan();
        if(key == KEY1_PRESSED) {
            g_mode = MODE_REGISTER;
			OLED_Clear();
			UpdateDisplay();
            OLED_ShowString(4,1,"[Reg Mode]");
            delay_ms(500);
        }
        else if(key == KEY2_PRESSED) {
            g_mode = MODE_UNREGISTER;
			OLED_Clear();
			UpdateDisplay();
            OLED_ShowString(4,1,"[Unreg Mode]");
            delay_ms(500);
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
            
            delay_ms(1000);
            g_mode = MODE_NORMAL;
        }
    }
}

// 处理未注册卡
void HandleUnregisteredCard(void)
{
    u1_TxData(NeedRegister, sizeof(NeedRegister));
    
    OLED_Clear();
    OLED_ShowString(1, 1, "Unregister Card");
    OLED_ShowString(2, 1, "Please register");
    
    delay_ms(3000);
    UpdateDisplay();
}

// 处理已注册卡入场
void HandleRegisteredEntry(u8 index)
{
    if(g_availableSpots > 0) {
        g_registeredCards[index].entryHour = TimeData.hour;
        g_registeredCards[index].entryMin = TimeData.minute;
        g_availableSpots--;
        
        // 更新EEPROM
        SaveCardToEEPROM(index);
        
        // OLED_Clear();
        // OLED_ShowString(1, 1, "Welcome!");
        // OLED_ShowString(2, 1, "Entry Time:");
        // OLED_ShowNum(3, 1, TimeData.hour, 2);
        // OLED_ShowString(3, 3, ":");
        // OLED_ShowNum(3, 4, TimeData.minute, 2);  
        g_showParkingStatus = 1;
        UpdateDisplay();
        
        Motor_Ctrl_Direction_Angle(1, 1, 90);
        u1_TxData(WelcomeMsg, sizeof(WelcomeMsg));
        
        delay_ms(3000);
        Motor_Ctrl_Direction_Angle(1, 0, 90);
        
        UpdateDisplay();
    } else {
        OLED_Clear();
        OLED_ShowString(1, 1, "Parking Full");
        OLED_ShowString(2, 1, "Please wait");
        delay_ms(2000);
        UpdateDisplay();
    }
}

// 处理已注册卡出场
void HandleRegisteredExit(u8 index)
{
    u8 hours = TimeData.hour - g_registeredCards[index].entryHour;
    u8 mins = TimeData.minute - g_registeredCards[index].entryMin;
    float fee = mins * PARKING_RATE;
    
    OLED_Clear();
    OLED_ShowString(1, 1, "Exit Process");
    OLED_ShowString(2, 1, "Parking Time:");
    OLED_ShowNum(3, 1, hours, 2);
    OLED_ShowString(3, 3, "h");
    OLED_ShowNum(3, 4, mins, 2);
    OLED_ShowString(3, 6, "m");
    
    OLED_ShowString(4, 1, "Fee: ");
    OLED_ShowNum(4, 6, fee, 4);
    OLED_ShowString(4, 10, "yuan");
    
    Motor_Ctrl_Direction_Angle(2, 1, 90);
    u1_TxData(GoodbyeMsg, sizeof(GoodbyeMsg));
    
    g_registeredCards[index].entryHour = 0xFF;
    g_registeredCards[index].entryMin = 0xFF;
    g_availableSpots++;
    
    // 更新EEPROM
    SaveCardToEEPROM(index);
    
    delay_ms(3000);
    Motor_Ctrl_Direction_Angle(2, 0, 90);
    
    UpdateDisplay();
}

// 处理车辆入场/出场
void ProcessParking(u8* cardID) 
{
    u8 cardStatus = 0; // 0=未注册 1=已注册未入场 2=已注册已入场
    u8 cardIndex = 0;
    
    for(u8 i = 0; i < g_registeredCount; i++) {
        if(memcmp(cardID, g_registeredCards[i].cardID, 4) == 0) {
            if(g_registeredCards[i].entryHour != 0xFF) {
                cardStatus = 2;
            } else {
                cardStatus = 1;
            }
            cardIndex = i;
            break;
        }
    }

    switch(cardStatus) {
        case 0:
            HandleUnregisteredCard();
            break;
        case 1:
            HandleRegisteredEntry(cardIndex);
            break;
        case 2:
            HandleRegisteredExit(cardIndex);
            break;
    }
}

// 处理刷卡注册
void ProcessRegistration(u8* cardID)
{
    if(g_registeredCount < MAX_CARDS) {
        memcpy(g_registeredCards[g_registeredCount].cardID, cardID, 4);
        g_registeredCards[g_registeredCount].entryHour = 0xFF;
        g_registeredCards[g_registeredCount].entryMin = 0xFF;
        
        // 保存到EEPROM
        SaveCardToEEPROM(g_registeredCount);
        
        g_registeredCount++;
        u1_TxData(RegSuccess, sizeof(RegSuccess));
        OLED_ShowString(4,1,"Reg Success!");
		delay_ms(1000);
		UpdateDisplay();
    } else {
        OLED_ShowString(4,1,"System Full!");
    }
}

// 处理注销
void ProcessUnregistration(u8* cardID)
{
    for(u8 i=0; i<g_registeredCount; i++) {
        if(memcmp(cardID, g_registeredCards[i].cardID, 4) == 0) {
            // 从EEPROM删除
            DeleteCardFromEEPROM(i);
            
            // 从内存数组中移除
            for(u8 j=i; j<g_registeredCount-1; j++) {
                memcpy(&g_registeredCards[j], &g_registeredCards[j+1], sizeof(ParkingRecord));
                SaveCardToEEPROM(j);
            }
            
            g_registeredCount--;
            u1_TxData(UnregSuccess, sizeof(UnregSuccess));
            OLED_ShowString(4,1,"Unreg Success!");
			delay_ms(1000);
			UpdateDisplay();
            return;
        }
    }
    OLED_ShowString(4,1,"Card Not Found!");
}

// 更新显示内容
void UpdateDisplay(void)
{
    OLED_Clear();
    
    if(g_showParkingStatus) {
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
        OLED_ShowString(2,3,":");
        OLED_ShowNum(2,4,TimeData.minute,2);
        OLED_ShowNum(2,1,TimeData.hour,2);		
        OLED_ShowNum(1,1,TimeData.year,4);
        OLED_ShowNum(1,6,TimeData.month,2);
        OLED_ShowNum(1,9,TimeData.day,2);
        OLED_ShowString(1,5,"-");
        OLED_ShowString(1,8,"-");
        
        OLED_ShowString(3,1,"Spots:");
        OLED_ShowNum(3,7,g_availableSpots,2);
    }
}

