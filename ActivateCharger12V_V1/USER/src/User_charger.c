#include "User_charger.h"


/************************ 全局变量定义 ************************/
sCHGType gChgType = {PREC};    /* 充电状态 */

PWMControl pwmLow, pwmHigh;
int step = 100;  // 步长

BatteryVoltageTable BatV = {
    .Sealed = {14.5f, 28.8f},    // 铅酸电池
    .Li_ion = {14.6f, 29.6f},    // 锂电池
    .AGM = {14.8f, 29.4f}        // AGM电池
};

/*******************************************************
* 函数名称：ChargeStart
* 功能描述：启动充电
* 输入参数：无
* 输出参数：无
*******************************************************/
void ChargeStart(void)
{
//  if(BATone_flag==1){//确认完电池类型再输出
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
* 函数名称：ChargeStop
* 功能描述：停止充电
* 输入参数：无
* 输出参数：无
*******************************************************/
void ChargeStop(void)
{
	OUT_OFF;
  
	SD_OFF;

}
/*******************************************************
* 函数名称：ChargeStop
* 功能描述：停止充电
* 输入参数：无
* 输出参数：无
*******************************************************/
void ControlPWM_Init(void)
{
 //初始化 PWM 控制器
 PWMControl_Init(&pwmLow, CC100PCT_PWM_VAL_24V, step);   // 低电池状态的目标 PWM 值
 PWMControl_Init(&pwmHigh, CC100PCT_PWM_VAL_24V, step); // 高电池状态的目标 PWM 值 
  
}

/*******************************************************
* 函数名称：ChargingProgram
* 功能描述：充电进程
* 输入参数：无
* 输出参数：无
*******************************************************/
void ChargingProgram(void)
{
   switch(ChargerTasknum)    //选择00模式
     {
       
       case Standby_mode://待机
               
             ChargeStop();
             ALL_LEDOFF();
        
          break;
        case Sealed_mode://铅酸
               
             Sealed_mode_Charger();
             Sealed_ledon();
        
          break;
      
        case Start_stop_mode://AGM
          
             Start_stop_mode_Charger();
             Start_stop_ledon();
        
          break;
        
        case Li_battery_mode://锂电
          
             Li_battery_mode_Charger();
             Motorcycle_ledon();
        
          break;
      
        case SIX_mode://修复
           SIX_mode_Charger();
           SIX_ledon();
          break;
        
        case Repair_mode://修复
          
             Repair_mode_Charger();
        
             ALL_LEDOFF();
        
             if(gFaultType.Now!=FAULT_NULL)
             {
                ChargerTasknum=Standby_mode;
             }        
        
          break;
             
        case power_supply_mode://12V供电模式
                  
             Power_supply_mode_Charger();
        
             ALL_LEDOFF();
          
          break;
      }
}

/*******************************************************
* 函数名称：Sealed_mode_Charger
* 功能描述：铅酸电池
* 输入参数：无
* 输出参数：无
 *******************************************************/
void Sealed_mode_Charger(void)  //恒流恒压到充满
{

 static uint8_t bov_cnt=0,full_cnt=0,hc_cnt=0;
 static uint8_t ci=0,NULL_cnt=0,BERR_CNT=0;
  
  switch(gChgType.Now)
  {    
    
    case WAIT_BATT:

      ChargeStop();
      ci=0;

      if(gFaultType.Now==FAULT_NULL)   //等待无错误
      {
       gChgType.Now=PREC;
      }        
    break;
    case PREC:
      
      ChargeStop();
      ci=0;

      SetChargerVoltage1(VOLTAGE_PWM_12V);//最大电压PWM
    
      SetChargeCurrent(PWM_VAL_2A);//最大电流PWM
    
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
        
        ChargeStart();            //启动充电
    
        if(BAT_flag==1)
        {
          SetChargerVoltage1(BatV.Sealed.Battery_24V*PWM_24V);//最大电压PWM      
          /*  电流缓冲处理 */
          if(ci>0){SetChargeCurrent(PWM_VAL_2A);}
          if(ci>5){SetChargeCurrent(CC100PCT_PWM_VAL_24V);}  
          if(ci<6){ci++;}
          
        }else{
          
          SetChargerVoltage1(BatV.Sealed.Battery_12V*PWM_12V);//最大电压PWM
          
          /*  电流缓冲处理 */
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
     
         ChargeStop(); //开机不控制电流
         SetChargerVoltage1(VOLTAGE_PWM_12V);//最大电压PWM
         SetChargeCurrent(PWM_VAL_2A);//最大电流PWM
         bov_cnt=0,full_cnt=0;
    
        if(BAT_flag==1)
        {
          if(gBat_VoltageValue<2600) //重新充
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
            if(gBat_VoltageValue<1200) //重新充
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
      
      if(gBat_VoltageValue<500) //电池错误
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
      
        ChargeStart();            //启动充电
    
        if(NULL_cnt>10)
        {
        
          if(BAT_flag==1)
          {
            SetChargerVoltage1(BatV.Sealed.Battery_24V*PWM_24V);//最大电压PWM      
            /*  电流缓冲处理 */
            if(ci>0){SetChargeCurrent(PWM_VAL_2A);}
            if(ci>5){SetChargeCurrent(CC100PCT_PWM_VAL_24V);}  
            if(ci<6){ci++;}
          }else{
            SetChargerVoltage1(BatV.Sealed.Battery_12V*PWM_12V);//最大电压PWM
            
            /*  电流缓冲处理 */
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
* 函数名称：Start_stop_mode_Charger
* 功能描述：启停功能
* 输入参数：无
* 输出参数：无
*******************************************************/
void Start_stop_mode_Charger(void)
{
 static uint8_t bov_cnt,full_cnt=0,hc_cnt=0;
 static uint8_t ci=0,NULL_cnt=0,BERR_CNT=0;  // 当前值初始为0

  switch(gChgType.Now)
  {    
    case WAIT_BATT:
       ci=0;
       ChargeStop(); 
       if(gFaultType.Now==FAULT_NULL)   //等待无错误
       {
         gChgType.Now=PREC;
       }        
      break;
    case PREC:
      
      ChargeStop(); //开机不控制电流
      ci=0;
      SetChargerVoltage(VOLTAGE_PWM_12V);//最大电压PWM
      SetChargeCurrent(PWM_VAL_2A);//最大电流PWM
    
    
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
      
        ChargeStart();            //启动充电
    
        if(BAT_flag==1)
        {
          SetChargerVoltage1(BatV.AGM.Battery_24V*PWM_24V);//最大电压PWM      
          /*  电流缓冲处理 */
          if(ci>0){SetChargeCurrent(PWM_VAL_2A);}
          if(ci>5){SetChargeCurrent(CC100PCT_PWM_VAL_24V);}  
          if(ci<6){ci++;}

        }else{
          SetChargerVoltage1(BatV.AGM.Battery_12V*PWM_12V);//最大电压PWM
          
          /*  电流缓冲处理 */
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
      
         ChargeStop(); //开机不控制电流
         
         SetChargerVoltage(VOLTAGE_PWM_12V);//最大电压PWM
         SetChargeCurrent(PWM_VAL_2A);//最大电流PWM
         
         bov_cnt=0,full_cnt=0;
    
        if(BAT_flag==1)
        {
          if(gBat_VoltageValue<2600) //重新充
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
            if(gBat_VoltageValue<1300) //重新充
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
        
       if(gBat_VoltageValue<500) //电池错误
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
           
        ChargeStart();            //启动充电
    
      if(NULL_cnt>10)
        {
          if(BAT_flag==1)
          {
            SetChargerVoltage1(BatV.AGM.Battery_24V*PWM_24V);//最大电压PWM      
            /*  电流缓冲处理 */
            if(ci>0){SetChargeCurrent(PWM_VAL_2A);}
            if(ci>5){SetChargeCurrent(CC100PCT_PWM_VAL_24V);}  
            if(ci<6){ci++;}

          }else{
            SetChargerVoltage1(BatV.AGM.Battery_12V*PWM_12V);//最大电压PWM
            
            /*  电流缓冲处理 */
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
* 函数名称：锂电模式
* 功能描述：摩托车功能 摩托车最大2A
* 输入参数：无
* 输出参数：无
*******************************************************/
void Li_battery_mode_Charger(void)
{
 static uint8_t bov_cnt=0,full_cnt=0,hc_cnt=0,NULL_cnt=0,ci=0;
  

  switch(gChgType.Now)
  {    
    case WAIT_BATT:
      
      ci=0;
      ChargeStop(); 
      if(gFaultType.Now==FAULT_NULL)   //等待无错误
      {
       gChgType.Now=PREC;
      }        
    break;
    case PREC:
      ci=0;
      ChargeStop(); //开机不控制电流
      SetChargerVoltage(VOLTAGE_PWM_12V);//最大电压PWM
      SetChargeCurrent(PWM_VAL_2A);//最大电流PWM
    
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
      
    
        ChargeStart();            //启动充电
    
        if(BAT_flag==1)
        {
          SetChargerVoltage(BatV.Li_ion.Battery_24V*PWM_24V);//最大电压PWM      
          /*  电流缓冲处理 */
          if(ci>0){SetChargeCurrent(PWM_VAL_2A);}
          if(ci>5){SetChargeCurrent(CC100PCT_PWM_VAL_24V);}  
          if(ci<6){ci++;}

        }else{
          SetChargerVoltage(BatV.Li_ion.Battery_12V*PWM_12V);//最大电压PWM
          
          /*  电流缓冲处理 */
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
         ChargeStop(); //开机不控制电流
    
         SetChargerVoltage(VOLTAGE_PWM_12V);//最大电压PWM
         SetChargeCurrent(PWM_VAL_2A);//最大电流PWM
      
         bov_cnt=0,full_cnt=0,NULL_cnt=0;
    
        if(BAT_flag==1)
        {
          if(gBat_VoltageValue<2600) //重新充
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
            if(gBat_VoltageValue<1300) //重新充
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
      
        ChargeStart();            //启动充电
        if(NULL_cnt>10){
        if(BAT_flag==1)
        {
          SetChargerVoltage(BatV.Li_ion.Battery_24V*PWM_24V);//最大电压PWM      
          /*  电流缓冲处理 */
          if(ci>0){SetChargeCurrent(PWM_VAL_2A);}
          if(ci>5){SetChargeCurrent(CC100PCT_PWM_VAL_24V);}  
          if(ci<6){ci++;}

        }else{
          SetChargerVoltage(BatV.Li_ion.Battery_12V*PWM_12V);//最大电压PWM
          
          /*  电流缓冲处理 */
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
* 函数名称：Repair_mode_Charger
* 功能描述：修复功能 脉冲 电流最大5A
* 输入参数：无
* 输出参数：无
*******************************************************/
void Repair_mode_Charger(void)
{
//  int pwm24Value=0,pwm12Value=0;
  static uint8_t bov_cnt=0,full_cnt=0,i=0;
  
  switch(gChgType.Now)
  {      
    case WAIT_BATT:
      
       ChargeStop(); 
       if(gFaultType.Now==FAULT_NULL)   //等待无错误
       {
         gChgType.Now=PREC;
       }        
      break;  
    case PREC:
      ChargeStop(); //开机不控制电流
      SetChargerVoltage (VOLTAGE_PWM_12V);//最大电压PWM
      SetChargeCurrent(PWM_VAL_2A);//最大电流PWM
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
      
        ChargeStart();            //启动充电
          
        if(BAT_flag==1)
        {
          SetChargerVoltage(VOLTAGE_PWM_24V);//最大电压PWM
           
         //pwm24Value = PWMControl_Update(&pwmLow);//脉冲
          
          if(i<10){
            SetChargeCurrent(CC100PCT_PWM_VAL_24V);//最大电流PWM
            i++;
          }else if(i<20)
          {
            i++;
            SetChargeCurrent(0);
          }else{i=0;}
          
         }else{
           
          SetChargerVoltage(VOLTAGE_PWM_12V);//最大电压PWM
           
//          pwm12Value = PWMControl_Update(&pwmLow);
//           
//          SetChargeCurrent(pwm12Value);//最大电流PWM
           
         //一秒切换一次电流
         if(i<10){
            SetChargeCurrent(CC100PCT_PWM_VAL_24V);//最大电流PWM
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
      
      ChargeStop(); //开机不控制电流
      SetChargerVoltage(VOLTAGE_PWM_12V);//最大电压PWM
      
      SetChargeCurrent(PWM_VAL_2A);
      
      bov_cnt=0,full_cnt=0;
    
      break;
      
    case HC_NULL:
      
      break;       
   }
  
}


/*******************************************************
* 函数名称：12V供电模式
* 功能描述：强制12V输出，10-11A电流供电
* 输入参数：无
* 输出参数：无
*******************************************************/
void Power_supply_mode_Charger(void)
{
  static uint8_t i=0;
  switch(gChgType.Now)
  {      
    case WAIT_BATT:
      
       ChargeStop(); 
       if(gFaultType.Now==FAULT_NULL)   //等待无错误
       {
         gChgType.Now=PREC;
       }        
      break;  
       
    case PREC:
      ChargeStop(); //开机不控制电流
      SetChargerVoltage (PWM_12V*13.6);//最大电压PWM
      SetChargeCurrent(PWM_VAL_2A);//最大电流PWM
      
      gChgType.Now=CCC1;
      
      break;
      
    case CCC1:
      
        ChargeStart();            //启动充电
          
        SetChargerVoltage(PWM_12V*13.6);//最大电压PWM
            
        /*  电流缓冲处理 */
        if(i>0){SetChargeCurrent(PWM_VAL_2A);}
        if(i>5){SetChargeCurrent(CC100PCT_PWM_VAL_24V);}  
        if(i>10){SetChargeCurrent(Power_PWM_VAL);}  
        
        if(i<12){i++;}
        
      break;
          
    case CHG_FULL:
      
      ChargeStop(); //开机不控制电流
      SetChargerVoltage(PWM_12V*13.6);//最大电压PWM   
      SetChargeCurrent(PWM_VAL_2A);
          
      break;
      
    case HC_NULL:
      
      break;       
   }
  
}


/*******************************************************
* 函数名称：SIX_mode_Charger
* 功能描述：6V电池功能
* 输入参数：无
* 输出参数：无
*******************************************************/
void SIX_mode_Charger(void)
{
 static uint8_t full_cnt=0;

  switch(gChgType.Now)
  {    
    case WAIT_BATT:

      ChargeStop(); 
      if(gFaultType.Now==FAULT_NULL)   //等待无错误
      {
       gChgType.Now=PREC;
      }        
    break;
    case PREC:
      
      ChargeStart(); //开机不控制电流
      SetChargerVoltage(VOLTAGE_PWM_6V);//最大电压PWM
      SetChargeCurrent(PWM_VAL_2A);//最大电流PWM
    
      gChgType.Now=CCC1;
      
      break;
      
    case CCC1:
      
        ChargeStart();            //启动充电
    
        SetChargerVoltage(VOLTAGE_PWM_6V);//最大电压PWM
        SetChargeCurrent(CC100PCT_PWM_VAL_12V);//最大电流PWM

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
         ChargeStop(); //开机不控制电流
         full_cnt=0;
         SetChargerVoltage(VOLTAGE_PWM_6V);//最大电压PWM
         SetChargeCurrent(PWM_VAL_2A);//最大电流PWM
      
      break;


    case HC_NULL:
       
      break;      
   }
  
}


// 初始化 PWM 控制器
void PWMControl_Init(PWMControl *pwm, int targetValue, int step) {
    pwm->currentValue = 0;
    pwm->targetValue = targetValue;
    pwm->step = step;
    pwm->state = RISING;
}

// 每次调用调整PWM值并返回新的PWM值
int PWMControl_Update(PWMControl *pwm) {
    if (pwm->state == RISING) {
        // 上升阶段
        pwm->currentValue += pwm->step;
        if (pwm->currentValue >= pwm->targetValue) {
            pwm->currentValue = pwm->targetValue;
            pwm->state = FALLING;  // 达到目标值后切换到下降阶段
        }
    } else if (pwm->state == FALLING) {
        // 下降阶段
        pwm->currentValue -= pwm->step;
        if (pwm->currentValue <= 0) {
            pwm->currentValue = 0;
            pwm->state = RISING;  // 回到 0 后切换回上升阶段（循环）
        }
    }
    return pwm->currentValue;
}


// 每次增加的步进值
#define STEP 50

// 逐步增加到目标值的函数
uint16_t rampUpToTarget(uint16_t current, uint16_t target) {
    if (current < target) {
        current += STEP;
        if (current > target) {
            current = target;  // 防止超过目标值
        }
    }
    return current;
}
