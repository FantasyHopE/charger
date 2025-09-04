#ifndef __USER_PWM_H
#define __USER_PWM_H

#ifdef __cplusplus
extern "C"
{
#endif

/* ----- Includes -----*/



#include "User_include.h"



/*º¯ÊıÉùÃ÷*/
void GPIO_Configuration(void);
void PWM_OutputConfig(void);
void SetChargeCurrent(uint16_t data);
void SetChargerVoltage(uint16_t data);
uint16_t CurrentControl(uint16_t data);
void SetChargerVoltage1(uint16_t data);

#ifdef __cplusplus
}
#endif

#endif 
