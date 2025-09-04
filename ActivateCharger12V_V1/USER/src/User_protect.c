#include "User_protect.h"

/************************ 全局变量定义 ************************/
sFAULTType gFaultType = {FAULT_NULL, FAULT_NULL};    /* 故障类型 */
uint8_t BAT_flag=0;                                  /* 电池类型 12V/24V 默认24V*/
uint8_t BATone_flag=0;                               /* 第一次上电记录   */
uint8_t jwen_v=0;                                   /* 升压标志位   */
uint8_t W_S_flag=0;                                 /* 温度标志 冬夏   */
uint8_t BERR_flag=0;                                  /* 电池错误标志位*/
uint8_t FaultLock = 0;



/*******************************************************
* 函数名称：FaultCheck
* 功能描述：充电器故障检测
* 输入参数：无
* 输出参数：故障类型
*******************************************************/
FAULTType FaultCheck(void)
{
  static uint8_t otp_cnt = 0,ovp_cnt = 0,ocp_cnt = 0,OVP_FLAG=0;
  static uint8_t jw_cnt = 0,fj_cnt=0;
  static uint8_t BREE_cnt = 0,mode_cnt=0;
//  static uint8_t bat_cnt[4] ={0};
  static uint8_t WDBC_ONE_FLAG=0,MOde_ONE_FLAG=0;
  
	/****************** 内部NTC温度检查 *******************/
	if ((gTemperature >= 95)||(gTemperature_z>= 100))
	{
    if(otp_cnt>20)
    {
		 return HOTP;}else{
       otp_cnt++;
     }
	}else{
     otp_cnt=0;
  }
	
	if (gFaultType.Past == HOTP)
	{
		if ((gTemperature > 70)||(gTemperature_z>= 80))
		{
			return HOTP;
		}
	}
    
 if(WDBC_ONE_FLAG==0){//判断一次温度补偿
  if(gTemperature>28)
  {
    if(jw_cnt>60)
    {
      jwen_v=1;
      WDBC_ONE_FLAG=1;
    }else{jw_cnt++;}
  }  
  else if(gTemperature<10)
  {
    if(jw_cnt>60)
    {
      jwen_v=2;
      WDBC_ONE_FLAG=1;
    }else{jw_cnt++;}
  }else{
     jw_cnt=0;
      jwen_v=0;
  }
 }

   	/******************* 电池类型检测 ********************/
//  if(BATone_flag!=1){
//      if (gBat_VoltageValue >= 1800){
//           if(bat_cnt[2]>10){
//             BATone_flag=1;
//             BAT_flag=1;
//           }else{bat_cnt[2]++;}
//        }
//      else if((gBat_VoltageValue <= 1650)&&(gBat_VoltageValue>200)){
//          if(bat_cnt[3]>10){
//             BATone_flag=1;
//             BAT_flag=0;
//           }else{bat_cnt[3]++;}
//      }
//    }
//  else{   
//        if(BAT_flag==0) //充电过程中变到24V
//        { 
//          if(gBat_VoltageValue >= 1800){

//            if(bat_cnt[0]>20)
//            {
//              SysReset();//复位
//            }else{bat_cnt[0]++;}
//          }
//          else{
//            bat_cnt[0]=0;
//          }
//        }else{       //充电过程中变到12V  //防止报错误后改变电池类型
//          if((gBat_VoltageValue <= 1650)&&(gBat_VoltageValue>200)){
//             
//              if(bat_cnt[1]>20)
//              {
//                SysReset();//复位
//              }else{bat_cnt[1]++;}
//          }else{
//            bat_cnt[1]=0;
//          }
//        }
//    }
   
      
   /**************** 电池错误 *****************/
   if(BERR_flag==1)
   {
     return BERR;
   }

     /******************短路检测 *******************/
  if(gtemp_VoltageValue<210)
  {  if(fj_cnt++>10) 
      return FJBH;
  }else if(gtemp_VoltageValue<280)//短路
  {
    if(fj_cnt++>10) 
    return FJBH;
  }else{fj_cnt=0;}
   
   
   	/**************** 未连接 *****************/
	if ((gBat_VoltageValue<=200)&&(ChargerTasknum!=power_supply_mode))
	{    
    if(BREE_cnt++>2)
    {
      return STANDBY;
    }
	}else{
     BREE_cnt=0;
  }  
  
     	/**************** 过流 *****************/
	if (gCurrentValue>=1100)
	{    
    if(ocp_cnt>25)
    {
      return COCP;
    }else{
      ocp_cnt++;
    }
	}else{
     ocp_cnt=0;
  }  
  
  
       	/**************** 过YA *****************/
	if (gBat_VoltageValue>=1700)
	{    
    if(ovp_cnt>25)
    {
      OVP_FLAG=1;
      return COVP;
    }else{
      ovp_cnt++;
    }
	}else{
     ovp_cnt=0;
  }
  
  if(OVP_FLAG){
    
  if (gBat_VoltageValue<=1500){
    OVP_FLAG=0;
  }else{
    return COVP;
   }
  }

	/******************* 电池充满 ********************/
	if ((gChgType.Now == CHG_FULL)||(gChgType.Now == HC_NULL)) //充满或回充
	{
		return FULL_CHG;
	}
  
  if(MOde_ONE_FLAG==0)//只覆盖一次
  {
   if(mode_cnt>6){  //加段延迟防止上电初始化时直接误判
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
			case BERR:
			case FJBH:
      case STANDBY:
      case COVP:  
			case COCP:
				gChgType.Now = PREC;                                                     
				break;

			default:
				break;
		}
  }else{
        if (gFaultType.Past == FAULT_NULL)             /* 防止多个故障，使gChgType.Now和gChgType.Past都为WAIT_BACK */
        {
          gChgType.Past = gChgType.Now;                /* 保存充电状态 */
        }
        switch (gFaultType.Now)
        {

          case FAULT_NULL:
          case FULL_CHG:
               break;
          case HOTP:
          case BERR:
          case FJBH:
          case STANDBY:
          case COVP:  
            gChgType.Now = WAIT_BATT;
               break;

          case COCP:
            gChgType.Now = WAIT_BATT;
            FaultLock=1;                                                         
          break;

          default:
          break;
        }
	}
	gFaultType.Past = gFaultType.Now;
}

