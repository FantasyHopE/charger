#include "User_adc.h"

/************************** ȫ�ֱ������� *************************/
uint16_t gtemp_VoltageValue=0;      /* B-��ѹֵ       */
uint16_t gBat_VoltageValue=0;      /* ��ص�ѹֵ       */
uint16_t gInner_VoltageValue=0;    /* B+��ѹֵ */
uint16_t gCurrentValue=0;          /* ������� */
uint16_t gTemperature_ADValue=0;   /* �¶�ADֵ */
float    gTemperature=0;           /* �¶� */

uint16_t gAdcResult[10][4]={0};    //ת������
uint8_t gFlagIrq;

uint8_t  FJBH_flag=0;


/*******************************************************
* �������ƣ�User_adc_init
* ����������ADC ��ʼ��
* ���������void
* ���������void
*******************************************************/
void User_adc_init(void)
{

    /********����ADC����˿�********/
    //��GPIOʱ��
    __SYSCTRL_GPIOA_CLK_ENABLE();
    __SYSCTRL_GPIOB_CLK_ENABLE();
    //��ADCʱ��
    __SYSCTRL_ADC_CLK_ENABLE();

    //set PA04 as AIN4 INPUT  �¶�
    PA04_ANALOG_ENABLE();
    //set PA05 as AIN5 INPUT   BAT-
    PA05_ANALOG_ENABLE();
    //set PA06 as AIN6 INPUT   BAT+
    PA06_ANALOG_ENABLE();

    //set PB03 as AIN10 INPUT
    PB03_ANALOG_ENABLE();   //ʹ��PB03 ģ��ת��   ����

    /********����ADC����˿�********/

    ADC_InitTypeDef ADC_InitStructure = {0};


    //ADC����ת��ģʽ����
    ADC_InitStructure.ADC_ClkDiv = ADC_Clk_Div1;    // PCLK/2   6mhz
    ADC_InitStructure.ADC_ConvertMode = ADC_ConvertMode_Continuous;
    ADC_InitStructure.ADC_SQREns = ADC_SqrEns0to3; //ʹ����ͨ��
    
    ADC_InitStructure.ADC_IN0.ADC_InputChannel = ADC_InputCH4;   //ѡ�� 8ͨ�� ��Ӧ��  16��ADC����ͨ��   
    ADC_InitStructure.ADC_IN0.ADC_SampTime = ADC_SampTime30Clk;  //����ʱ��
    

    ADC_InitStructure.ADC_IN1.ADC_InputChannel = ADC_InputCH5;
    ADC_InitStructure.ADC_IN1.ADC_SampTime = ADC_SampTime42Clk;
    
    ADC_InitStructure.ADC_IN2.ADC_InputChannel = ADC_InputCH6;
    ADC_InitStructure.ADC_IN2.ADC_SampTime = ADC_SampTime42Clk;
    
    ADC_InitStructure.ADC_IN3.ADC_InputChannel = ADC_InputCH10;   //pb3
    ADC_InitStructure.ADC_IN3.ADC_SampTime = ADC_SampTime54Clk;
    


    ADC_Init(&ADC_InitStructure);
  
    ADC_ClearITPendingAll();//��������ж�
    
    ADC_ITConfig(ADC_IT_EOS, ENABLE);//ʹ��ת������ж�
    NVIC_EnableIRQ(ADC_IRQn);
    //ADCʹ��
    ADC_Enable();
    ADC_SoftwareStartConvCmd(ENABLE);//ADCת���������

}

/*******************************************************
* �������ƣ�CurrentSample
* ������������������
* ���������void
* ���������void
*******************************************************/
void User_ADC_Sample(void)
{
  static uint8_t i=0;
  if (gFlagIrq & ADC_ISR_EOS_Msk)
  {
    ADC_SoftwareStartConvCmd(DISABLE); //ʧ�����ת��

    ADC_GetSqr0Result(&gAdcResult[i][0]); //�¶�
    ADC_GetSqr1Result(&gAdcResult[i][1]); //BAT-
    ADC_GetSqr2Result(&gAdcResult[i][2]); //BAT+
    ADC_GetSqr3Result(&gAdcResult[i][3]); //����
    
    i++;
    i%=10;//ת��10��
    gFlagIrq = 0u; 
    ADC_SoftwareStartConvCmd(ENABLE);   //ʹ�����ת��
  }
}


/*******************************************************
* �������ƣ�CurrentSample
* ������������������
* ���������void
* ���������void
*******************************************************/
void CurrentSample(void)
{
  uint16_t current_buff[10];
	uint8_t m;
	
	/* ���ڴ��л�ȡͨ��DMA���䵽�ڴ��AD���� */
	for (m=0; m<10; m++)
	{
		current_buff[m] = gAdcResult[m][3];    /* ����ADֵ�������е�λ����gAdcResult[][3] */
	}
	
	gCurrentValue = CurrentCalculate(current_buff);
	
	if (gCurrentValue < 10)              /* �����������0.5Aʱ�����Ϊ0 */
	{
		gCurrentValue = 0;
	}
  
}

