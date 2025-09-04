#ifndef __USER_PROTECT_H
#define __USER_PROTECT_H

#include "User_include.h"

/********************** 变量类型重定义 **********************/
typedef enum
{
	FAULT_NULL = 0x00,    /* 无故障                   */
	HOTP       = 0x01,    /* 硬件过温                 */
	COVP       = 0x02,    /* 充电机输出过压           */
	COCP       = 0x03,    /* 充电机输出过流           */
	BERR       = 0x04,    /* 电池异常                 */
	FULL_CHG   = 0x05,    /* 充满                     */
	FJBH       = 0x06    /* 反接保护                 */
}FAULTType;

typedef struct
{
	FAULTType Now;
	FAULTType Past;
}sFAULTType;


/************************* 变量声明 *************************/
extern sFAULTType gFaultType;


/************************* 函数声明 *************************/
extern void protect(void);
extern uint8_t StartCheck(void);











#endif 


