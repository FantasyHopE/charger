#ifndef __USER_FlASH_H_
#define __USER_FlASH_H_

#include "User_include.h"


extern uint16_t FEE_data[5]; //�������ʼֵ�������Ŷ�Ӧ����ID��������Ͳ˵���������һ��

typedef enum {
    ChargerMode_ID,//���ģʽ
    En1,
    En2,
    En3,
    En4
} en_feedata_t; // ����ID

extern void FEE_init(void); // eeprom��ʼ��
extern uint16_t FEE_rd(en_feedata_t); // ��
extern void FEE_wr(uint16_t,en_feedata_t); // д





#endif
