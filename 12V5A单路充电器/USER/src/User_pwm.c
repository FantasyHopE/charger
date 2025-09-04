#include "User_pwm.h"

void GPIO_Configuration(void)
{

    /* PB2 作为ATIM的CH2 PWM 输出 */
    __SYSCTRL_GPIOB_CLK_ENABLE();
    __SYSCTRL_GPIOA_CLK_ENABLE();
    __SYSCTRL_ATIM_CLK_ENABLE();

    
   /* PB5 作为GTIM1的CH2 PWM 输出  */
    PB05_DIGTAL_ENABLE();
    PB05_DIR_OUTPUT();
    PB05_PUSHPULL_ENABLE();
    PB05_AFx_GTIM1CH2();
  
 
    // PB02 ATIM_CH2    
    PB02_DIGTAL_ENABLE();
    PB02_DIR_OUTPUT();
    PB02_PUSHPULL_ENABLE();
		PB02_AFx_ATIMCH2();

  
    // PA00作为ATIM_CH4
    PA00_DIGTAL_ENABLE();
    PA00_DIR_OUTPUT();
    PA00_PUSHPULL_ENABLE();
    PA00_AFx_ATIMCH4();
    
    // PB04作为ATIMCH1 
    PB04_DIGTAL_ENABLE();
    PB04_DIR_OUTPUT();
    PB04_PUSHPULL_ENABLE();
    PB04_AFx_ATIMCH1();  // 复用为ATIM通道1
}


void PWM_OutputConfig(void)
{
    GPIO_Configuration();
    
    GTIM_InitTypeDef GTIM_InitStruct = {0};
    GTIM_OCModeCfgTypeDef GTIM_OCModeCfgStruct = {DISABLE,DISABLE,0};

    __SYSCTRL_GTIM1_CLK_ENABLE();

    GTIM_InitStruct.AlignMode    =  GTIM_ALIGN_MODE_EDGE;
    GTIM_InitStruct.ARRBuffState =  GTIM_ARR_BUFF_EN;
    GTIM_InitStruct.Direction    =  GTIM_DIRECTION_UP;
    GTIM_InitStruct.EventOption  =  GTIM_EVENT_NORMAL;
    GTIM_InitStruct.Prescaler    =  4 - 1;
    GTIM_InitStruct.PulseMode    =  GTIM_PULSE_MODE_DIS;
    GTIM_InitStruct.ReloadValue  =  3000 -1;
    GTIM_InitStruct.UpdateOption =  GTIM_UPDATE_DIS;
    GTIM_TimeBaseInit(CW_GTIM1, &GTIM_InitStruct);
    
    
    GTIM_OCModeCfgStruct.FastMode = DISABLE;
    GTIM_OCModeCfgStruct.OCMode = GTIM_OC_MODE_PWM1;
    GTIM_OCModeCfgStruct.OCPolarity = GTIM_OC_POLAR_NONINVERT;
    GTIM_OCModeCfgStruct.PreloadState = DISABLE;
    
    // 通道2 (PB05)
    GTIM_OC2ModeCfg(CW_GTIM1, &GTIM_OCModeCfgStruct);
    GTIM_SetCompare2(CW_GTIM1, PWM_VAL_2A);
    GTIM_OC2Cmd(CW_GTIM1, ENABLE);
    
    GTIM_Cmd(CW_GTIM1, ENABLE);
    
    
    ATIM_InitTypeDef ATIM_InitStruct = {DISABLE,0};
    ATIM_OCInitTypeDef ATIM_OCInitStruct = {0};
    
    // 对ATIM进行计数的基本设置   
    ATIM_InitStruct.BufferState = DISABLE;     //不启用ARR的缓存功能    
    ATIM_InitStruct.CounterAlignedMode = ATIM_COUNT_ALIGN_MODE_CENTER_BOTH;    // 
    ATIM_InitStruct.CounterDirection = ATIM_COUNTING_UP;
    ATIM_InitStruct.CounterOPMode = ATIM_OP_MODE_REPETITIVE;    
    ATIM_InitStruct.Prescaler = 8; //8分频，计数时钟1MHz
    ATIM_InitStruct.ReloadValue = 3000;                // 溢出周期100us
    ATIM_InitStruct.RepetitionCounter = 0;
    ATIM_Init(&ATIM_InitStruct);
    

    ATIM_OCInitStruct.BufferState = DISABLE;
    ATIM_OCInitStruct.OCComplement = DISABLE;
    ATIM_OCInitStruct.OCFastMode = DISABLE;    
    
    ATIM_OCInitStruct.OCInterruptState = ENABLE;
    ATIM_OCInitStruct.OCMode = ATIM_OCMODE_PWM2;    
    ATIM_OCInitStruct.OCPolarity = ATIM_OCPOLARITY_NONINVERT ;
    
    /* ATIM通道1配置 (PB04) */
    ATIM_OC1Init(&ATIM_OCInitStruct);
    ATIM_CH1Config(ENABLE);

    
     /* ATIM通道2配置 (PB02) */
    ATIM_OC2Init(&ATIM_OCInitStruct);
    ATIM_CH2Config(ENABLE);
    
    /* ATIM通道4配置 (PA00) */
    ATIM_OC4Init(&ATIM_OCInitStruct);
    ATIM_CH4Config(ENABLE);

    
    ATIM_CtrlPWMOutputs(ENABLE);
    
    ATIM_Cmd(ENABLE);
    
    ATIM_SetCompare1(0);//百分之100
    ATIM_SetCompare2(0);//百分之30
    ATIM_SetCompare4(0);//百分之60
}