/*******************************************************
* �������ƣ�CurrentCalculate
* ��������������ֵ����
* ���������pdata������ָ��
* ����������ɼ��ĵ���ֵ
*******************************************************/
uint16_t CurrentCalculate(uint16_t *pdata)
{
	uint16_t CurrentSum = 0;
	uint16_t CurrentMax = 0;
	uint16_t CurentMin = 0xFFFF;
	uint16_t average = 0;
	float temp = 0;
	uint8_t i = 0;
	
	for (i=0; i<10; i++)
	{
		if (CurentMin > *(pdata + i))    /* �ҳ���С���� */
		{
			CurentMin = *(pdata + i);
		}
		
		if (CurrentMax < *(pdata + i))    /* �ҳ������� */
		{
			CurrentMax = *(pdata + i);
		}
		
		CurrentSum += *(pdata + i); 
	}
	
	/* ��ƽ�� */
	average = (CurrentSum - (CurentMin + CurrentMax)) >> 3;    /* ����3λ������8 */
	
	/* ADת�� */
	temp = (float)average * CURRENT_CONVERSION_VALUE;
	
	return (uint16_t)temp;
}

/*******************************************************
* �������ƣ�VoltageSample
* ������������ѹ����
* ���������void
* ���������void
*******************************************************/
void VoltageSample(void)
{
	uint16_t bat_voltage_buff[10];
	uint16_t inner_voltage_buff[10];
	uint8_t m;
	
	/* ��ȡ��ص�ѹ */
	for (m=0; m<10; m++)
	{
		bat_voltage_buff[m] = gAdcResult[m][1];      /* B+  --PA5 */
	}
	
	/* ��ȡ�����ڲ���ѹֵ */
	for (m=0; m<10; m++)
	{
		inner_voltage_buff[m] = gAdcResult[m][2];    /* B-  --PA6 */
	}
	
	
	gtemp_VoltageValue = VoltageCalculate(bat_voltage_buff);
	if (gtemp_VoltageValue <= 10)
	{
		gtemp_VoltageValue = 0;
	}
	
	gInner_VoltageValue = VoltageCalculate(inner_voltage_buff);
  
	if (gInner_VoltageValue <= 10)
	{
		gInner_VoltageValue = 0;
	}
	
  
  if(gtemp_VoltageValue>gInner_VoltageValue)
  {
     FJBH_flag=1;
     gBat_VoltageValue=0;
  }else{   
    gBat_VoltageValue=gInner_VoltageValue-gtemp_VoltageValue;
    FJBH_flag=0;
  }

}

/*******************************************************
* �������ƣ�VoltageCalculate
* ���������������ѹ����
* ���������pdata������ָ��
* ����������ɼ��ĵ�ѹֵ
*******************************************************/
uint16_t VoltageCalculate(uint16_t *pdata)
{
	uint16_t VoltageSum = 0;
	uint16_t VoltageMax = 0;
	uint16_t VoltageMin = 0xFFFF;
	uint16_t average = 0;
	float temp = 0;
	uint8_t i = 0;
	
	for (i=0; i<10; i++)
	{
		if (VoltageMin > *(pdata + i))    /* �ҳ���С���� */
		{
			VoltageMin = *(pdata + i);
		}
		
		if (VoltageMax < *(pdata + i))    /* �ҳ������� */
		{
			VoltageMax = *(pdata + i);
		}
		
		VoltageSum += *(pdata + i); 
	}
	/* ��ʼ��ƽ�� */
	average = (VoltageSum - (VoltageMin + VoltageMax)) >> 3;    /* ����3λ������8 */
	/* AD����ֵת��Ϊʵ�ʵ�ѹֵ */
	temp = (float)average * VOLTAGE_CONVERSION_VALUE;
	
	return (uint16_t)temp;
}



void TemperatureSample(void)
{
	uint16_t temperature_buff[8];
	uint16_t average = 0;
	uint16_t sum = 0;
	uint8_t m;

  
	
	for (m=0; m<8; m++)
	{
		temperature_buff[m] = gAdcResult[m][0];    /* �ڲ��¶�ADֵ�������е�λ����dma_to_adc_buff[][1] */
		sum += temperature_buff[m];
	}
	average = sum >> 3;                               /* ����8 */
	gTemperature_ADValue = average;                   /* ��ADֵ�ŵ�ȫ�ֱ��� */
	
  gTemperature = calculateTemperature(gTemperature_ADValue);
  
  if(gTemperature<=1)
  {
    gTemperature=0;
  
  }
 
}

// ���� ADC ֵ�����¶�
float calculateTemperature(int adcValue) {
    return A * adcValue + B;
}
