#include "User_pwm.h"


// 新增滤波和滞回控制相关全局变量
#define FILTER_SAMPLES 5       // 电压采样滤波窗口大小
static uint16_t voltage_buffer[FILTER_SAMPLES] = {0};
static uint8_t voltage_index = 0;
static uint16_t filtered_voltage = 0;



#define POWER_LIMIT_WATTS 13000.0f      // 130W功率限制（单位：mW）

// PWM渐变步进限制（避免占空比突变）
#define MAX_PWM_STEP 50

uint16_t VoltageFilter(uint16_t raw_voltage);

void GPIO_Configuration(void)
{

    /* PB4 作为GTIM1的CH3 PWM 输出 */
    __SYSCTRL_GPIOB_CLK_ENABLE();
    
    PB04_DIGTAL_ENABLE();
    PB04_DIR_OUTPUT();
    PB04_PUSHPULL_ENABLE();
    PB04_AFx_GTIM1CH3();
    /* PB5 作为GTIM1的CH2 PWM 输出 */
    PB05_DIGTAL_ENABLE();
    PB05_DIR_OUTPUT();
    PB05_PUSHPULL_ENABLE();
    PB05_AFx_GTIM1CH2();
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
    
    GTIM_OC3ModeCfg(CW_GTIM1, &GTIM_OCModeCfgStruct); //通道3使能
    GTIM_OC2ModeCfg(CW_GTIM1, &GTIM_OCModeCfgStruct); //通道2使能
    
    GTIM_SetCompare3(CW_GTIM1, 0);//设置PWM占空比
    GTIM_SetCompare2(CW_GTIM1, 0);//设置PWM占空比 
    
    GTIM_OC3Cmd(CW_GTIM1, ENABLE);
    GTIM_OC2Cmd(CW_GTIM1, ENABLE);
    
    GTIM_Cmd(CW_GTIM1, ENABLE);
}



/*******************************************************
* 函数名称：VoltageFilter
* 功能描述：对电池电压进行移动平均滤波
* 输入参数：raw_voltage - 原始ADC采样值
* 输出参数：滤波后的电压值
*******************************************************/
uint16_t VoltageFilter(uint16_t raw_voltage) {
    // 更新采样缓冲区
    voltage_buffer[voltage_index] = raw_voltage;
    voltage_index = (voltage_index + 1) % FILTER_SAMPLES;
    
    // 计算移动平均值
    uint32_t sum = 0;
    for(uint8_t i=0; i<FILTER_SAMPLES; i++) {
        sum += voltage_buffer[i];
    }
    return (uint16_t)(sum / FILTER_SAMPLES);
}

/*******************************************************
* 函数名称：SetChargeCurrent
* 功能描述：设置充电电流（带滞回控制的功率限制）
* 输入参数：data - PWM比较值（来自温控或其他控制）
* 输出参数：无
*******************************************************/
void SetChargeCurrent(uint16_t data) {
    uint16_t read_value = 0, pwmvalue = 0, pwmvalue1 = 0, pwmvalue2 = 0;
    float thiscurrent = 0;

  
    // 1. 电压采样滤波（降低噪声影响）
    filtered_voltage = VoltageFilter(gBat_VoltageValue);
    
    // 2. 滞回功率限制（避免130W临界振荡）
    thiscurrent = POWER_LIMIT_WATTS / filtered_voltage; // 计算限流值
    
 
    pwmvalue2 = (thiscurrent / 10.0f) * CC100PCT_PWM_VAL_12V;

    // 3. 温控电流限制
    pwmvalue1 = CurrentControl(data); 
     
  
    // 4. 双重限制取最小值
    pwmvalue = (pwmvalue1 < pwmvalue2) ? pwmvalue1 : pwmvalue2;
    
    // 12V电池系统
    if(pwmvalue > CC100PCT_PWM_VAL_12V) {
        pwmvalue = CC100PCT_PWM_VAL_12V;
    }
    
    
    read_value = GTIM_GetCapture2(CW_GTIM1);
    
    if (read_value != pwmvalue)
    {
      GTIM_SetCompare2(CW_GTIM1,pwmvalue);
    }
    
}

/*******************************************************
* 函数名称：SetChargerVoltage
* 功能描述：设置充电电压（设置占空比）
* 输入参数：PWM比较值
* 输出参数：无
*******************************************************/
void SetChargerVoltage(uint16_t data)
{
	uint16_t read_value;
 

	read_value = GTIM_GetCapture3(CW_GTIM1);
	
	if (read_value != data)
	{
		GTIM_SetCompare3(CW_GTIM1,data);
	}
}
//铅酸专用
void SetChargerVoltage1(uint16_t data)
{
	uint16_t read_value;
  uint8_t  temp_value;
  
  temp_value=(BAT_flag)?6:3;//根据电池类型补偿
  
  if(jwen_v==1)   //温度补偿
  {
        //最大电压PWM-1V
    data=data-temp_value;
  
  }else if(jwen_v==2)
  { 
        //最大电压PWM-1V
     data=data+temp_value;
  }
  

	read_value = GTIM_GetCapture3(CW_GTIM1);
	
	if (read_value != data)
	{
		GTIM_SetCompare3(CW_GTIM1,data);
	}
}

/*******************************************************
* 函数名称：CurrentControl
* 功能描述：电流控制，根据PCB板上的NTC温度来调整电流值
* 输入参数：预期的电流PWM
* 输出参数：当前温度下最大能输出的电流PWM
*******************************************************/
uint16_t CurrentControl(uint16_t data)
{
	uint16_t ControlPWM = 0;
	uint16_t  num1 = 0,pwm1=0;
  
  pwm1=(BAT_flag)?PWM_05A:PWM_1A;//根据电池类型减电流
  
	if (gTemperature <= 80)    /* 若无NTC接入，或者NTC断路 */
	{
		return data;
	}
 
  if(gTemperature <= 85)
  {
    num1=pwm1*1;
  }
  else if(gTemperature <= 90)
  {
    num1=pwm1*2;
  }else if(gTemperature <= 95)
  {
    num1=pwm1*3;
  }
  
  
//  if(gTemperature <= 50)
//  {
//    num1=pwm1*1;
//  }
//  else if(gTemperature <= 60)
//  {
//    num1=pwm1*2;
//  }else if(gTemperature <= 70)
//  {
//    num1=pwm1*3;
//  }else if(gTemperature <= 80)
//  {
//    num1=pwm1*5;
//  }
//  else if(gTemperature <= 90)
//  {
//    num1=pwm1*7;
//  }
  
  
  
  ControlPWM=data-num1;
  
	return ControlPWM;
}