/*******************************************************
* 函数名称：SetChargeCurrent
* 功能描述：设置充电电流（设置占空比）
* 输入参数：PWM比较值
* 输出参数：无
*******************************************************/
void SetChargeCurrent(uint16_t data)
{
  uint16_t ReadData = 0, temp = 0;
	
	ReadData = GTIM_GetCapture2(CW_GTIM1);
	
	if (data <= ReadData)
	{
		// 要减小PWM，立即减小
		GTIM_SetCompare2(CW_GTIM1, data);
	}
	else
	{
		// PWM值要增大
		temp = data - ReadData;
		if (temp > 20)
		{
			data = ReadData + 20;
			GTIM_SetCompare2(CW_GTIM1, data);
		}
		else
		{
			data = ReadData + temp;
			GTIM_SetCompare2(CW_GTIM1, data);
		}
	}
  
}


sLED_State StatusLEDs = {Lights}; //呼吸灯状态
// 呼吸灯控制函数
// 初始化三个灯的占空比
uint16_t pwm_val[3] = {0};

// 控制亮度增加还是减少的标志位
int8_t dir[3] = {1,1,1}; // 1表示增加，-1表示减少


void Breathing_Light_Update(void) {
    // 调整灯1的占空比
    if (StatusLEDs.LED_30==Flicker) { // 判断灯1是否闪烁
        pwm_val[0] += dir[0] * PWM_STEP;
        if (pwm_val[0] >= PWM_MAX) {
            pwm_val[0] = PWM_MAX;
            dir[0] = -1; // 开始减少
        } else if (pwm_val[0] <= PWM_MIN) {
            pwm_val[0] = PWM_MIN;
            dir[0] = 1; // 开始增加
        }
        ATIM_SetCompare2(pwm_val[0]);
    } else if(StatusLEDs.LED_30==Always) {
        ATIM_SetCompare2(PWM_MAX); // 灯1常亮
    }else if(StatusLEDs.LED_30==Lights){
        ATIM_SetCompare2(0); // 灯1熄灭
    }

    // 调整灯2的占空比
    if (StatusLEDs.LED_60==Flicker) {
        pwm_val[1] += dir[1] * PWM_STEP;
        if (pwm_val[1] >= PWM_MAX) {
            pwm_val[1] = PWM_MAX;
            dir[1] = -1;
        } else if (pwm_val[1] <= PWM_MIN) {
            pwm_val[1] = PWM_MIN;
            dir[1] = 1;
        }
        ATIM_SetCompare4(pwm_val[1]);
    }
    else if(StatusLEDs.LED_60==Always) {
        ATIM_SetCompare4(PWM_MAX); // 灯2保持常亮
    }else if(StatusLEDs.LED_60==Lights){
        ATIM_SetCompare4(0); // 灯2熄灭
    }

    // 调整灯3的占空比
    if (StatusLEDs.LED_100==Flicker) {
        pwm_val[2] += dir[2] * PWM_STEP;
        if (pwm_val[2] >= PWM_MAX) {
            pwm_val[2] = PWM_MAX;
            dir[2] = -1;
        } else if (pwm_val[2] <= PWM_MIN ) {
            pwm_val[2] = PWM_MIN;
            dir[2] = 1;
        }
        ATIM_SetCompare1(pwm_val[2]);
    } else if(StatusLEDs.LED_100==Always) {
        ATIM_SetCompare1(PWM_MAX); // 灯3常亮
    }else if(StatusLEDs.LED_100==Lights){
        ATIM_SetCompare1(0); // 灯3熄灭
    }
}
