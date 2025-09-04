#include "User_charger.h"


/************************ ȫ�ֱ������� ************************/
sCHGType gChgType = {PREC};    /* ���״̬ */

PWMControl pwmLow, pwmHigh;
int step = 100;  // ����

BatteryVoltageTable BatV = {
    .Sealed = {14.5f, 28.8f},    // Ǧ����
    .Li_ion = {14.6f, 29.6f},    // ﮵��
    .AGM = {14.8f, 29.4f}        // AGM���
};

/*******************************************************
* �������ƣ�ChargeStart
* �����������������
* �����������
* �����������
*******************************************************/
void ChargeStart(void)
{
//  if(BATone_flag==1){//ȷ���������������
    OUT_ON;
    SD_ON;
//  }else{
//  	OUT_OFF;
//	  SD_OFF;
//  }
}

void ChargeStart1(void)
{
   OUT_OFF;

   SD_ON;
  
}
/*******************************************************
* �������ƣ�ChargeStop
* ����������ֹͣ���
* �����������
* �����������
*******************************************************/
void ChargeStop(void)
{
	OUT_OFF;
  
	SD_OFF;

}
/*******************************************************
* �������ƣ�ChargeStop
* ����������ֹͣ���
* �����������
* �����������
*******************************************************/
void ControlPWM_Init(void)
{
 //��ʼ�� PWM ������
 PWMControl_Init(&pwmLow, CC100PCT_PWM_VAL_24V, step);   // �͵��״̬��Ŀ�� PWM ֵ
 PWMControl_Init(&pwmHigh, CC100PCT_PWM_VAL_24V, step); // �ߵ��״̬��Ŀ�� PWM ֵ 
  
}

/*******************************************************
* �������ƣ�ChargingProgram
* ����������������
* �����������
* �����������
*******************************************************/
void ChargingProgram(void)
{
   switch(ChargerTasknum)    //ѡ��00ģʽ
     {
       
       case Standby_mode://����
               
             ChargeStop();
             ALL_LEDOFF();
        
          break;
        case Sealed_mode://Ǧ��
               
             Sealed_mode_Charger();
             Sealed_ledon();
        
          break;
      
        case Start_stop_mode://AGM
          
             Start_stop_mode_Charger();
             Start_stop_ledon();
        
          break;
        
        case Li_battery_mode://﮵�
          
             Li_battery_mode_Charger();
             Motorcycle_ledon();
        
          break;
      
        case SIX_mode://�޸�
           SIX_mode_Charger();
           SIX_ledon();
          break;
        
        case Repair_mode://�޸�
          
             Repair_mode_Charger();
        
             ALL_LEDOFF();
        
             if(gFaultType.Now!=FAULT_NULL)
             {
                ChargerTasknum=Standby_mode;
             }        
        
          break;
             
        case power_supply_mode://12V����ģʽ
                  
             Power_supply_mode_Charger();
        
             ALL_LEDOFF();
          
          break;
      }
}

/*******************************************************
* �������ƣ�Sealed_mode_Charger
* ����������Ǧ����
* �����������
* �����������
 *******************************************************/
