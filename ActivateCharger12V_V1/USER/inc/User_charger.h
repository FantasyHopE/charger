#ifndef __USER_CHARGER_H
#define __USER_CHARGER_H

#include "User_include.h"

#define CHARGE_24VOLTAGE        30.0             /* 充电器最高电压 */
#define CHARGE_24CURRENT        5.0             /* 充电器最大电流 */

#define CHARGE_12VOLTAGE        15.1             /* 充电器最高电压 */
#define CHARGE_12CURRENT        10.0             /* 充电器最大电流 */


#define CHARGE_6VOLTAGE         7.25             /* 充电器最高电压 */
#define CHARGE_6CURRENT         10.0             /* 充电器最大电流 */

#define PWM_24V                 31.77            /* 24V电压系数 */
#define PWM_12V                 31.22            /* 12V电压系数 */


#define PWM_1A                  129           /* 1A电流系数 */
#define PWM_05A                 65            /* 0.5A电流系数 */

#define VOLTAGE_PWM_24V       (uint16_t)(CHARGE_24VOLTAGE * 31.55)     /* 充电器最高电压PWM，36.82为系数，1050 */
#define CC100PCT_PWM_VAL_24V  (uint16_t)(CHARGE_24CURRENT * 129)       /* 充电器最高电流PWM，147为系数，1470   */

#define OUTOFFVOLTAGE_PWM_24V       (uint16_t)(33 * 33)     /* 充电器最高电压PWM，36.82为系数，1050 */
#define OUTOFFVOLTAGE_PWM_12V       (uint16_t)(CHARGE_12VOLTAGE+1 * 31.56)     /* 充电器最高电压PWM，36.82为系数，1050 */

#define VOLTAGE_PWM_12V       (uint16_t)(CHARGE_12VOLTAGE * PWM_12V)     /* 充电器最高电压PWM，36.82为系数，1050 */
#define CC100PCT_PWM_VAL_12V  (uint16_t)(CHARGE_12CURRENT * 131)       /* 充电器最高电流PWM，147为系数，1470   */

#define Power_PWM_VAL         (uint16_t)(11 * 131)       /* 充电器最高电流PWM，147为系数，1470   */

#define VOLTAGE_PWM_6V        (uint16_t)(CHARGE_6VOLTAGE * PWM_12V)     /* 充电器最高电压PWM，36.82为系数，1050 */

#define PWM_VAL_2A  (uint16_t)(2 * 147)        


typedef struct {
    float Battery_12V;      // 12V电压
    float Battery_24V;      // 24V电压
} BatteryConfig;

typedef struct {
    BatteryConfig Sealed;  // 铅酸电池
    BatteryConfig Li_ion;  // 锂电池
    BatteryConfig AGM;     // 12V锂电
} BatteryVoltageTable;



/********************** 变量类型重定义 **********************/
typedef enum
{
  WAIT_BATT  = 0x01,
  PREC       = 0x02,
	CCC1       = 0x03,        /* 恒流1阶段    */
	CHG_FULL   = 0x04,        /* 电池充满     */
  HC_NULL    = 0x05,        /* 电池充满     */
}CHGType;

typedef struct
{
	CHGType Now;
	CHGType Past;
}sCHGType;



typedef enum {
    RISING,   // 上升阶段
    FALLING   // 下降阶段
} State;

typedef struct {
    int currentValue; // 当前的PWM值
    int targetValue;  // 目标值
    int step;         // 每次调用增加/减少的步长
    State state;      // 当前的状态：上升或下降
} PWMControl;


extern sCHGType gChgType;
extern BatteryVoltageTable BatV;
/********************** 函数声明 **********************/

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
