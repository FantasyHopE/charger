#include "User_charger.h"

/************************ 全局变量定义 ************************/
sCHGType gChgType = {WAIT_BATT};    /* 充电状态 */


/*******************************************************
* 函数名称：ChargeStart
* 功能描述：启动充电
* 输入参数：无
* 输出参数：无
*******************************************************/
void ChargeStart(void)
{
  ON_;
}
/*******************************************************-- 
* 函数名称：ChargeStop
* 功能描述：停止充电
* 输入参数：无
* 输出参数：无
*******************************************************/
void ChargeStop(void)
{
  OFF_;
}

/*******************************************************
* 函数名称：ChargingProgram
* 功能描述：充电进程
* 输入参数：无
* 输出参数：无
*******************************************************/
void ChargingProgram(void)
{

      switch(ChargerTasknum)    //选择模式
     {
        case standby:
          ChargeStop();
          SetChargeCurrent(PWM_VAL_2A);
          break;
        
        case Sealed://铅酸
          Sealed_Charger();
          break;
      
        case AGM://启停
          AGM_Charger();
         
         break;
        
        case lithium_battery://锂电
         lithium_battery_Charger();

          break;
      
        
        case SIXV://修复
          SIXV_Charger();
          break;
      }
}

/*******************************************************
* 函数名称：Sealed_Charger
* 功能描述：铅酸充电
* 输入参数：无
* 输出参数：无
 *******************************************************/
void Sealed_Charger(void)  //恒流恒压到充满
{
  static uint8_t i=0,HC_CNT=0;
  static uint16_t Stopvoltage1=0;
  
  switch(gChgType.Now)  //多阶段充电流程
  {    
    case WAIT_BACK:

    ChargeStop(); 
    i=0;
    HC_CNT=0;
    SetChargeCurrent(PWM_VAL_2A);
    break;
    
   case WAIT_BATT:
     
    Stopvoltage1=get_compensated_voltage(gTemperature);
    SetChargeCurrent(PWM_VAL_2A);
    ChargeStop();
   
    if(gFaultType.Now==FAULT_NULL)   //等待无错误
    {
     gChgType.Now=CCC1;
     i=0;
     HC_CNT=0;
    } 
    
    break;
    
    case CCC1:
      
     ChargeStart();
     SetChargeCurrent(PWM_VAL_5A);
    
     if(gBat_VoltageValue>Stopvoltage1)
     {
       if(i++>30){
        gChgType.Now=CHG_FULL;
       }
     }
     
    break;

    case CHG_FULL:
    ChargeStop();
    if(gBat_VoltageValue<1250)
    {
       if(HC_CNT++>200)
       {
        gChgType.Now=CCC1;
        HC_CNT=0;
        i=0;
       }
    }      
    break; 

  }

}
/*******************************************************
* 函数名称：AGM_Charger
* 功能描述：启停功能
* 输入参数：无
* 输出参数：无
*******************************************************/
void AGM_Charger(void)
{
  static uint8_t i=0,HC_CNT=0;
  static uint16_t Stopvoltage2=0;
  switch(gChgType.Now)
  {    
    case WAIT_BACK:

    SetChargeCurrent(PWM_VAL_2A);
    ChargeStop();
    i=0;
    HC_CNT=0;
    break;
    
   case WAIT_BATT:

    Stopvoltage2=get_compensated_voltage(gTemperature);
    SetChargeCurrent(PWM_VAL_2A);
    ChargeStop();
    if(gFaultType.Now==FAULT_NULL)   //等待无错误
    {
     gChgType.Now=CCC1;
     i=0;
     HC_CNT=0;
    } 
    
    break;
    
    case CCC1:
      
     ChargeStart();
     SetChargeCurrent(PWM_VAL_5A);
    
     if(gBat_VoltageValue>Stopvoltage2)
     {
       if(i++>30){
        gChgType.Now=CHG_FULL;
       }
     }
     
    break;

    case CHG_FULL:
    ChargeStop();
    if(gBat_VoltageValue<1250)
    {
       if(HC_CNT++>30)
       {
        gChgType.Now=CCC1;
        HC_CNT=0;
        i=0;
       }
    }      
    break; 

  }
}
/*******************************************************
* 函数名称：lithium_battery_Charger
* 功能描述：锂电
* 输入参数：无
* 输出参数：无
*******************************************************/
void lithium_battery_Charger(void)
{
  static uint8_t i=0;
  static uint16_t Stopvoltage3=0;
  
  switch(gChgType.Now)
  {    
    case WAIT_BACK:

    SetChargeCurrent(PWM_VAL_2A);
    ChargeStop();
    i=0;
    break;
    
   case WAIT_BATT:
    Stopvoltage3=lithium_temp_compensation(gTemperature);
    SetChargeCurrent(PWM_VAL_2A);
    ChargeStop();
    if(gFaultType.Now==FAULT_NULL)   //等待无错误
    {
     gChgType.Now=CCC1;
     i=0;
    }     
    break;
    
    case CCC1:
      
     ChargeStart();
     SetChargeCurrent(PWM_VAL_5A);
    
     if(gBat_VoltageValue>Stopvoltage3)
     {
       if(i++>30){
        gChgType.Now=CHG_FULL;
       }
     }    
    break;

    case CHG_FULL:
    ChargeStop();   
    break; 

  }
  
}

