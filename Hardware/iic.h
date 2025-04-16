/*-------------------------------------------------*/
/*            超纬电子STM32系列开发板              */
/*-------------------------------------------------*/
/*                                                 */
/*            实现IIc总线功能的头文件              */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __IIC_H
#define __IIC_H

// 修改为PA11(SCL)和PA12(SDA)
#define SDA_IN()       {GPIOA->CRH&=0XFFF0FFFF;GPIOA->CRH|=0X00080000;GPIOA->BSRR=(1<<12);} // PA12设置为上拉输入模式
#define SDA_OUT()      {GPIOA->CRH&=0XFFF0FFFF;GPIOA->CRH|=0X00030000;}                    // PA12设置为推免输出模式
#define READ_SDA        GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12)       // 读取PA12(SDA) 
#define IIC_SDA_OUT(x)  GPIO_WriteBit(GPIOA, GPIO_Pin_12, (BitAction)x) // 设置PA12(SDA)电平 

#define IIC_SCL_H    GPIO_SetBits(GPIOA, GPIO_Pin_11)    // PA11(SCL)拉高
#define IIC_SDA_H    GPIO_SetBits(GPIOA, GPIO_Pin_12)    // PA12(SDA)拉高

#define IIC_SCL_L    GPIO_ResetBits(GPIOA, GPIO_Pin_11)  // PA11(SCL)拉低
#define IIC_SDA_L    GPIO_ResetBits(GPIOA, GPIO_Pin_12)  // PA12(SDA)拉低

void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
char IIC_Wait_Ack(void);
void IIC_Send_Byte(unsigned char);
unsigned char IIC_Read_Byte(unsigned char);

#endif