void Sealed_mode_Charger(void)  //������ѹ������
{

 static uint8_t bov_cnt=0,full_cnt=0,hc_cnt=0;
 static uint8_t ci=0,NULL_cnt=0,BERR_CNT=0;
  
  switch(gChgType.Now)
  {    
    
    case WAIT_BATT:

      ChargeStop();
      ci=0;

      if(gFaultType.Now==FAULT_NULL)   //�ȴ��޴���
      {
       gChgType.Now=PREC;
      }        
    break;
    case PREC:
      
      ChargeStop();
      ci=0;

      SetChargerVoltage1(VOLTAGE_PWM_12V);//����ѹPWM
    
      SetChargeCurrent(PWM_VAL_2A);//������PWM
    
      if(gBat_VoltageValue >= 1500)
      {
        bov_cnt=0; 
        return; 
      }else{
         bov_cnt++;
      }
      
      if(bov_cnt>20)
      gChgType.Now=CCC1;
      
      break;
      
    case CCC1:
        
        ChargeStart();            //�������
    
        if(BAT_flag==1)
        {
          SetChargerVoltage1(BatV.Sealed.Battery_24V*PWM_24V);//����ѹPWM      
          /*  �������崦�� */
          if(ci>0){SetChargeCurrent(PWM_VAL_2A);}
          if(ci>5){SetChargeCurrent(CC100PCT_PWM_VAL_24V);}  
          if(ci<6){ci++;}
          
        }else{
          
          SetChargerVoltage1(BatV.Sealed.Battery_12V*PWM_12V);//����ѹPWM
          
          /*  �������崦�� */
          if(ci>0){SetChargeCurrent(PWM_VAL_2A);}
          if(ci>5){SetChargeCurrent(CC100PCT_PWM_VAL_24V);}  
          if(ci>10){SetChargeCurrent(CC100PCT_PWM_VAL_12V);}  
          if(ci<12){ci++;}
       
        }

        if(gCurrentValue<50)
          {
            
            full_cnt++;
            if(full_cnt>100)
            {
             ci=0;
             gChgType.Now=CHG_FULL;
            }
          }else{
            full_cnt=0;
          }
       
      break;
          
    case CHG_FULL:
     
         ChargeStop(); //���������Ƶ���
         SetChargerVoltage1(VOLTAGE_PWM_12V);//����ѹPWM
         SetChargeCurrent(PWM_VAL_2A);//������PWM
         bov_cnt=0,full_cnt=0;
    
        if(BAT_flag==1)
        {
          if(gBat_VoltageValue<2600) //���³�
          {
            hc_cnt++;
           
            if(hc_cnt>100){   
                gChgType.Now=HC_NULL;
                full_cnt=0;
        
            }
          }else{
               hc_cnt=0;
            }
        }else{
            if(gBat_VoltageValue<1200) //���³�
            {
              hc_cnt++;
              if(hc_cnt>100){   
                  gChgType.Now=HC_NULL;
                  full_cnt=0;
          
              }
            }else{
               hc_cnt=0;
            }
      }
      
      if(gBat_VoltageValue<500) //��ش���
       {
           if(BERR_CNT++>50){          
             BERR_flag=1;
           }
       }       
       else
       {
            BERR_CNT=0;
       }
        
      break; 
      
    case HC_NULL:
      
        ChargeStart();            //�������
    
        if(NULL_cnt>10)
        {
        
          if(BAT_flag==1)
          {
            SetChargerVoltage1(BatV.Sealed.Battery_24V*PWM_24V);//����ѹPWM      
            /*  �������崦�� */
            if(ci>0){SetChargeCurrent(PWM_VAL_2A);}
            if(ci>5){SetChargeCurrent(CC100PCT_PWM_VAL_24V);}  
            if(ci<6){ci++;}
          }else{
            SetChargerVoltage1(BatV.Sealed.Battery_12V*PWM_12V);//����ѹPWM
            
            /*  �������崦�� */
            if(ci>0){SetChargeCurrent(PWM_VAL_2A);}
            if(ci>5){SetChargeCurrent(CC100PCT_PWM_VAL_24V);}  
            if(ci>10){SetChargeCurrent(CC100PCT_PWM_VAL_12V);}  
            if(ci<12){ci++;}
          }
        }else{
          NULL_cnt++;
        }

        if(gCurrentValue<50)
          {
            
            full_cnt++;
            if(full_cnt>100)
            {
             ci=0;
             gChgType.Now=CHG_FULL;
            }
          }else{
            full_cnt=0;
          }
          
      break;       
   }

  
}
/*******************************************************
* �������ƣ�Start_stop_mode_Charger
* ������������ͣ����
* �����������
* �����������
*******************************************************/
void Start_stop_mode_Charger(void)
{
 static uint8_t bov_cnt,full_cnt=0,hc_cnt=0;
 static uint8_t ci=0,NULL_cnt=0,BERR_CNT=0;  // ��ǰֵ��ʼΪ0

  switch(gChgType.Now)
  {    
    case WAIT_BATT:
       ci=0;
       ChargeStop(); 
       if(gFaultType.Now==FAULT_NULL)   //�ȴ��޴���
       {
         gChgType.Now=PREC;
       }        
      break;
    case PREC:
      
      ChargeStop(); //���������Ƶ���
      ci=0;
      SetChargerVoltage(VOLTAGE_PWM_12V);//����ѹPWM
      SetChargeCurrent(PWM_VAL_2A);//������PWM
    
    
      if(gBat_VoltageValue >= 1500)
      {
        bov_cnt=0; 
        return; 
      }else{
         bov_cnt++;
      }
      
      if(bov_cnt>20)
        
      gChgType.Now=CCC1;
      
      break;
      
    case CCC1:
      
        ChargeStart();            //�������
    
        if(BAT_flag==1)
        {
          SetChargerVoltage1(BatV.AGM.Battery_24V*PWM_24V);//����ѹPWM      
          /*  �������崦�� */
          if(ci>0){SetChargeCurrent(PWM_VAL_2A);}
          if(ci>5){SetChargeCurrent(CC100PCT_PWM_VAL_24V);}  
          if(ci<6){ci++;}

        }else{
          SetChargerVoltage1(BatV.AGM.Battery_12V*PWM_12V);//����ѹPWM
          
          /*  �������崦�� */
          if(ci>0){SetChargeCurrent(PWM_VAL_2A);}
          if(ci>5){SetChargeCurrent(CC100PCT_PWM_VAL_24V);}  
          if(ci>10){SetChargeCurrent(CC100PCT_PWM_VAL_12V);}  
          if(ci<12){ci++;}

        }

        if(gCurrentValue<50)
          {
            
            full_cnt++;
            if(full_cnt>100)
            {
             ci=0;
             gChgType.Now=CHG_FULL;
            }
          }else{
            full_cnt=0;
          }
       
      break;
          
    case CHG_FULL:
      
         ChargeStop(); //���������Ƶ���
         
         SetChargerVoltage(VOLTAGE_PWM_12V);//����ѹPWM
         SetChargeCurrent(PWM_VAL_2A);//������PWM
         
         bov_cnt=0,full_cnt=0;
    
        if(BAT_flag==1)
        {
          if(gBat_VoltageValue<2600) //���³�
          {
            hc_cnt++;
            if(hc_cnt>100){   
                gChgType.Now=HC_NULL;
              full_cnt=0;
        
            }
          }else{
               hc_cnt=0;
            }
        }else{
            if(gBat_VoltageValue<1300) //���³�
            {
              hc_cnt++;
              if(hc_cnt>100){   
                  gChgType.Now=HC_NULL;
                full_cnt=0;
          
              }
            }else{
               hc_cnt=0;
            }
       }
        
       if(gBat_VoltageValue<500) //��ش���
       {
           if(BERR_CNT++>50){          
             BERR_flag=1;
           }
       }       
       else
       {
            BERR_CNT=0;
       }
       
    case HC_NULL:
           
        ChargeStart();            //�������
    
      if(NULL_cnt>10)
        {
          if(BAT_flag==1)
          {
            SetChargerVoltage1(BatV.AGM.Battery_24V*PWM_24V);//����ѹPWM      
            /*  �������崦�� */
            if(ci>0){SetChargeCurrent(PWM_VAL_2A);}
            if(ci>5){SetChargeCurrent(CC100PCT_PWM_VAL_24V);}  
            if(ci<6){ci++;}

          }else{
            SetChargerVoltage1(BatV.AGM.Battery_12V*PWM_12V);//����ѹPWM
            
            /*  �������崦�� */
            if(ci>0){SetChargeCurrent(PWM_VAL_2A);}
            if(ci>5){SetChargeCurrent(CC100PCT_PWM_VAL_24V);}  
            if(ci>10){SetChargeCurrent(CC100PCT_PWM_VAL_12V);}  
            if(ci<12){ci++;}
            
          }         
        }else{
          NULL_cnt++;
        }
        

        if(gCurrentValue<50)
        {
          
          full_cnt++;
          if(full_cnt>100)
          {
           ci=0;
           gChgType.Now=CHG_FULL;
          }
        }else{
          full_cnt=0;
        }
       
      
      break;  
   }
}
/*******************************************************
* �������ƣ�﮵�ģʽ
* ����������Ħ�г����� Ħ�г����2A
* �����������
* �����������
*******************************************************/
void Li_battery_mode_Charger(void)
{
 static uint8_t bov_cnt=0,full_cnt=0,hc_cnt=0,NULL_cnt=0,ci=0;
  

  switch(gChgType.Now)
  {    
    case WAIT_BATT:
      
      ci=0;
      ChargeStop(); 
      if(gFaultType.Now==FAULT_NULL)   //�ȴ��޴���
      {
       gChgType.Now=PREC;
      }        
    break;
    case PREC:
      ci=0;
      ChargeStop(); //���������Ƶ���
      SetChargerVoltage(VOLTAGE_PWM_12V);//����ѹPWM
      SetChargeCurrent(PWM_VAL_2A);//������PWM
    
      if(gBat_VoltageValue >= 1500)
      {
        bov_cnt=0; 
        return; 
      }else{
         bov_cnt++;
      }
      
      if(bov_cnt>10)
      gChgType.Now=CCC1;
      
      break;
      
    case CCC1:
      
    
        ChargeStart();            //�������
    
        if(BAT_flag==1)
        {
          SetChargerVoltage(BatV.Li_ion.Battery_24V*PWM_24V);//����ѹPWM      
          /*  �������崦�� */
          if(ci>0){SetChargeCurrent(PWM_VAL_2A);}
          if(ci>5){SetChargeCurrent(CC100PCT_PWM_VAL_24V);}  
          if(ci<6){ci++;}

        }else{
          SetChargerVoltage(BatV.Li_ion.Battery_12V*PWM_12V);//����ѹPWM
          
          /*  �������崦�� */
          if(ci>0){SetChargeCurrent(PWM_VAL_2A);}
          if(ci>5){SetChargeCurrent(CC100PCT_PWM_VAL_24V);}  
          if(ci>10){SetChargeCurrent(CC100PCT_PWM_VAL_12V);}  
          if(ci<12){ci++;}

        }

          if(gCurrentValue<50)
          {
            full_cnt++;
            if(full_cnt>100)
            {
             gChgType.Now=CHG_FULL;
            }
          }else{
            full_cnt=0;
          }
       
      break;
          
    case CHG_FULL:
         ChargeStop(); //���������Ƶ���
    
         SetChargerVoltage(VOLTAGE_PWM_12V);//����ѹPWM
         SetChargeCurrent(PWM_VAL_2A);//������PWM
      
         bov_cnt=0,full_cnt=0,NULL_cnt=0;
    
        if(BAT_flag==1)
        {
          if(gBat_VoltageValue<2600) //���³�
          {
            hc_cnt++;
            if(hc_cnt>100){   
                gChgType.Now=HC_NULL;
              full_cnt=0;
        
            }
          }else{
               hc_cnt=0;
            }
        }else{
            if(gBat_VoltageValue<1300) //���³�
            {
              hc_cnt++;
              if(hc_cnt>100){   
                  gChgType.Now=HC_NULL;
                full_cnt=0;
          
              }
            }else{
               hc_cnt=0;
            }
      }      
      break;


    case HC_NULL:
      
        ChargeStart();            //�������
        if(NULL_cnt>10){
        if(BAT_flag==1)
        {
          SetChargerVoltage(BatV.Li_ion.Battery_24V*PWM_24V);//����ѹPWM      
          /*  �������崦�� */
          if(ci>0){SetChargeCurrent(PWM_VAL_2A);}
          if(ci>5){SetChargeCurrent(CC100PCT_PWM_VAL_24V);}  
          if(ci<6){ci++;}

        }else{
          SetChargerVoltage(BatV.Li_ion.Battery_12V*PWM_12V);//����ѹPWM
          
          /*  �������崦�� */
          if(ci>0){SetChargeCurrent(PWM_VAL_2A);}
          if(ci>5){SetChargeCurrent(CC100PCT_PWM_VAL_24V);}  
          if(ci>10){SetChargeCurrent(CC100PCT_PWM_VAL_12V);}  
          if(ci<12){ci++;}

        }
      }else{
        NULL_cnt++;
      }

      if(gCurrentValue<50)
          {
            full_cnt++;
            if(full_cnt>100)
            {
             gChgType.Now=CHG_FULL;
            }
          }else{
            full_cnt=0;
          }
       
      break;      
   }
  
}

