#ifndef __USER_INCLUDE_H_
#define __USER_INCLUDE_H_

/*------- Includes -------*/

/*芯片驱动头文件*/

#include "cw32l010.h"
#include "cw32l010_adc.h"
#include "cw32l010_gpio.h"
#include "cw32l010_uart.h"
#include "cw32l010_sysctrl.h"
#include "cw32l010_systick.h"
#include "interrupts_cw32l010.h"
#include "system_cw32l010.h"
#include "cw32l010_gtim.h"
#include "cw32l010_atim.h"
#include "stdio.h"
#include "string.h"
#include <stdlib.h>
#include "stdbool.h"
#include <math.h>
#include "cw32l010_iwdt.h"
/*硬件驱动头文件*/





/*用户个人头文件*/
#include "main.h"
#include "User_protect.h"
#include "User_gpio.h"
#include "User_adc.h"
#include "User_lcd.h"
#include "User_pwm.h"
#include "User_printf.h"
#include "User_charger.h"
#include "User_flash.h"
#include "task.h"
#endif
