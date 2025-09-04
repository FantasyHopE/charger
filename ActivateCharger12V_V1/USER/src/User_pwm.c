#include "User_pwm.h"


// �����˲����ͻؿ������ȫ�ֱ���
#define FILTER_SAMPLES 5       // ��ѹ�����˲����ڴ�С
static uint16_t voltage_buffer[FILTER_SAMPLES] = {0};
static uint8_t voltage_index = 0;
static uint16_t filtered_voltage = 0;



#define POWER_LIMIT_WATTS 13000.0f      // 130W�������ƣ���λ��mW��

// PWM���䲽�����ƣ�����ռ�ձ�ͻ�䣩
#define MAX_PWM_STEP 50

uint16_t VoltageFilter(uint16_t raw_voltage);

void GPIO_Configuration(void)
{

    /* PB4 ��ΪGTIM1��CH3 PWM ��� */
    __SYSCTRL_GPIOB_CLK_ENABLE();
    
    PB04_DIGTAL_ENABLE();
    PB04_DIR_OUTPUT();
    PB04_PUSHPULL_ENABLE();
    PB04_AFx_GTIM1CH3();
    /* PB5 ��ΪGTIM1��CH2 PWM ��� */
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
    
    GTIM_OC3ModeCfg(CW_GTIM1, &GTIM_OCModeCfgStruct); //ͨ��3ʹ��
    GTIM_OC2ModeCfg(CW_GTIM1, &GTIM_OCModeCfgStruct); //ͨ��2ʹ��
    
    GTIM_SetCompare3(CW_GTIM1, 0);//����PWMռ�ձ�
    GTIM_SetCompare2(CW_GTIM1, 0);//����PWMռ�ձ� 
    
    GTIM_OC3Cmd(CW_GTIM1, ENABLE);
    GTIM_OC2Cmd(CW_GTIM1, ENABLE);
    
    GTIM_Cmd(CW_GTIM1, ENABLE);
}



/*******************************************************
* �������ƣ�VoltageFilter
* �����������Ե�ص�ѹ�����ƶ�ƽ���˲�
* ���������raw_voltage - ԭʼADC����ֵ
* ����������˲���ĵ�ѹֵ
*******************************************************/
uint16_t VoltageFilter(uint16_t raw_voltage) {
    // ���²���������
    voltage_buffer[voltage_index] = raw_voltage;
    voltage_index = (voltage_index + 1) % FILTER_SAMPLES;
    
    // �����ƶ�ƽ��ֵ
    uint32_t sum = 0;
    for(uint8_t i=0; i<FILTER_SAMPLES; i++) {
        sum += voltage_buffer[i];
    }
    return (uint16_t)(sum / FILTER_SAMPLES);
}

/*******************************************************
* �������ƣ�SetChargeCurrent
* �������������ó����������ͻؿ��ƵĹ������ƣ�
* ���������data - PWM�Ƚ�ֵ�������¿ػ��������ƣ�
* �����������
*******************************************************/
void SetChargeCurrent(uint16_t data) {
    uint16_t read_value = 0, pwmvalue = 0, pwmvalue1 = 0, pwmvalue2 = 0;
    float thiscurrent = 0;

  
    // 1. ��ѹ�����˲�����������Ӱ�죩
    filtered_voltage = VoltageFilter(gBat_VoltageValue);
    
    // 2. �ͻع������ƣ�����130W�ٽ��񵴣�
    thiscurrent = POWER_LIMIT_WATTS / filtered_voltage; // ��������ֵ
    
 
    pwmvalue2 = (thiscurrent / 10.0f) * CC100PCT_PWM_VAL_12V;

    // 3. �¿ص�������
    pwmvalue1 = CurrentControl(data); 
     
  
    // 4. ˫������ȡ��Сֵ
    pwmvalue = (pwmvalue1 < pwmvalue2) ? pwmvalue1 : pwmvalue2;
    
    // 12V���ϵͳ
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
* �������ƣ�SetChargerVoltage
* �������������ó���ѹ������ռ�ձȣ�
* ���������PWM�Ƚ�ֵ
* �����������
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
//Ǧ��ר��
void SetChargerVoltage1(uint16_t data)
{
	uint16_t read_value;
  uint8_t  temp_value;
  
  temp_value=(BAT_flag)?6:3;//���ݵ�����Ͳ���
  
  if(jwen_v==1)   //�¶Ȳ���
  {
        //����ѹPWM-1V
    data=data-temp_value;
  
  }else if(jwen_v==2)
  { 
        //����ѹPWM-1V
     data=data+temp_value;
  }
  

	read_value = GTIM_GetCapture3(CW_GTIM1);
	
	if (read_value != data)
	{
		GTIM_SetCompare3(CW_GTIM1,data);
	}
}

/*******************************************************
* �������ƣ�CurrentControl
* �����������������ƣ�����PCB���ϵ�NTC�¶�����������ֵ
* ���������Ԥ�ڵĵ���PWM
* �����������ǰ�¶������������ĵ���PWM
*******************************************************/
uint16_t CurrentControl(uint16_t data)
{
	uint16_t ControlPWM = 0;
	uint16_t  num1 = 0,pwm1=0;
  
  pwm1=(BAT_flag)?PWM_05A:PWM_1A;//���ݵ�����ͼ�����
  
	if (gTemperature <= 80)    /* ����NTC���룬����NTC��· */
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