/*******************************************************
* �������ƣ�Repair_mode_Charger
* �����������޸����� ���� �������5A
* �����������
* �����������
*******************************************************/
void Repair_mode_Charger(void)
{
//  int pwm24Value=0,pwm12Value=0;
  static uint8_t bov_cnt=0,full_cnt=0,i=0;
  
  switch(gChgType.Now)
  {      
    case WAIT_BATT:
      
       ChargeStop(); 
       if(gFaultType.Now==FAULT_NULL)   //�ȴ��޴���
       {
         gChgType.Now=PREC;
       }        
      break;  
    case PREC:
      ChargeStop(); //���������Ƶ���
      SetChargerVoltage (VOLTAGE_PWM_12V);//����ѹPWM
      SetChargeCurrent(PWM_VAL_2A);//������PWM
      if(gBat_VoltageValue >= 2900)
      {
        bov_cnt=0; 
        return; 
      }else{
         bov_cnt++;
      }
      
      if(bov_cnt>10)
      gChgType.Now=CCC1;
      
      break;
      
    case CCC1:
      
        ChargeStart();            //�������
          
        if(BAT_flag==1)
        {
          SetChargerVoltage(VOLTAGE_PWM_24V);//����ѹPWM
           
         //pwm24Value = PWMControl_Update(&pwmLow);//����
          
          if(i<10){
            SetChargeCurrent(CC100PCT_PWM_VAL_24V);//������PWM
            i++;
          }else if(i<20)
          {
            i++;
            SetChargeCurrent(0);
          }else{i=0;}
          
         }else{
           
          SetChargerVoltage(VOLTAGE_PWM_12V);//����ѹPWM
           
//          pwm12Value = PWMControl_Update(&pwmLow);
//           
//          SetChargeCurrent(pwm12Value);//������PWM
           
         //һ���л�һ�ε���
         if(i<10){
            SetChargeCurrent(CC100PCT_PWM_VAL_24V);//������PWM
            i++;
          }else if(i<20){
            i++;
            SetChargeCurrent(0);
          }else{i=0;}
          
         }

          if(gCurrentValue<50)
          {
            full_cnt++;
            if(full_cnt>100)
            {
             gChgType.Now=CHG_FULL;
            }
          }else{
            full_cnt=0;
          }
       
      break;
          
    case CHG_FULL:
      
      ChargeStop(); //���������Ƶ���
      SetChargerVoltage(VOLTAGE_PWM_12V);//����ѹPWM
      
      SetChargeCurrent(PWM_VAL_2A);
      
      bov_cnt=0,full_cnt=0;
    
      break;
      
    case HC_NULL:
      
      break;       
   }
  
}


