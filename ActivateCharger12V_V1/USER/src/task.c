#include "task.h"

/************************ 全局变量定义 ************************/
uint32_t gTaskDelay[TASK_MAX] = {0};


/*******************************************************
* 函数名称：TaskDelayInit
* 功能描述：任务延时变量初始化
* 输入参数：无
* 输出参数：无
*******************************************************/
void TaskDelayInit(void)
{
	uint8_t i = 0;
	
	for (i=0; i<TASK_MAX; i++)
	{
		gTaskDelay[i] = 1;
	}
  gTaskDelay[0] = TASK_DELAY_0;
	gTaskDelay[1] = TASK_DELAY_1;
	gTaskDelay[2] = TASK_DELAY_2;
	gTaskDelay[3] = TASK_DELAY_3;
	gTaskDelay[4] = TASK_DELAY_4;
	gTaskDelay[5] = 200;
	gTaskDelay[6] = TASK_DELAY_6;

}

/*******************************************************
* 函数名称：Task0    20ms
* 功能描述：AC电压采集、输出电压采集
* 输入参数：无
* 输出参数：无
*******************************************************/
void Task0(void)
{
	
  User_ADC_Sample();
  
  gTaskDelay[0] = TASK_DELAY_0;
}

/*******************************************************
* 函数名称：Task1    20ms
* 功能描述：采样
* 输入参数：无
* 输出参数：无
*******************************************************/
void Task1(void)
{
  CurrentSample();
  VoltageSample();
  TemperatureSample();
  
  gTaskDelay[1] = TASK_DELAY_1;
}

/*******************************************************
* 函数名称：Task2    200ms
* 功能描述：故障检测
* 输入参数：无
* 输出参数：无
*******************************************************/
void Task2(void)
{
  

  protect();
//  gFaultType.Now=FAULT_NULL;
  
  gTaskDelay[2] = TASK_DELAY_2;
}

/*******************************************************
* 函数名称：Task3    10ms
* 功能描述：按键检测
* 输入参数：无
* 输出参数：无
*******************************************************/
void Task3(void)
{

	ButtonTask();
  
  gTaskDelay[3] = TASK_DELAY_3;
}

/*******************************************************
* 函数名称：Task4    200ms
* 功能描述：充电进程
* 输入参数：无
* 输出参数：无
*******************************************************/
void Task4(void)
{
  static uint8_t inspect_cnt=0;
  if(inspect_cnt>10)
  {
    ChargingProgram();
  }else{            //开机自检1秒
      inspect_cnt++;
      ChargeStop(); //开机不控制电流

      SetChargerVoltage(VOLTAGE_PWM_24V);//最大电压PWM
      SetChargeCurrent(PWM_VAL_2A);//最大电流PWM
  }
  
  gTaskDelay[4] = TASK_DELAY_4;
}
 
/*******************************************************
* 函数名称：Task5    2000ms
* 功能描述：LCD刷新
* 输入参数：无
* 输出参数：无
*******************************************************/
Tasknum current_mode=Standby_mode;

void Task5(void)
{
   
  Screen_refresh(); 
   
  gTaskDelay[5] = TASK_DELAY_5;
}


/*******************************************************
* 函数名称：Task6    200ms
* 功能描述：
* 输入参数：无
* 输出参数：无
*******************************************************/
void Task6(void)
{

//	printf("gBat_VoltageValue=%d\r\n",gBat_VoltageValue);
//  printf("gtemp_VoltageValue=%d\r\n",gtemp_VoltageValue);
//  printf("gCurrentValue=%d\r\n",gCurrentValue);
//  printf("gFaultType.Now=%02X\r\n",gFaultType.Now);
//  printf("ChargerTasknum=%d\r\n",ChargerTasknum);
	gTaskDelay[6] = TASK_DELAY_6;
}

/*******************************************************
* 函数名称：print_buff    
* 功能描述：打印二维数组
* 输入参数：无
* 输出参数：无
*******************************************************/
void print_buff(uint16_t * arr, int row, int col)
{
	int i = 0;
	int j = 0;
	for (i = 0; i < row; i++)
	{
		for (j = 0; j < col; j++)
		{
			printf("%d\r\n ", arr[i*col+j]);
		}
		printf("\r\n");
	}
}

