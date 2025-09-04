#include "User_charger.h"

/************************ ȫ�ֱ������� ************************/
sCHGType gChgType = {WAIT_BATT};    /* ���״̬ */


/*******************************************************
* �������ƣ�ChargeStart
* �����������������
* �����������
* �����������
*******************************************************/
void ChargeStart(void)
{
  ON_;
}
/*******************************************************-- 
* �������ƣ�ChargeStop
* ����������ֹͣ���
* �����������
* �����������
*******************************************************/
void ChargeStop(void)
{
  OFF_;
}

/*******************************************************
* �������ƣ�ChargingProgram
* ����������������
* �����������
* �����������
*******************************************************/
void ChargingProgram(void)
{

      switch(ChargerTasknum)    //ѡ��ģʽ
     {
        case standby:
          ChargeStop();
          SetChargeCurrent(PWM_VAL_2A);
          break;
        
        case Sealed://Ǧ��
          Sealed_Charger();
          break;
      
        case AGM://��ͣ
          AGM_Charger();
         
         break;
        
        case lithium_battery://﮵�
         lithium_battery_Charger();

          break;
      
        
        case SIXV://�޸�
          SIXV_Charger();
          break;
      }
}

/*******************************************************
* �������ƣ�Sealed_Charger
* ����������Ǧ����
* �����������
* �����������
 *******************************************************/
void Sealed_Charger(void)  //������ѹ������
{
  static uint8_t i=0,HC_CNT=0;
  static uint16_t Stopvoltage1=0;
  
  switch(gChgType.Now)  //��׶γ������
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
   
    if(gFaultType.Now==FAULT_NULL)   //�ȴ��޴���
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
* �������ƣ�AGM_Charger
* ������������ͣ����
* �����������
* �����������
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
    if(gFaultType.Now==FAULT_NULL)   //�ȴ��޴���
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
* �������ƣ�lithium_battery_Charger
* ����������﮵�
* �����������
* �����������
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
    if(gFaultType.Now==FAULT_NULL)   //�ȴ��޴���
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
* �������ƣ�SIXV_Charger
* ����������6V
* �����������
* �����������
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
    if(gFaultType.Now==FAULT_NULL)   //�ȴ��޴���
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
  * @brief  Ǧ�����¶Ȳ�������
  * @param  temp: ��ǰ�¶�(��), ��Χ����0~50��
  * @retval ������ĵ�ѹ��ֵ����λ��ʮ������1440=14.4V��
  */
uint16_t get_compensated_voltage(uint8_t temp) {
    // ��׼��������
    const int8_t BASE_TEMP = 25;          // ��׼�¶�25��
    const uint16_t BASE_VOLTAGE = 1440;    // ��׼��ѹ14.4V����ӦǦ���أ�
    const int8_t COMP_COEFF = -18;        // ����ϵ��-18mV/�棨-1.8/�� in 0.1V��λ��

    // �����¶Ȳ�
    int8_t delta_temp = temp - BASE_TEMP;

    // ���㲹��ֵ����λ��0.1V��
    int16_t compensation = (delta_temp * COMP_COEFF) / 10; // ��������

    // Ӧ�ò���
    uint16_t volt_threshold = BASE_VOLTAGE + compensation;

    // ��ѹ��ȫ�޷���14.1V ~ 15.0V��
    volt_threshold = (volt_threshold > 1500) ? 1500 : volt_threshold;
    volt_threshold = (volt_threshold < 1410) ? 1410 : volt_threshold;

    return volt_threshold;
}


/**
  * @brief  ﮵���¶Ȳ���������LiFePO4��
  * @param  temp: ��ǰ�¶�(��), ���鷶Χ-20~60��
  * @retval ������ĳ���ѹ��ֵ����λ��ʮ������1460=14.6V��
  */
uint16_t lithium_temp_compensation(int8_t temp) {
    // ��׼������LiFePO4 4������飩
    const int8_t BASE_TEMP = 25;          // ��׼�¶�25��
    const uint16_t BASE_VOLTAGE = 1460;   // ��׼��ѹ14.6V��3.65V/cell��
    const int8_t COMP_COEFF = -4;         // ����ϵ��-4mV/�棨���飩

    // �����²��
    int8_t delta_temp = temp - BASE_TEMP;
    int16_t compensation = delta_temp * COMP_COEFF;

    // Ӧ�ò���
    uint16_t volt_threshold = BASE_VOLTAGE + compensation;

    // ��ѹ��ȫ�޷���14.0V ~ 14.8V��
    volt_threshold = (volt_threshold > 1480) ? 1480 : volt_threshold;
    volt_threshold = (volt_threshold < 1400) ? 1400 : volt_threshold;

    return volt_threshold;
}
