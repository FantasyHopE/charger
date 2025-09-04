#ifndef __USER_ADC_H
#define __USER_ADC_H


/* ----- Includes -----*/

#include "User_include.h"


// 线性拟合参数
#define A -0.035
#define B 95.75

#define VOLTAGE_CONVERSION_VALUE  2.341
#define CURRENT_CONVERSION_VALUE  0.589

/************************** 函数声明 *************************/
uint16_t CurrentCalculate(uint16_t *pdata);
void User_ADC_Sample(void);
void User_adc_init(void);
uint16_t VoltageCalculate(uint16_t *pdata);
void VoltageSample(void); 
void CurrentSample(void);
void TemperatureSample(void);
float calculateTemperature(int adcValue);

/************************** 变量声明 *************************/
extern uint8_t gFlagIrq;
extern uint16_t gBat_VoltageValue;      /* 电池电压值       */
extern uint16_t gCurrentValue;    /* 输出电流 */
extern uint16_t gAdcResult[10][5]; 
extern uint16_t gtemp_VoltageValue;      /* 电池电压值       */
extern uint16_t gTemperature_ADValue[2];   /* 温度AD值 */
extern float gTemperature;           /* 温度 */
extern float    gTemperature_z;           /* 主板温度 */
#endif 
