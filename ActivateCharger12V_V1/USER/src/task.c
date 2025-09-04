#include "task.h"

/************************ ȫ�ֱ������� ************************/
uint32_t gTaskDelay[TASK_MAX] = {0};


/*******************************************************
* �������ƣ�TaskDelayInit
* ����������������ʱ������ʼ��
* �����������
* �����������
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
* �������ƣ�Task0    20ms
* ����������AC��ѹ�ɼ��������ѹ�ɼ�
* �����������
* �����������
*******************************************************/
void Task0(void)
{
	
  User_ADC_Sample();
  
  gTaskDelay[0] = TASK_DELAY_0;
}

/*******************************************************
* �������ƣ�Task1    20ms
* ��������������
* �����������
* �����������
*******************************************************/
void Task1(void)
{
  CurrentSample();
  VoltageSample();
  TemperatureSample();
  
  gTaskDelay[1] = TASK_DELAY_1;
}

/*******************************************************
* �������ƣ�Task2    200ms
* �������������ϼ��
* �����������
* �����������
*******************************************************/
void Task2(void)
{
  

  protect();
//  gFaultType.Now=FAULT_NULL;
  
  gTaskDelay[2] = TASK_DELAY_2;
}

/*******************************************************
* �������ƣ�Task3    10ms
* �����������������
* �����������
* �����������
*******************************************************/
void Task3(void)
{

	ButtonTask();
  
  gTaskDelay[3] = TASK_DELAY_3;
}

/*******************************************************
* �������ƣ�Task4    200ms
* ����������������
* �����������
* �����������
*******************************************************/
void Task4(void)
{
  static uint8_t inspect_cnt=0;
  if(inspect_cnt>10)
  {
    ChargingProgram();
  }else{            //�����Լ�1��
      inspect_cnt++;
      ChargeStop(); //���������Ƶ���

      SetChargerVoltage(VOLTAGE_PWM_24V);//����ѹPWM
      SetChargeCurrent(PWM_VAL_2A);//������PWM
  }
  
  gTaskDelay[4] = TASK_DELAY_4;
}
 
/*******************************************************
* �������ƣ�Task5    2000ms
* ����������LCDˢ��
* �����������
* �����������
*******************************************************/
Tasknum current_mode=Standby_mode;

void Task5(void)
{
   
  Screen_refresh(); 
   
  gTaskDelay[5] = TASK_DELAY_5;
}


/*******************************************************
* �������ƣ�Task6    200ms
* ����������
* �����������
* �����������
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
* �������ƣ�print_buff    
* ������������ӡ��ά����
* �����������
* �����������
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

