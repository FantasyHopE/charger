#ifndef __USER_GPIO_H
#define __USER_GPIO_H

#include "User_include.h"


typedef enum {
    BUTTON_IDLE,     // ����δ����
    BUTTON_DEBOUNCE, // ȥ�����׶�
    BUTTON_PRESSED,  // ��������
    BUTTON_LONG_PRESSED  // ����
} ButtonState;


typedef enum {
    standby,          // ����   
    Sealed     ,      // Ǧ�� 1��
    AGM,              // AGM 2��
    lithium_battery,  // ﮵� 3��
    SIXV              // �޸�ģʽ 4�� 
} Tasknum;

#define DEBOUNCE_DELAY_MS 10  // ȥ����ʱ�䣬��λΪ����
#define LONG_PRESS_THRESHOLD_MS 1000  // ��������ֵʱ�䣬��λΪ����


#define ON_   PA01_SETHIGH()                          
#define OFF_  PA01_SETLOW()

extern Tasknum  ChargerTasknum;
extern Tasknum  Chargermode_Flash;

void ButtonTask(void);
void User_gpio_init(void);
void HandleLongPressImmediate(void);
void HandleShortPress(void);

#endif 
