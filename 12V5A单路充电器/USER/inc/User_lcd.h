#ifndef __USER_LCD_H
#define __USER_LCD_H

#include "User_include.h"

typedef enum {
    OFF_LED,            // ����
    Sealed_LED,         // Ǧ�� 
    AGM_LED,            // AGM 
    Lidian_LED,         // ﮵�
    SIXV_LED            // 6V  
} LED_NUM;


void LED_flash(void);
// ������LED
void LED_AllOn(void);
// �ر�����LED 
void LED_AllOff(void);
// �ر�����LED  ����ָ����LED
void LED_AlloneOff(uint8_t index);
// ��ָ��LED
void LED_SetOn(uint8_t index);
// �ر�ָ��LED
void LED_SetOff(uint8_t index);
// ���ݵ�ص�ѹ����LED״̬
void Update_LED_Status_By_Voltage(void);

void Handle_Error_State(void);

void Set_Charge_Mode_LED(void);
#endif 
