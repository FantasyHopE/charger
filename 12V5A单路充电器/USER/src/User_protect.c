#include "User_protect.h"


/************************ ȫ�ֱ������� ************************/
sFAULTType gFaultType = {FAULT_NULL, FAULT_NULL};    /* �������� */


/*******************************************************
* �������ƣ�FaultCheck
* ������������������ϼ��
* �����������
* �����������������
*******************************************************/
FAULTType FaultCheck(void)
{
	static uint8_t oup_cnt = 0;
	static uint8_t ocp_cnt = 0;
  static uint8_t FJBH_cnt = 0;
  static uint8_t mode_cnt=0;
//  static uint8_t bat_cnt[4] ={0};
  static bool MOde_ONE_FLAG=0;
	/******************** ���ӱ������ ********************/
	if (FJBH_flag)//PA0
	{
    if(FJBH_cnt++>30)
		return FJBH;
	}else{FJBH_cnt=0;}
  
	/****************** �ڲ�NTC�¶ȼ�� *******************/
	if (gTemperature >= 90)
	{
		return HOTP;
	}
	
	if (gFaultType.Past == HOTP)
	{
		if (gTemperature > 75)
		{
			return HOTP;
		}
	}
	
 
	/******************* ���Ƿѹ��� ********************/
	if (gBat_VoltageValue <= 100)
	{
			return BERR;
	
	}
	/**************** ���������ѹ��� *****************/
	if (gBat_VoltageValue > 2000)
	{
		if (++oup_cnt >= 8)
		{
			return COVP;
		}
	}
	else
	{
		if (oup_cnt > 0)
			oup_cnt--;
	}
  
	/**************** �������������� *****************/
	if (gCurrentValue > 700)
	{
		if (++ocp_cnt >= 8)
		{
			return COCP;
		}
	}
	else
	{
		if (ocp_cnt > 0)
			ocp_cnt--;
	}
	
	/******************* ��س��� ********************/
	if (gFaultType.Now == FULL_CHG)
	{
		return FULL_CHG;
	}
  
  
	if(MOde_ONE_FLAG==0)//ֻ����һ��
  {
   if(mode_cnt>5){  //�Ӷ��ӳٷ�ֹ�ϵ��ʼ��ʱֱ������
     ChargerTasknum=Chargermode_Flash;//У����������Ϊ�ϴεĳ��ģʽ
     MOde_ONE_FLAG=1;
   }else{
       mode_cnt++; 
   }
  }
	
	return FAULT_NULL;
}

/*******************************************************
* �������ƣ�protect
* ������������������ϴ���ͻָ�
* �����������
* �����������
*******************************************************/
void protect(void)
{
	static uint8_t i = 0;
	static bool FaultLock = 0;
  	/* ��ѹ�͹���ֱ���������ϼ�� */
	if (FaultLock)
	{
		return;
	}
  
	gFaultType.Now = FaultCheck();                   /* ��ȡ������������� */
	
	if (gFaultType.Now == gFaultType.Past)           /* ��������һ��������ִ�к������� */
	{
		return;
	}
	
	if (gFaultType.Now == FAULT_NULL)
	{
		if (++i < 5)                                  /* ��ʱ1s������ȷ���쳣���� */
		{
			return;
		}
		i = 0;
		
		switch (gFaultType.Past)
		{
			case FAULT_NULL:
			case FULL_CHG:
				break;
      
      case HOTP:
			case COVP:
			case COCP:
			case FJBH:
			case BERR:
				gChgType.Now = WAIT_BATT;
				break;					
		}
	}
	else
	{
		if (gFaultType.Past == FAULT_NULL)             /* ��ֹ������ϣ�ʹgChgType.Now��gChgType.Past��ΪWAIT_BACK */
		{
			gChgType.Past = gChgType.Now;                /* ������״̬ */
		}
		
		switch (gFaultType.Now)
		{			
			case HOTP:
			case BERR:
			case FJBH:
      case COVP:
				gChgType.Now = WAIT_BACK;
				break;

			case COCP:
				gChgType.Now = WAIT_BACK;
        FaultLock=1; 
				break;
			
			default:
				break;
		}
	}
	
	gFaultType.Past = gFaultType.Now;
}
