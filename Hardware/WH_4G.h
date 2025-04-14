#ifndef __WH_4G_H_
#define __WH_4G_H_
 
#include "stm32f10x.h"  //������Ҫ��ͷ�ļ�

#define SU03T_R_Buff_MAX_Length   20

typedef struct
{
	uint8_t R_Buff[SU03T_R_Buff_MAX_Length];  //���ջ�����
	uint8_t R_Length;     //���յ�����
	uint8_t R_Idle;       //���б�־λ
}SU03T;


void WH_4G_Config(void);
//void Wire4G_Anser03Data(void) ;
void Wire4G_yuzhiData(uint16_t *adder,uint16_t value) ;
void Wire4G_sendData(uint16_t adder,uint16_t value) ;
//void Wire4G_floatData(uint16_t *adder,float value) ;
#endif