/*******************************************************
* �������ƣ�12V����ģʽ
* ����������ǿ��12V�����10-11A��������
* �����������
* �����������
*******************************************************/
void Power_supply_mode_Charger(void)
{
  static uint8_t i=0;
  switch(gChgType.Now)
  {      
    case WAIT_BATT:
      
       ChargeStop(); 
       if(gFaultType.Now==FAULT_NULL)   //�ȴ��޴���
       {
         gChgType.Now=PREC;
       }        
      break;  
       
    case PREC:
      ChargeStop(); //���������Ƶ���
      SetChargerVoltage (PWM_12V*13.6);//����ѹPWM
      SetChargeCurrent(PWM_VAL_2A);//������PWM
      
      gChgType.Now=CCC1;
      
      break;
      
    case CCC1:
      
        ChargeStart();            //�������
          
        SetChargerVoltage(PWM_12V*13.6);//����ѹPWM
            
        /*  �������崦�� */
        if(i>0){SetChargeCurrent(PWM_VAL_2A);}
        if(i>5){SetChargeCurrent(CC100PCT_PWM_VAL_24V);}  
        if(i>10){SetChargeCurrent(Power_PWM_VAL);}  
        
        if(i<12){i++;}
        
      break;
          
    case CHG_FULL:
      
      ChargeStop(); //���������Ƶ���
      SetChargerVoltage(PWM_12V*13.6);//����ѹPWM   
      SetChargeCurrent(PWM_VAL_2A);
          
      break;
      
    case HC_NULL:
      
      break;       
   }
  
}


