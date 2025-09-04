#include "User_pwm.h"

void GPIO_Configuration(void)
{

    /* PB2 ��ΪATIM��CH2 PWM ��� */
    __SYSCTRL_GPIOB_CLK_ENABLE();
    __SYSCTRL_GPIOA_CLK_ENABLE();
    __SYSCTRL_ATIM_CLK_ENABLE();

    
   /* PB5 ��ΪGTIM1��CH2 PWM ���  */
    PB05_DIGTAL_ENABLE();
    PB05_DIR_OUTPUT();
    PB05_PUSHPULL_ENABLE();
    PB05_AFx_GTIM1CH2();
  
 
    // PB02 ATIM_CH2    
    PB02_DIGTAL_ENABLE();
    PB02_DIR_OUTPUT();
    PB02_PUSHPULL_ENABLE();
		PB02_AFx_ATIMCH2();

  
    // PA00��ΪATIM_CH4
    PA00_DIGTAL_ENABLE();
    PA00_DIR_OUTPUT();
    PA00_PUSHPULL_ENABLE();
    PA00_AFx_ATIMCH4();
    
    // PB04��ΪATIMCH1 
    PB04_DIGTAL_ENABLE();
    PB04_DIR_OUTPUT();
    PB04_PUSHPULL_ENABLE();
    PB04_AFx_ATIMCH1();  // ����ΪATIMͨ��1
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
    
    // ͨ��2 (PB05)
    GTIM_OC2ModeCfg(CW_GTIM1, &GTIM_OCModeCfgStruct);
    GTIM_SetCompare2(CW_GTIM1, PWM_VAL_2A);
    GTIM_OC2Cmd(CW_GTIM1, ENABLE);
    
    GTIM_Cmd(CW_GTIM1, ENABLE);
    
    
    ATIM_InitTypeDef ATIM_InitStruct = {DISABLE,0};
    ATIM_OCInitTypeDef ATIM_OCInitStruct = {0};
    
    // ��ATIM���м����Ļ�������   
    ATIM_InitStruct.BufferState = DISABLE;     //������ARR�Ļ��湦��    
    ATIM_InitStruct.CounterAlignedMode = ATIM_COUNT_ALIGN_MODE_CENTER_BOTH;    // 
    ATIM_InitStruct.CounterDirection = ATIM_COUNTING_UP;
    ATIM_InitStruct.CounterOPMode = ATIM_OP_MODE_REPETITIVE;    
    ATIM_InitStruct.Prescaler = 8; //8��Ƶ������ʱ��1MHz
    ATIM_InitStruct.ReloadValue = 3000;                // �������100us
    ATIM_InitStruct.RepetitionCounter = 0;
    ATIM_Init(&ATIM_InitStruct);
    

    ATIM_OCInitStruct.BufferState = DISABLE;
    ATIM_OCInitStruct.OCComplement = DISABLE;
    ATIM_OCInitStruct.OCFastMode = DISABLE;    
    
    ATIM_OCInitStruct.OCInterruptState = ENABLE;
    ATIM_OCInitStruct.OCMode = ATIM_OCMODE_PWM2;    
    ATIM_OCInitStruct.OCPolarity = ATIM_OCPOLARITY_NONINVERT ;
    
    /* ATIMͨ��1���� (PB04) */
    ATIM_OC1Init(&ATIM_OCInitStruct);
    ATIM_CH1Config(ENABLE);

    
     /* ATIMͨ��2���� (PB02) */
    ATIM_OC2Init(&ATIM_OCInitStruct);
    ATIM_CH2Config(ENABLE);
    
    /* ATIMͨ��4���� (PA00) */
    ATIM_OC4Init(&ATIM_OCInitStruct);
    ATIM_CH4Config(ENABLE);

    
    ATIM_CtrlPWMOutputs(ENABLE);
    
    ATIM_Cmd(ENABLE);
    
    ATIM_SetCompare1(0);//�ٷ�֮100
    ATIM_SetCompare2(0);//�ٷ�֮30
    ATIM_SetCompare4(0);//�ٷ�֮60
}

/*******************************************************
* �������ƣ�SetChargeCurrent
* �������������ó�����������ռ�ձȣ�
* ���������PWM�Ƚ�ֵ
* �����������
*******************************************************/
void SetChargeCurrent(uint16_t data)
{
  uint16_t ReadData = 0, temp = 0;
	
	ReadData = GTIM_GetCapture2(CW_GTIM1);
	
	if (data <= ReadData)
	{
		// Ҫ��СPWM��������С
		GTIM_SetCompare2(CW_GTIM1, data);
	}
	else
	{
		// PWMֵҪ����
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


sLED_State StatusLEDs = {Lights}; //������״̬
// �����ƿ��ƺ���
// ��ʼ�������Ƶ�ռ�ձ�
uint16_t pwm_val[3] = {0};

// �����������ӻ��Ǽ��ٵı�־λ
int8_t dir[3] = {1,1,1}; // 1��ʾ���ӣ�-1��ʾ����


void Breathing_Light_Update(void) {
    // ������1��ռ�ձ�
    if (StatusLEDs.LED_30==Flicker) { // �жϵ�1�Ƿ���˸
        pwm_val[0] += dir[0] * PWM_STEP;
        if (pwm_val[0] >= PWM_MAX) {
            pwm_val[0] = PWM_MAX;
            dir[0] = -1; // ��ʼ����
        } else if (pwm_val[0] <= PWM_MIN) {
            pwm_val[0] = PWM_MIN;
            dir[0] = 1; // ��ʼ����
        }
        ATIM_SetCompare2(pwm_val[0]);
    } else if(StatusLEDs.LED_30==Always) {
        ATIM_SetCompare2(PWM_MAX); // ��1����
    }else if(StatusLEDs.LED_30==Lights){
        ATIM_SetCompare2(0); // ��1Ϩ��
    }

    // ������2��ռ�ձ�
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
        ATIM_SetCompare4(PWM_MAX); // ��2���ֳ���
    }else if(StatusLEDs.LED_60==Lights){
        ATIM_SetCompare4(0); // ��2Ϩ��
    }

    // ������3��ռ�ձ�
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
        ATIM_SetCompare1(PWM_MAX); // ��3����
    }else if(StatusLEDs.LED_100==Lights){
        ATIM_SetCompare1(0); // ��3Ϩ��
    }
}
