#include "User_protect.h"


/************************ 全局变量定义 ************************/
sFAULTType gFaultType = {FAULT_NULL, FAULT_NULL};    /* 故障类型 */


/*******************************************************
* 函数名称：FaultCheck
* 功能描述：充电器故障检测
* 输入参数：无
* 输出参数：故障类型
*******************************************************/
FAULTType FaultCheck(void)
{
	static uint8_t oup_cnt = 0;
	static uint8_t ocp_cnt = 0;
  static uint8_t FJBH_cnt = 0;
  static uint8_t mode_cnt=0;
//  static uint8_t bat_cnt[4] ={0};
  static bool MOde_ONE_FLAG=0;
	/******************** 反接保护检查 ********************/
	if (FJBH_flag)//PA0
	{
    if(FJBH_cnt++>30)
		return FJBH;
	}else{FJBH_cnt=0;}
  
	/****************** 内部NTC温度检查 *******************/
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
	
 
	/******************* 电池欠压检查 ********************/
	if (gBat_VoltageValue <= 100)
	{
			return BERR;
	
	}
	/**************** 充电机输出过压检查 *****************/
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
  
	/**************** 充电机输出过流检查 *****************/
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
	
	/******************* 电池充满 ********************/
	if (gFaultType.Now == FULL_CHG)
	{
		return FULL_CHG;
	}
  
  
	if(MOde_ONE_FLAG==0)//只覆盖一次
  {
   if(mode_cnt>5){  //加段延迟防止上电初始化时直接误判
     ChargerTasknum=Chargermode_Flash;//校验无误后更改为上次的充电模式
     MOde_ONE_FLAG=1;
   }else{
       mode_cnt++; 
   }
  }
	
	return FAULT_NULL;
}

/*******************************************************
* 函数名称：protect
* 功能描述：充电器故障处理和恢复
* 输入参数：无
* 输出参数：无
*******************************************************/
void protect(void)
{
	static uint8_t i = 0;
	static bool FaultLock = 0;
  	/* 过压和过流直接锁死故障检测 */
	if (FaultLock)
	{
		return;
	}
  
	gFaultType.Now = FaultCheck();                   /* 获取充电器故障类型 */
	
	if (gFaultType.Now == gFaultType.Past)           /* 故障类型一样，不再执行后续程序 */
	{
		return;
	}
	
	if (gFaultType.Now == FAULT_NULL)
	{
		if (++i < 5)                                  /* 延时1s，方便确认异常类型 */
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
		if (gFaultType.Past == FAULT_NULL)             /* 防止多个故障，使gChgType.Now和gChgType.Past都为WAIT_BACK */
		{
			gChgType.Past = gChgType.Now;                /* 保存充电状态 */
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
