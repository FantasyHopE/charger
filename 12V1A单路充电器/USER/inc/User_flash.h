#ifndef __USER_FlASH_H_
#define __USER_FlASH_H_

#include "User_include.h"


extern uint16_t FEE_data[5]; //变量表初始值，数组标号对应变量ID。变量表和菜单参数放在一起

typedef enum {
    ChargerMode_ID,//充电模式
    En1,
    En2,
    En3,
    En4
} en_feedata_t; // 变量ID

extern void FEE_init(void); // eeprom初始化
extern uint16_t FEE_rd(en_feedata_t); // 读
extern void FEE_wr(uint16_t,en_feedata_t); // 写





#endif
