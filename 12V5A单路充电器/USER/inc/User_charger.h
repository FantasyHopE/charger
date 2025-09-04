#ifndef __USER_CHARGER_H
#define __USER_CHARGER_H

#include "User_include.h"

#define PWM_VAL_5A  600 //1358     

#define PWM_VAL_2A  130  
/********************** ���������ض��� **********************/
typedef enum
{
  WAIT_BACK  = 0x00,
  WAIT_BATT  = 0x01,
	CCC1       = 0x02,        /* ����1�׶�    */
	CHG_FULL   = 0x03         /* ��س���     */
}CHGType;

typedef struct
{
	CHGType Now;
	CHGType Past;
}sCHGType;


  
extern sCHGType gChgType;

/********************** �������� **********************/

void ChargingProgram(void);
void ChargeStart(void);
void ChargeStop(void);
void SIXV_Charger(void);
void lithium_battery_Charger(void);
void AGM_Charger(void);
void Sealed_Charger(void);
uint16_t get_compensated_voltage(uint8_t temp);
uint16_t lithium_temp_compensation(int8_t temp);



#endif 
