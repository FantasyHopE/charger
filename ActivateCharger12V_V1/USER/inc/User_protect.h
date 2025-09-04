#ifndef __USER_PROTECT_H
#define __USER_PROTECT_H

#include "User_include.h"





/********************** 变量类型重定义 **********************/
typedef enum
{
	FAULT_NULL = 0x00,    /* 无故障                   */
	INF        = 0x01,    /* 输入异常                 */
	HOTP       = 0x02,    /* 硬件过温                 */
	ROTP       = 0x03,    /* 软件过温                 */
	COVP       = 0x04,    /* 充电机输出过压           */
	COCP       = 0x05,    /* 充电机输出过流           */
	BOVP       = 0x06,    /* 电池过压                 */
	BERR       = 0x07,    /* 电池异常                 */
	LERR       = 0x08,    /* 负载异常(充电中突然空载) */
	FULL_CHG   = 0x09,    /* 充满                     */
	FJBH       = 0x0a,    /* 反接保护                 */
	STANDBY    = 0x0b     /* 待机          */
}FAULTType;

typedef struct
{
	FAULTType Now;
	FAULTType Past;
}sFAULTType;

void protect(void);


/************************* 变量声明 *************************/
extern sFAULTType gFaultType;
extern uint8_t BAT_flag; 
extern uint8_t W_S_flag;                                 /* 温度标志 冬夏   */
extern uint8_t jwen_v; 
extern uint8_t BATone_flag;                               /* 第一次上电记录   */
extern uint8_t FaultLock;
extern uint8_t BERR_flag;





#endif 


