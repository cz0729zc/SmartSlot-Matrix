/*-------------------------------------------------*/
/*            ��γ����STM32ϵ�п�����              */
/*-------------------------------------------------*/
/*                                                 */
/*            ʵ��IIc���߹��ܵ�ͷ�ļ�              */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __IIC_H
#define __IIC_H

// �޸�ΪPA11(SCL)��PA12(SDA)
#define SDA_IN()       {GPIOA->CRH&=0XFFF0FFFF;GPIOA->CRH|=0X00080000;GPIOA->BSRR=(1<<12);} // PA12����Ϊ��������ģʽ
#define SDA_OUT()      {GPIOA->CRH&=0XFFF0FFFF;GPIOA->CRH|=0X00030000;}                    // PA12����Ϊ�������ģʽ
#define READ_SDA        GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12)       // ��ȡPA12(SDA) 
#define IIC_SDA_OUT(x)  GPIO_WriteBit(GPIOA, GPIO_Pin_12, (BitAction)x) // ����PA12(SDA)��ƽ 

#define IIC_SCL_H    GPIO_SetBits(GPIOA, GPIO_Pin_11)    // PA11(SCL)����
#define IIC_SDA_H    GPIO_SetBits(GPIOA, GPIO_Pin_12)    // PA12(SDA)����

#define IIC_SCL_L    GPIO_ResetBits(GPIOA, GPIO_Pin_11)  // PA11(SCL)����
#define IIC_SDA_L    GPIO_ResetBits(GPIOA, GPIO_Pin_12)  // PA12(SDA)����

void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
char IIC_Wait_Ack(void);
void IIC_Send_Byte(unsigned char);
unsigned char IIC_Read_Byte(unsigned char);

#endif