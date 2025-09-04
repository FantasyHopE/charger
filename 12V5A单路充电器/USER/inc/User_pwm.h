#ifndef __USER_PWM_H
#define __USER_PWM_H

#ifdef __cplusplus
extern "C"
{
#endif

/* ----- Includes -----*/



#include "User_include.h"

typedef enum
{
  Lights  = 0x00, //关灯
	Flicker = 0x01, //闪烁
  Always  = 0x02  //常亮
}LED_State;


typedef struct
{
	LED_State LED_30;
	LED_State LED_60;
  LED_State LED_100;
}sLED_State;



// 定义占空比最大值、最小值和步长
#define PWM_MAX 3000
#define PWM_MIN 100
#define PWM_STEP 200

extern sLED_State StatusLEDs; //呼吸灯状态



/*函数声明*/
void GPIO_Configuration(void);
void PWM_OutputConfig(void);
void SetChargeCurrent(uint16_t data);
void Breathing_Light_Update(void);

#ifdef __cplusplus
}
#endif

#endif 
