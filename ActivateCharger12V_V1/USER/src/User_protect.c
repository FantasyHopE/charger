#include "User_protect.h"

/************************ ȫ�ֱ������� ************************/
sFAULTType gFaultType = {FAULT_NULL, FAULT_NULL};    /* �������� */
uint8_t BAT_flag=0;                                  /* ������� 12V/24V Ĭ��24V*/
uint8_t BATone_flag=0;                               /* ��һ���ϵ��¼   */
uint8_t jwen_v=0;                                   /* ��ѹ��־λ   */
uint8_t W_S_flag=0;                                 /* �¶ȱ�־ ����   */
uint8_t BERR_flag=0;                                  /* ��ش����־λ*/
uint8_t FaultLock = 0;



/*******************************************************
* �������ƣ�FaultCheck
* ������������������ϼ��
* �����������
* �����������������
*******************************************************/
FAULTType FaultCheck(void)
{
  static uint8_t otp_cnt = 0,ovp_cnt = 0,ocp_cnt = 0,OVP_FLAG=0;
  static uint8_t jw_cnt = 0,fj_cnt=0;
  static uint8_t BREE_cnt = 0,mode_cnt=0;
//  static uint8_t bat_cnt[4] ={0};
  static uint8_t WDBC_ONE_FLAG=0,MOde_ONE_FLAG=0;
  
	/****************** �ڲ�NTC�¶ȼ�� *******************/
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
    
 if(WDBC_ONE_FLAG==0){//�ж�һ���¶Ȳ���
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

   	/******************* ������ͼ�� ********************/
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
//        if(BAT_flag==0) //�������б䵽24V
//        { 
//          if(gBat_VoltageValue >= 1800){

//            if(bat_cnt[0]>20)
//            {
//              SysReset();//��λ
//            }else{bat_cnt[0]++;}
//          }
//          else{
//            bat_cnt[0]=0;
//          }
//        }else{       //�������б䵽12V  //��ֹ�������ı�������
//          if((gBat_VoltageValue <= 1650)&&(gBat_VoltageValue>200)){
//             
//              if(bat_cnt[1]>20)
//              {
//                SysReset();//��λ
//              }else{bat_cnt[1]++;}
//          }else{
//            bat_cnt[1]=0;
//          }
//        }
//    }
   
      
   /**************** ��ش��� *****************/
   if(BERR_flag==1)
   {
     return BERR;
   }

     /******************��·��� *******************/
  if(gtemp_VoltageValue<210)
  {  if(fj_cnt++>10) 
      return FJBH;
  }else if(gtemp_VoltageValue<280)//��·
  {
    if(fj_cnt++>10) 
    return FJBH;
  }else{fj_cnt=0;}
   
   
   	/**************** δ���� *****************/
	if ((gBat_VoltageValue<=200)&&(ChargerTasknum!=power_supply_mode))
	{    
    if(BREE_cnt++>2)
    {
      return STANDBY;
    }
	}else{
     BREE_cnt=0;
  }  
  
     	/**************** ���� *****************/
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
  
  
       	/**************** ��YA *****************/
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

	/******************* ��س��� ********************/
	if ((gChgType.Now == CHG_FULL)||(gChgType.Now == HC_NULL)) //������س�
	{
		return FULL_CHG;
	}
  
  if(MOde_ONE_FLAG==0)//ֻ����һ��
  {
   if(mode_cnt>6){  //�Ӷ��ӳٷ�ֹ�ϵ��ʼ��ʱֱ������
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
        if (gFaultType.Past == FAULT_NULL)             /* ��ֹ������ϣ�ʹgChgType.Now��gChgType.Past��ΪWAIT_BACK */
        {
          gChgType.Past = gChgType.Now;                /* ������״̬ */
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

