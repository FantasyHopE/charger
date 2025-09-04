#include "User_adc.h"

/************************** 全局变量定义 *************************/
uint16_t gtemp_VoltageValue=0;      /* B-电压值       */
uint16_t gBat_VoltageValue=0;      /* 电池电压值       */
uint16_t gInner_VoltageValue=0;    /* B+电压值 */
uint16_t gCurrentValue=0;          /* 输出电流 */
uint16_t gTemperature_ADValue=0;   /* 温度AD值 */
float    gTemperature=0;           /* 温度 */

uint16_t gAdcResult[10][4]={0};    //转换数组
uint8_t gFlagIrq;

uint8_t  FJBH_flag=0;


/*******************************************************
* 函数名称：User_adc_init
* 功能描述：ADC 初始化
* 输入参数：void
* 输出参数：void
*******************************************************/
void User_adc_init(void)
{

    /********配置ADC输入端口********/
    //打开GPIO时钟
    __SYSCTRL_GPIOA_CLK_ENABLE();
    __SYSCTRL_GPIOB_CLK_ENABLE();
    //打开ADC时钟
    __SYSCTRL_ADC_CLK_ENABLE();

    //set PA04 as AIN4 INPUT  温度
    PA04_ANALOG_ENABLE();
    //set PA05 as AIN5 INPUT   BAT-
    PA05_ANALOG_ENABLE();
    //set PA06 as AIN6 INPUT   BAT+
    PA06_ANALOG_ENABLE();

    //set PB03 as AIN10 INPUT
    PB03_ANALOG_ENABLE();   //使能PB03 模拟转换   电流

    /********配置ADC输入端口********/

    ADC_InitTypeDef ADC_InitStructure = {0};


    //ADC序列转换模式配置
    ADC_InitStructure.ADC_ClkDiv = ADC_Clk_Div1;    // PCLK/2   6mhz
    ADC_InitStructure.ADC_ConvertMode = ADC_ConvertMode_Continuous;
    ADC_InitStructure.ADC_SQREns = ADC_SqrEns0to3; //使能四通道
    
    ADC_InitStructure.ADC_IN0.ADC_InputChannel = ADC_InputCH4;   //选择 8通道 对应的  16个ADC输入通道   
    ADC_InitStructure.ADC_IN0.ADC_SampTime = ADC_SampTime30Clk;  //采样时间
    

    ADC_InitStructure.ADC_IN1.ADC_InputChannel = ADC_InputCH5;
    ADC_InitStructure.ADC_IN1.ADC_SampTime = ADC_SampTime42Clk;
    
    ADC_InitStructure.ADC_IN2.ADC_InputChannel = ADC_InputCH6;
    ADC_InitStructure.ADC_IN2.ADC_SampTime = ADC_SampTime42Clk;
    
    ADC_InitStructure.ADC_IN3.ADC_InputChannel = ADC_InputCH10;   //pb3
    ADC_InitStructure.ADC_IN3.ADC_SampTime = ADC_SampTime54Clk;
    


    ADC_Init(&ADC_InitStructure);
  
    ADC_ClearITPendingAll();//清除所有中断
    
    ADC_ITConfig(ADC_IT_EOS, ENABLE);//使能转换完成中断
    NVIC_EnableIRQ(ADC_IRQn);
    //ADC使能
    ADC_Enable();
    ADC_SoftwareStartConvCmd(ENABLE);//ADC转换软件启动

}

/*******************************************************
* 函数名称：CurrentSample
* 功能描述：电流采样
* 输入参数：void
* 输出参数：void
*******************************************************/
void User_ADC_Sample(void)
{
  static uint8_t i=0;
  if (gFlagIrq & ADC_ISR_EOS_Msk)
  {
    ADC_SoftwareStartConvCmd(DISABLE); //失能软件转换

    ADC_GetSqr0Result(&gAdcResult[i][0]); //温度
    ADC_GetSqr1Result(&gAdcResult[i][1]); //BAT-
    ADC_GetSqr2Result(&gAdcResult[i][2]); //BAT+
    ADC_GetSqr3Result(&gAdcResult[i][3]); //电流
    
    i++;
    i%=10;//转换10次
    gFlagIrq = 0u; 
    ADC_SoftwareStartConvCmd(ENABLE);   //使能软件转换
  }
}


/*******************************************************
* 函数名称：CurrentSample
* 功能描述：电流采样
* 输入参数：void
* 输出参数：void
*******************************************************/
void CurrentSample(void)
{
  uint16_t current_buff[10];
	uint8_t m;
	
	/* 从内存中获取通过DMA传输到内存的AD数据 */
	for (m=0; m<10; m++)
	{
		current_buff[m] = gAdcResult[m][3];    /* 电流AD值在数组中的位置是gAdcResult[][3] */
	}
	
	gCurrentValue = CurrentCalculate(current_buff);
	
	if (gCurrentValue < 10)              /* 输出电流低于0.5A时算电流为0 */
	{
		gCurrentValue = 0;
	}
  
}

/*******************************************************
* 函数名称：CurrentCalculate
* 功能描述：电流值计算
* 输入参数：pdata：数据指针
* 输出参数：采集的电流值
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
		if (CurentMin > *(pdata + i))    /* 找出最小的数 */
		{
			CurentMin = *(pdata + i);
		}
		
		if (CurrentMax < *(pdata + i))    /* 找出最大的数 */
		{
			CurrentMax = *(pdata + i);
		}
		
		CurrentSum += *(pdata + i); 
	}
	
	/* 求平均 */
	average = (CurrentSum - (CurentMin + CurrentMax)) >> 3;    /* 右移3位即除以8 */
	
	/* AD转换 */
	temp = (float)average * CURRENT_CONVERSION_VALUE;
	
	return (uint16_t)temp;
}

/*******************************************************
* 函数名称：VoltageSample
* 功能描述：电压采样
* 输入参数：void
* 输出参数：void
*******************************************************/
void VoltageSample(void)
{
	uint16_t bat_voltage_buff[10];
	uint16_t inner_voltage_buff[10];
	uint8_t m;
	
	/* 获取电池电压 */
	for (m=0; m<10; m++)
	{
		bat_voltage_buff[m] = gAdcResult[m][1];      /* B+  --PA5 */
	}
	
	/* 获取充电机内部电压值 */
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
* 函数名称：VoltageCalculate
* 功能描述：输出电压计算
* 输入参数：pdata：数据指针
* 输出参数：采集的电压值
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
		if (VoltageMin > *(pdata + i))    /* 找出最小的数 */
		{
			VoltageMin = *(pdata + i);
		}
		
		if (VoltageMax < *(pdata + i))    /* 找出最大的数 */
		{
			VoltageMax = *(pdata + i);
		}
		
		VoltageSum += *(pdata + i); 
	}
	/* 开始求平均 */
	average = (VoltageSum - (VoltageMin + VoltageMax)) >> 3;    /* 右移3位即除以8 */
	/* AD采样值转换为实际电压值 */
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
		temperature_buff[m] = gAdcResult[m][0];    /* 内部温度AD值在数组中的位置是dma_to_adc_buff[][1] */
		sum += temperature_buff[m];
	}
	average = sum >> 3;                               /* 除以8 */
	gTemperature_ADValue = average;                   /* 将AD值放到全局变量 */
	
  gTemperature = calculateTemperature(gTemperature_ADValue);
  
  if(gTemperature<=1)
  {
    gTemperature=0;
  
  }
 
}

// 根据 ADC 值计算温度
float calculateTemperature(int adcValue) {
    return A * adcValue + B;
}
