#ifndef __USER_GPIO_H
#define __USER_GPIO_H

#include "User_include.h"


typedef enum {
    BUTTON_IDLE,     // 按键未按下
    BUTTON_DEBOUNCE, // 去抖动阶段
    BUTTON_PRESSED,  // 按键按下
    BUTTON_LONG_PRESSED  // 长按
} ButtonState;


typedef enum {
    standby,          // 待机   
    Sealed     ,      // 铅酸 1；
    AGM,              // AGM 2；
    lithium_battery,  // 锂电 3；
    SIXV              // 修复模式 4； 
} Tasknum;

#define DEBOUNCE_DELAY_MS 10  // 去抖动时间，单位为毫秒
#define LONG_PRESS_THRESHOLD_MS 1000  // 长按的阈值时间，单位为毫秒


#define ON_   PA01_SETHIGH()                          
#define OFF_  PA01_SETLOW()

extern Tasknum  ChargerTasknum;
extern Tasknum  Chargermode_Flash;

void ButtonTask(void);
void User_gpio_init(void);
void HandleLongPressImmediate(void);
void HandleShortPress(void);

#endif 
