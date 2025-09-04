#ifndef __USER_PROTECT_H
#define __USER_PROTECT_H

#include "User_include.h"





/********************** ���������ض��� **********************/
typedef enum
{
	FAULT_NULL = 0x00,    /* �޹���                   */
	INF        = 0x01,    /* �����쳣                 */
	HOTP       = 0x02,    /* Ӳ������                 */
	ROTP       = 0x03,    /* �������                 */
	COVP       = 0x04,    /* ���������ѹ           */
	COCP       = 0x05,    /* �����������           */
	BOVP       = 0x06,    /* ��ع�ѹ                 */
	BERR       = 0x07,    /* ����쳣                 */
	LERR       = 0x08,    /* �����쳣(�����ͻȻ����) */
	FULL_CHG   = 0x09,    /* ����                     */
	FJBH       = 0x0a,    /* ���ӱ���                 */
	STANDBY    = 0x0b     /* ����          */
}FAULTType;

typedef struct
{
	FAULTType Now;
	FAULTType Past;
}sFAULTType;

void protect(void);


/************************* �������� *************************/
extern sFAULTType gFaultType;
extern uint8_t BAT_flag; 
extern uint8_t W_S_flag;                                 /* �¶ȱ�־ ����   */
extern uint8_t jwen_v; 
extern uint8_t BATone_flag;                               /* ��һ���ϵ��¼   */
extern uint8_t FaultLock;
extern uint8_t BERR_flag;





#endif 