/*******************************************************
* �������ƣ�SIX_mode_Charger
* ����������6V��ع���
* �����������
* �����������
*******************************************************/
void SIX_mode_Charger(void)
{
 static uint8_t full_cnt=0;

  switch(gChgType.Now)
  {    
    case WAIT_BATT:

      ChargeStop(); 
      if(gFaultType.Now==FAULT_NULL)   //�ȴ��޴���
      {
       gChgType.Now=PREC;
      }        
    break;
    case PREC:
      
      ChargeStart(); //���������Ƶ���
      SetChargerVoltage(VOLTAGE_PWM_6V);//����ѹPWM
      SetChargeCurrent(PWM_VAL_2A);//������PWM
    
      gChgType.Now=CCC1;
      
      break;
      
    case CCC1:
      
        ChargeStart();            //�������
    
        SetChargerVoltage(VOLTAGE_PWM_6V);//����ѹPWM
        SetChargeCurrent(CC100PCT_PWM_VAL_12V);//������PWM

        if(gCurrentValue<50)
          {
            full_cnt++;
            if(full_cnt>100)
            {
             gChgType.Now=CHG_FULL;
            }
          }else{
            full_cnt=0;
          }
       
      break;
          
    case CHG_FULL:
         ChargeStop(); //���������Ƶ���
         full_cnt=0;
         SetChargerVoltage(VOLTAGE_PWM_6V);//����ѹPWM
         SetChargeCurrent(PWM_VAL_2A);//������PWM
      
      break;


    case HC_NULL:
       
      break;      
   }
  
}


