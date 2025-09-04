#ifndef __USER_LCD_H
#define __USER_LCD_H

#ifdef __cplusplus
extern "C"
{
#endif

/* ----- Includes -----*/



#include "User_include.h"

// �������ţ�����ʹ�� GPIO ģ��ͨ�ţ�
#define TM1621_CS_PIN    GPIO_PIN_3   //PA3
#define TM1621_WR_PIN    GPIO_PIN_1   //PB1
#define TM1621_DATA_PIN  GPIO_PIN_0   //PB0

// �͵�ƽ�͸ߵ�ƽ�Ĳ�����
#define TM1621_CS_LOW   GPIO_WritePin( CW_GPIOA, TM1621_CS_PIN ,GPIO_Pin_RESET)
#define TM1621_CS_HIGH  GPIO_WritePin(CW_GPIOA, TM1621_CS_PIN, GPIO_Pin_SET)

#define TM1621_WR_LOW   GPIO_WritePin(CW_GPIOB, TM1621_WR_PIN, GPIO_Pin_RESET)
#define TM1621_WR_HIGH  GPIO_WritePin(CW_GPIOB, TM1621_WR_PIN, GPIO_Pin_SET)

#define TM1621_DATA_LOW   GPIO_WritePin(CW_GPIOB, TM1621_DATA_PIN, GPIO_Pin_RESET)
#define TM1621_DATA_HIGH  GPIO_WritePin(CW_GPIOB, TM1621_DATA_PIN, GPIO_Pin_SET)


#define TM1621D_BYTES  23
#define REG_NO_UPDATE  0
#define REG_UPDATE     1

#define TM1621D_BIAS_COM2 0 	//AB=00 ��ѡ2�������� 
#define TM1621D_BIAS_COM3 1 	//AB=01 ��ѡ��3�������� 
#define TM1621D_BIAS_COM4 2 	//AB=10 ��ѡ4��������

#define TM1621D_BIAS_AB   TM1621D_BIAS_COM4

#if VER_HARDWIRE == VER1
	#define TM1621D_BIAS_C    1 //C = 0 ��ѡ1/2ƫѹ  C= 1 ��ѡ1/3ƫѹ (C=0 ȫ��ʾ C=1 ����ʾ)
#elif VER_HARDWIRE == VER2
	#define TM1621D_BIAS_C    1 //C = 0 ��ѡ1/2ƫѹ  C= 1 ��ѡ1/3ƫѹ (C=0 ȫ��ʾ C=1 ����ʾ)
#endif

#define TM1621D_BIAS	(0x20|( TM1621D_BIAS_AB << 2 )|TM1621D_BIAS_C)		//BIAS   0010 ABXC X    

#define TM1621D_LCDOFF  0x02		//�ر�LCD��ʾ
#define TM1621D_LCDON   0x03		//��LCD��ʾ

#define TM1621D_SYSDIS  0x00		//�ر�ϵͳ������ LCD ƫѹ������
#define TM1621D_SYSEN   0x01		//��ϵͳ����

#define READ             0x06
#define WRITE            0x05
#define READ_MODIFY_RITE 0x05
#define COMMAND          0x04


/*��������*/
void TM1621_Init(void);
uint8_t LcdInit( void );
void SendCommand(uint8_t u8Command);
void FillScreen( void );
void ClearScreen( void );
void SetSeg( uint8_t u8Addr,uint8_t u8Bit,uint8_t bOn );
void SendData(uint8_t u8Addr,uint8_t u8Data);
void SendLowBits(uint8_t u8Data,uint8_t u8Bit);
void SendHighBits(uint8_t u8Data,uint8_t u8Bit);
void SetSegs(uint8_t u8Addr, uint8_t u8BitMask, uint8_t bOn);
uint8_t tens_display(uint8_t num);
uint8_t units_display(uint8_t num1);
uint8_t decimal_display(uint8_t num2);
void display(uint16_t snum);
void Screen_refresh(void);
void BatteryStatus(void);
void NO1_8_OFF(void);
void NO2_8_OFF(void);
void NO3_8_OFF(void);


extern uint8_t B_V_FLAG,B_V_cnt;
#ifdef __cplusplus
}
#endif

#endif 
