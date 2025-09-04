#ifndef __TASK_H
#define __TASK_H

#include "User_include.h"

/************************ �궨�� ************************/
#define TASK_MAX        7        /* ��������������� */

#define TASK_DELAY_0    10
#define TASK_DELAY_1    1000
#define TASK_DELAY_2    200
#define TASK_DELAY_3    20
#define TASK_DELAY_4    100
#define TASK_DELAY_5    500
#define TASK_DELAY_6    2000



#define nop _nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_(); //�궨�� US��ʱ



/************************* �������� *************************/
extern uint32_t gTaskDelay[TASK_MAX];


/************************* �������� *************************/
extern void TaskDelayInit(void);
extern void Task0(void);
extern void Task1(void);
extern void Task2(void);
extern void Task3(void);
extern void Task4(void);
extern void Task5(void);
extern void Task6(void);
void print_buff(uint16_t *arr, int row, int col);

#endif