// ��ʼ�� PWM ������
void PWMControl_Init(PWMControl *pwm, int targetValue, int step) {
    pwm->currentValue = 0;
    pwm->targetValue = targetValue;
    pwm->step = step;
    pwm->state = RISING;
}

// ÿ�ε��õ���PWMֵ�������µ�PWMֵ
int PWMControl_Update(PWMControl *pwm) {
    if (pwm->state == RISING) {
        // �����׶�
        pwm->currentValue += pwm->step;
        if (pwm->currentValue >= pwm->targetValue) {
            pwm->currentValue = pwm->targetValue;
            pwm->state = FALLING;  // �ﵽĿ��ֵ���л����½��׶�
        }
    } else if (pwm->state == FALLING) {
        // �½��׶�
        pwm->currentValue -= pwm->step;
        if (pwm->currentValue <= 0) {
            pwm->currentValue = 0;
            pwm->state = RISING;  // �ص� 0 ���л��������׶Σ�ѭ����
        }
    }
    return pwm->currentValue;
}


// ÿ�����ӵĲ���ֵ
#define STEP 50

// �����ӵ�Ŀ��ֵ�ĺ���
uint16_t rampUpToTarget(uint16_t current, uint16_t target) {
    if (current < target) {
        current += STEP;
        if (current > target) {
            current = target;  // ��ֹ����Ŀ��ֵ
        }
    }
    return current;
}
