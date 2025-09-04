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
    BUTTON_IDLE,     // ����δ����
    BUTTON_DEBOUNCE, // ȥ�����׶�
    BUTTON_PRESSED,  // ��������
    BUTTON_LONG_PRESSED  // ����
} ButtonState;

typedef enum {
    Standby_mode,
    Sealed_mode,  // Ǧ��ģʽ 1��
    Start_stop_mode,  // ��ͣģʽ 2��
    Li_battery_mode,  // ﮵�ģʽ
    SIX_mode,         // 6vģʽ
    Repair_mode,      // �޸�ģʽ 4��
    power_supply_mode // ����ģʽ
} Tasknum;


#define DEBOUNCE_DELAY_MS 50  // ȥ����ʱ�䣬��λΪ����
#define LONG_PRESS_THRESHOLD_MS 3000  // ��������ֵʱ�䣬��λΪ����

#define  FB_ON    GPIO_WritePin(CW_GPIOB, GPIO_PIN_6,GPIO_Pin_SET)     //����������
#define  FB_OFF   GPIO_WritePin(CW_GPIOB, GPIO_PIN_6,GPIO_Pin_RESET)

#define  SD_ON   GPIO_WritePin(CW_GPIOA, GPIO_PIN_0,GPIO_Pin_SET)      //SD����
#define  SD_OFF  GPIO_WritePin(CW_GPIOA, GPIO_PIN_0,GPIO_Pin_RESET)

#define  OUT_OFF  GPIO_WritePin(CW_GPIOA, GPIO_PIN_1,GPIO_Pin_RESET)   //�������
#define  OUT_ON   GPIO_WritePin(CW_GPIOA, GPIO_PIN_1,GPIO_Pin_SET)

extern Tasknum  ChargerTasknum;
extern Tasknum  Chargermode_Flash;
/*��������*/
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
