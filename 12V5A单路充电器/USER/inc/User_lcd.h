#ifndef __USER_LCD_H
#define __USER_LCD_H

#include "User_include.h"

typedef enum {
    OFF_LED,            // 待机
    Sealed_LED,         // 铅酸 
    AGM_LED,            // AGM 
    Lidian_LED,         // 锂电
    SIXV_LED            // 6V  
} LED_NUM;


void LED_flash(void);
// 打开所有LED
void LED_AllOn(void);
// 关闭所有LED 
void LED_AllOff(void);
// 关闭所有LED  除了指定的LED
void LED_AlloneOff(uint8_t index);
// 打开指定LED
void LED_SetOn(uint8_t index);
// 关闭指定LED
void LED_SetOff(uint8_t index);
// 根据电池电压更新LED状态
void Update_LED_Status_By_Voltage(void);

void Handle_Error_State(void);

void Set_Charge_Mode_LED(void);
#endif 