/*******************************************************
* 函数名称：SIXV_Charger
* 功能描述：6V
* 输入参数：无
* 输出参数：无
*******************************************************/
void SIXV_Charger(void)
{

  static uint8_t i=0,HC_CNT=0;

  switch(gChgType.Now)
  {    
    case WAIT_BACK:

    SetChargeCurrent(PWM_VAL_2A);
    ChargeStop(); 
    i=0;
    HC_CNT=0;
    break;
    
   case WAIT_BATT:
    SetChargeCurrent(PWM_VAL_2A);
    ChargeStop();
    if(gFaultType.Now==FAULT_NULL)   //等待无错误
    {
     gChgType.Now=CCC1;
     i=0;
     HC_CNT=0;
    } 
    
    break;
    
    case CCC1:
      
     ChargeStart();
     SetChargeCurrent(PWM_VAL_5A);
    
     if(gBat_VoltageValue>700)
     {
       if(i++>30){
        gChgType.Now=CHG_FULL;
       }
     }
     
    break;

    case CHG_FULL:
    ChargeStop();
    if(gBat_VoltageValue<500)
    {
       if(HC_CNT++>200)
       {
        gChgType.Now=CCC1;
        HC_CNT=0;
        i=0;
       }
    }      
    break; 

  }
}

/**
  * @brief  铅酸电池温度补偿函数
  * @param  temp: 当前温度(℃), 范围建议0~50℃
  * @retval 补偿后的电压阈值（单位：十毫伏，1440=14.4V）
  */
uint16_t get_compensated_voltage(uint8_t temp) {
    // 基准参数定义
    const int8_t BASE_TEMP = 25;          // 基准温度25℃
    const uint16_t BASE_VOLTAGE = 1440;    // 基准电压14.4V（对应铅酸电池）
    const int8_t COMP_COEFF = -18;        // 补偿系数-18mV/℃（-1.8/℃ in 0.1V单位）

    // 计算温度差
    int8_t delta_temp = temp - BASE_TEMP;

    // 计算补偿值（单位：0.1V）
    int16_t compensation = (delta_temp * COMP_COEFF) / 10; // 整数运算

    // 应用补偿
    uint16_t volt_threshold = BASE_VOLTAGE + compensation;

    // 电压安全限幅（14.1V ~ 15.0V）
    volt_threshold = (volt_threshold > 1500) ? 1500 : volt_threshold;
    volt_threshold = (volt_threshold < 1410) ? 1410 : volt_threshold;

    return volt_threshold;
}


/**
  * @brief  锂电池温度补偿函数（LiFePO4）
  * @param  temp: 当前温度(℃), 建议范围-20~60℃
  * @retval 补偿后的充电电压阈值（单位：十毫伏，1460=14.6V）
  */
uint16_t lithium_temp_compensation(int8_t temp) {
    // 基准参数（LiFePO4 4串电池组）
    const int8_t BASE_TEMP = 25;          // 基准温度25℃
    const uint16_t BASE_VOLTAGE = 1460;   // 基准电压14.6V（3.65V/cell）
    const int8_t COMP_COEFF = -4;         // 补偿系数-4mV/℃（整组）

    // 计算温差补偿
    int8_t delta_temp = temp - BASE_TEMP;
    int16_t compensation = delta_temp * COMP_COEFF;

    // 应用补偿
    uint16_t volt_threshold = BASE_VOLTAGE + compensation;

    // 电压安全限幅（14.0V ~ 14.8V）
    volt_threshold = (volt_threshold > 1480) ? 1480 : volt_threshold;
    volt_threshold = (volt_threshold < 1400) ? 1400 : volt_threshold;

    return volt_threshold;
}
