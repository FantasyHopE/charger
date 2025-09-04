#ifndef __USER_CHARGER_H
#define __USER_CHARGER_H

#include "User_include.h"

#define CHARGE_24VOLTAGE        30.0             /* �������ߵ�ѹ */
#define CHARGE_24CURRENT        5.0             /* ����������� */

#define CHARGE_12VOLTAGE        15.1             /* �������ߵ�ѹ */
#define CHARGE_12CURRENT        10.0             /* ����������� */


#define CHARGE_6VOLTAGE         7.25             /* �������ߵ�ѹ */
#define CHARGE_6CURRENT         10.0             /* ����������� */

#define PWM_24V                 31.77            /* 24V��ѹϵ�� */
#define PWM_12V                 31.22            /* 12V��ѹϵ�� */


#define PWM_1A                  129           /* 1A����ϵ�� */
#define PWM_05A                 65            /* 0.5A����ϵ�� */

#define VOLTAGE_PWM_24V       (uint16_t)(CHARGE_24VOLTAGE * 31.55)     /* �������ߵ�ѹPWM��36.82Ϊϵ����1050 */
#define CC100PCT_PWM_VAL_24V  (uint16_t)(CHARGE_24CURRENT * 129)       /* �������ߵ���PWM��147Ϊϵ����1470   */

#define OUTOFFVOLTAGE_PWM_24V       (uint16_t)(33 * 33)     /* �������ߵ�ѹPWM��36.82Ϊϵ����1050 */
#define OUTOFFVOLTAGE_PWM_12V       (uint16_t)(CHARGE_12VOLTAGE+1 * 31.56)     /* �������ߵ�ѹPWM��36.82Ϊϵ����1050 */

#define VOLTAGE_PWM_12V       (uint16_t)(CHARGE_12VOLTAGE * PWM_12V)     /* �������ߵ�ѹPWM��36.82Ϊϵ����1050 */
#define CC100PCT_PWM_VAL_12V  (uint16_t)(CHARGE_12CURRENT * 131)       /* �������ߵ���PWM��147Ϊϵ����1470   */

#define Power_PWM_VAL         (uint16_t)(11 * 131)       /* �������ߵ���PWM��147Ϊϵ����1470   */

#define VOLTAGE_PWM_6V        (uint16_t)(CHARGE_6VOLTAGE * PWM_12V)     /* �������ߵ�ѹPWM��36.82Ϊϵ����1050 */

#define PWM_VAL_2A  (uint16_t)(2 * 147)        


typedef struct {
    float Battery_12V;      // 12V��ѹ
    float Battery_24V;      // 24V��ѹ
} BatteryConfig;

typedef struct {
    BatteryConfig Sealed;  // Ǧ����
    BatteryConfig Li_ion;  // ﮵��
    BatteryConfig AGM;     // 12V﮵�
} BatteryVoltageTable;



/********************** ���������ض��� **********************/
typedef enum
{
  WAIT_BATT  = 0x01,
  PREC       = 0x02,
	CCC1       = 0x03,        /* ����1�׶�    */
	CHG_FULL   = 0x04,        /* ��س���     */
  HC_NULL    = 0x05,        /* ��س���     */
}CHGType;

typedef struct
{
	CHGType Now;
	CHGType Past;
}sCHGType;



typedef enum {
    RISING,   // �����׶�
    FALLING   // �½��׶�
} State;

typedef struct {
    int currentValue; // ��ǰ��PWMֵ
    int targetValue;  // Ŀ��ֵ
    int step;         // ÿ�ε�������/���ٵĲ���
    State state;      // ��ǰ��״̬���������½�
} PWMControl;


extern sCHGType gChgType;
extern BatteryVoltageTable BatV;
/********************** �������� **********************/

void Sealed_mode_Charger(void);
void Start_stop_mode_Charger(void);
void Li_battery_mode_Charger(void);
void Repair_mode_Charger(void);
void Power_supply_mode_Charger(void);
void SIX_mode_Charger(void);
void ChargingProgram(void);
void ChargeStart(void);
void ChargeStart1(void);
void ChargeStop(void);
void PWMControl_Init(PWMControl *pwm, int targetValue, int step);
int PWMControl_Update(PWMControl *pwm);
void ControlPWM_Init(void);
uint16_t rampUpToTarget(uint16_t current, uint16_t target);
#endif 
