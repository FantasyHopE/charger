#ifndef __USER_PROTECT_H
#define __USER_PROTECT_H

#include "User_include.h"

/********************** ���������ض��� **********************/
typedef enum
{
	FAULT_NULL = 0x00,    /* �޹���                   */
	HOTP       = 0x01,    /* Ӳ������                 */
	COVP       = 0x02,    /* ���������ѹ           */
	COCP       = 0x03,    /* �����������           */
	BERR       = 0x04,    /* ����쳣                 */
	FULL_CHG   = 0x05,    /* ����                     */
	FJBH       = 0x06    /* ���ӱ���                 */
}FAULTType;

typedef struct
{
	FAULTType Now;
	FAULTType Past;
}sFAULTType;


/************************* �������� *************************/
extern sFAULTType gFaultType;


/************************* �������� *************************/
extern void protect(void);
extern uint8_t StartCheck(void);











#endif 


