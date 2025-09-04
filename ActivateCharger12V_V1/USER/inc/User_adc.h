#ifndef __USER_ADC_H
#define __USER_ADC_H


/* ----- Includes -----*/

#include "User_include.h"


// ������ϲ���
#define A -0.035
#define B 95.75

#define VOLTAGE_CONVERSION_VALUE  2.341
#define CURRENT_CONVERSION_VALUE  0.589

/************************** �������� *************************/
uint16_t CurrentCalculate(uint16_t *pdata);
void User_ADC_Sample(void);
void User_adc_init(void);
uint16_t VoltageCalculate(uint16_t *pdata);
void VoltageSample(void); 
void CurrentSample(void);
void TemperatureSample(void);
float calculateTemperature(int adcValue);

/************************** �������� *************************/
extern uint8_t gFlagIrq;
extern uint16_t gBat_VoltageValue;      /* ��ص�ѹֵ       */
extern uint16_t gCurrentValue;    /* ������� */
extern uint16_t gAdcResult[10][5]; 
extern uint16_t gtemp_VoltageValue;      /* ��ص�ѹֵ       */
extern uint16_t gTemperature_ADValue[2];   /* �¶�ADֵ */
extern float gTemperature;           /* �¶� */
extern float    gTemperature_z;           /* �����¶� */
#endif 
