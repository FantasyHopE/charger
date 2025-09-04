#ifndef __USER_GPIO_H
#define __USER_GPIO_H

#ifdef __cplusplus
extern "C"
{
#endif

/* ----- Includes -----*/

#include "User_include.h"

/* ----- define -----*/



typedef enum {
    BUTTON_IDLE,     // 按键未按下
    BUTTON_DEBOUNCE, // 去抖动阶段
    BUTTON_PRESSED,  // 按键按下
    BUTTON_LONG_PRESSED  // 长按
} ButtonState;

typedef enum {
    Standby_mode,
    Sealed_mode,  // 铅酸模式 1；
    Start_stop_mode,  // 启停模式 2；
    Li_battery_mode,  // 锂电模式
    SIX_mode,         // 6v模式
    Repair_mode,      // 修复模式 4；
    power_supply_mode // 供电模式
} Tasknum;


#define DEBOUNCE_DELAY_MS 50  // 去抖动时间，单位为毫秒
#define LONG_PRESS_THRESHOLD_MS 3000  // 长按的阈值时间，单位为毫秒

#define  FB_ON    GPIO_WritePin(CW_GPIOB, GPIO_PIN_6,GPIO_Pin_SET)     //蜂鸣器开关
#define  FB_OFF   GPIO_WritePin(CW_GPIOB, GPIO_PIN_6,GPIO_Pin_RESET)

#define  SD_ON   GPIO_WritePin(CW_GPIOA, GPIO_PIN_0,GPIO_Pin_SET)      //SD开关
#define  SD_OFF  GPIO_WritePin(CW_GPIOA, GPIO_PIN_0,GPIO_Pin_RESET)

#define  OUT_OFF  GPIO_WritePin(CW_GPIOA, GPIO_PIN_1,GPIO_Pin_RESET)   //输出开关
#define  OUT_ON   GPIO_WritePin(CW_GPIOA, GPIO_PIN_1,GPIO_Pin_SET)

extern Tasknum  ChargerTasknum;
extern Tasknum  Chargermode_Flash;
/*函数声明*/
void User_gpio_init(void);
void ButtonTask(void);
void led1_init(uint8_t S_R);
void led2_init(uint8_t S_R);
void led2_deint(void);
void led1_deint(void);
void ALL_LEDOFF(void);
void Sealed_ledon(void);
void Start_stop_ledon(void);
void Motorcycle_ledon(void);
void SIX_ledon(void);
void ALL_LEDON(void);
void HandleShortPress(void);
void HandleLongPressImmediate(void);

#ifdef __cplusplus
}
#endif

#endif 
