#include "User_lcd.h"

uint8_t m_au8Value[TM1621D_BYTES]={0};
uint8_t B_V_FLAG=0,B_V_cnt=0;
// ��ʼ�� TM1621D
void TM1621_Init(void)
{
      //��GPIOʱ��
    __SYSCTRL_GPIOA_CLK_ENABLE();
    __SYSCTRL_GPIOB_CLK_ENABLE();
    // ��ʼ�� GPIO ������Ϊ���
    GPIO_InitTypeDef LCD_InitStruct = {0};
   
    LCD_InitStruct.Pins = TM1621_CS_PIN;
    LCD_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    LCD_InitStruct.IT   = GPIO_IT_NONE;
   
    GPIO_Init(CW_GPIOA, &LCD_InitStruct);
    
    LCD_InitStruct.Pins = TM1621_WR_PIN | TM1621_DATA_PIN;
    LCD_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    LCD_InitStruct.IT   = GPIO_IT_NONE;
   
    GPIO_Init(CW_GPIOB, &LCD_InitStruct);
    

    TM1621_CS_HIGH;  // ���� CS����ʹ��
    TM1621_WR_HIGH;  // WR ����
    TM1621_DATA_HIGH;  // ����������
    
}




/*****************************************************************************
[��������]SendHighBits
[��������]д���λ   һλһλ�ķ���
[��    ��]
[ȫ�ֱ���]��
[�� �� ֵ]��
[��    ע]
*****************************************************************************/
void SendHighBits(uint8_t u8Data,uint8_t u8Bit)
{
	uint8_t u8Index = 0;

	for(u8Index = 0;u8Index < u8Bit;u8Index++)
	{
		if((u8Data & 0x80) == 0){
			TM1621_DATA_LOW;
		}
		else{
			TM1621_DATA_HIGH;
		}
						
		TM1621_WR_LOW;

		TM1621_WR_HIGH;
		
		u8Data <<= 1;
	}
}
/*****************************************************************************
[��������]SendLowBits
[��������]д���λ
[��    ��]
[ȫ�ֱ���]��
[�� �� ֵ]��
[��    ע] ����λ
*****************************************************************************/
void SendLowBits(uint8_t u8Data,uint8_t u8Bit)
{
	uint8_t u8Index = 0;

	for(u8Index = 0;u8Index < u8Bit;u8Index++)
	{
		if((u8Data & 0x01) == 0)
		{
			TM1621_DATA_LOW;
		}
		else{
			TM1621_DATA_HIGH;
		}
		TM1621_WR_LOW;

		TM1621_WR_HIGH;        
		u8Data >>= 1;
	}
}
/*****************************************************************************
[��������]SendData
[��������]дģʽ д���ݵ�RAM��
[��    ��]
[ȫ�ֱ���]��
[�� �� ֵ]��
[��    ע]
*****************************************************************************/
void SendData(uint8_t u8Addr,uint8_t u8Data)
{
	TM1621_CS_LOW; 
	SendHighBits(WRITE<<5,3);		//����һ��101
	SendHighBits(u8Addr<<2,6);		//����6λ�ĵ�ַ
	SendLowBits(u8Data,4);			//����4λ������
	TM1621_CS_HIGH;
}

/*****************************************************************************
[��������]vSetSeg
[��������]
[ȫ�ֱ���]��
[�� �� ֵ]
[��    ע] u8Addr seg��ַ   u8Bit com��ַ  bOn ��1��0 
*****************************************************************************/
void SetSeg( uint8_t u8Addr,uint8_t u8Bit,uint8_t bOn )
{
	if(u8Addr < TM1621D_BYTES)
	{
		uint8_t u8Write = 0;
		
		u8Write = m_au8Value[u8Addr];//��ȡ��Ӧ��seg��״̬
		
		if(bOn)		//����д1��0
		{
			u8Write |= (0x01 << u8Bit);			//����λ��1
		}
		else
		{
			u8Write &= ~(0x01 << u8Bit);		//����λ��0
		}
		u8Write &= 0x0F;
		m_au8Value[u8Addr] = u8Write;

		SendData(u8Addr,u8Write);		//�����Ӧ��seg��״̬
	}
}
/*****************************************************************************
[��������]SetSegs �Ľ�
[��������]
[ȫ�ֱ���]��
[�� �� ֵ]
[��    ע] u8Addr seg��ַ   u8Bit com��ַ  bOn ��1��0 
*****************************************************************************/
void SetSegs(uint8_t u8Addr, uint8_t u8BitMask, uint8_t bOn)
{
    if(u8Addr < TM1621D_BYTES)
    {
        uint8_t u8Write = m_au8Value[u8Addr];  // ��ȡ��ǰ SEG ���ŵ�״̬
        
        if(bOn)  // ���Ҫ����ָ��λ
        {
            u8Write |= u8BitMask;  // �� u8BitMask �����õ�λȫ���� 1
        }
        else  // ���ҪϨ��ָ��λ
        {
            u8Write &= ~u8BitMask;  // �� u8BitMask �����õ�λȫ���� 0
        }
        
        u8Write &= 0x0F;  // ֻ������ 4 λ��ȷ����Ӱ������λ
        m_au8Value[u8Addr] = u8Write;  // ���� m_au8Value �е�״̬
        
        SendData(u8Addr, u8Write);  // ���µ�״̬д�� TM1621D
    }
}

/*****************************************************************************
[��������]vClearScreen
[��������]
[ȫ�ֱ���]��
[�� �� ֵ]
[��    ע]
*****************************************************************************/
void ClearScreen( void )
{
	memset(&m_au8Value,0,TM1621D_BYTES);    
	for(uint8_t u8Addr = 0;u8Addr < TM1621D_BYTES;u8Addr++)
	{
		SendData(u8Addr,0);
	}
}


/*****************************************************************************
[��������]vFillScreen
[��������]
[ȫ�ֱ���]��
[�� �� ֵ]
[��    ע]
*****************************************************************************/
void FillScreen( void )
{
	memset(&m_au8Value,0x0F,TM1621D_BYTES);
	for(uint8_t u8Addr = 0;u8Addr < TM1621D_BYTES;u8Addr++)
	{
		SendData(u8Addr,0x0F);
	}
}

/*****************************************************************************
[��������]vSendCommand   
[��������]����ģʽ
[��    ��]
[ȫ�ֱ���]��
[�� �� ֵ]��
[��    ע]
*****************************************************************************/
void SendCommand(uint8_t u8Command)
{
	TM1621_CS_LOW;
	SendHighBits(COMMAND << 5,3);		//����һ��100
	SendHighBits(u8Command,9);				//���� 9λ������
	TM1621_CS_HIGH;
}

uint8_t LcdInit( void )
{

	TM1621_CS_LOW;	
	TM1621_CS_HIGH;
	TM1621_CS_LOW;		//�ߵ�ƽ�����ʼ��HT1621���нӿ�	
	// ��ʼ��Һ������

	SendCommand(TM1621D_SYSEN);		//��ϵͳ����
  SendCommand(TM1621D_BIAS);			//ƫѹ�Ͷ˿�ѡ��
  
	SendCommand(TM1621D_LCDOFF);		//�ر�LCD��ʾ
	SendCommand(TM1621D_LCDON);		//��LCD��ʾ
	//���һ�¶�����	
	FillScreen();  //ȫ��
  
  FB_ON;
 
	SysTickDelay(500);	
  FB_OFF;
  SysTickDelay(500);
  ALL_LEDON();
	ClearScreen();//ȫ�� 
  
 
	return 0;
}

/*****************************************************************************
[��������]Screen_refresh   
[��������]Lcd��Ļˢ��  �¶ȣ���ѹ������������ʾ
[��    ��]
[ȫ�ֱ���]��
[�� �� ֵ]��
[��    ע]
*****************************************************************************/
void Screen_refresh( void )
{
 
  static uint8_t FUL_flag=0,REP_flag=0,Standby_flag=0; //  ������־λ
  static uint8_t oneflag[10]={0};
  static uint8_t i=0;
  if(gFaultType.Now!=FAULT_NULL)
  {
   Standby_flag=0;Standby_flag=0;FUL_flag=0;

  }
   switch(gFaultType.Now)
    {
      case FAULT_NULL:          /* �޹���                   */
        

          memset(oneflag,0,10);
          
          FB_OFF;
          if(ChargerTasknum==Standby_mode){
            
            if(Standby_flag!=1){
               
                Standby_flag=1;
                ClearScreen();
             }
            
             SetSegs(15,0X0A,1); //0
             SetSegs(14,0X0F,1);
             
             SetSegs(13,0X0E,1); //F
             SendData(12,0X08);        
               
             SetSegs(11,0X0E,1); //F
             SendData(10,0X08); 
          
          }else if(ChargerTasknum==power_supply_mode){
            
            if(Standby_flag!=1){
               
                Standby_flag=1;
                ClearScreen();
             }
            display(gBat_VoltageValue);
            SendData(18, 0x04);//V
          
          }else if(ChargerTasknum==Repair_mode) 
           {
             if(REP_flag!=1){
               
                REP_flag=1;
                ClearScreen();
             }
             
             FUL_flag=0;
             SetSegs(15,0X0E,1); //P
             SendData(14,0X0C);
             
             SetSegs(13,0X0A,1); //U
             SetSegs(12,0X07,1); 

             
             SetSegs(11,0X0A,1); //L
             SetSegs(10,0X01,1); 
             
             if(i==0)
             {
               SetSegs(17,0x02,1);
               i=1;
             }else{               
              SetSegs(17,0x02,0);
              i=0;               
             }
             
             SetSegs(9,0x01,1);  //�����߿�
             SetSegs(16,0x0F,1);//�ٷְٵ���
             SetSegs(9,0x03,1);
             
             
           }else{
             
                if(FUL_flag==0){ 
                  ClearScreen();
                  FUL_flag=1;
                }
                
                  REP_flag=0;
                  Standby_flag=0;

                  B_V_cnt++;
                  if(B_V_cnt>15)B_V_FLAG=1;
//                if(B_V_cnt>20)B_V_FLAG=2;
                  if(B_V_cnt>30)B_V_FLAG=0,B_V_cnt=0;
                  
                   switch(B_V_FLAG) //������ѹ��ʾ
                   {
                     case 0:
                       display(gBat_VoltageValue);
                       SendData(18, 0x04);//V
                       break;
                     case 1:

                       display(gCurrentValue);
                       SendData(18, 0x01);//A
                       break;
                     case 2:

                       display(gTemperature*100);
                       SendData(18, 0x08);//��
                     
                       break;
                   }

                   //������ʾ
                   BatteryStatus();
                   
                   
                  if(jwen_v==1){   //������ʾ
                    SetSegs(13,0x01,1);
                    SetSegs(15,0x01,0); 
                  }else if(jwen_v==2){
                    SetSegs(15,0x01,1);
                    SetSegs(13,0x01,0); 
                  }else{
                    SetSegs(15,0x01,0);
                    SetSegs(13,0x01,0); 
                  
                  }
           
           }
          
          
        break;
     case INF:                 /* �����쳣                 */
       break;
     case HOTP:                /* Ӳ������                 */
      if(oneflag[0]!=1){

       ClearScreen();
       FUL_flag=0;
     
       oneflag[0]=1;}
      
       SetSegs(15,0X0A,1); //0
       SetSegs(14,0X0F,1);
       
       SetSegs(13,0X0A,1); //T
       SendData(12,0X08);        
       
       SetSegs(11,0X0E,1); //P
       SendData(10,0X0C);       
       

      
       if(i){
         i=0;
         FB_OFF;
       }else{
        i=1;
          FB_ON;
       }
       break;
       
     case ROTP:                /* �������                 */
       
       break;
     case COVP:                /* ���������ѹ           */
       if(oneflag[1]!=1){

       ClearScreen();
       FUL_flag=0;
       oneflag[1]=1;}
       
//       SetSegs(15,0X0A,1); //0
//       SetSegs(14,0X0F,1);
//       
//       SetSegs(13,0X0A,1); //u
//       SendData(12,0X07);        
//       
//       SetSegs(11,0X0E,1); //P
//       SendData(10,0X0C);       
       
       
       if(i){
       SetSegs(17,0x01,0);//flt
         i=0;
         FB_OFF;
       }else{
       SetSegs(17,0x01,1);//flt
        i=1;
        FB_ON;
       }
       
       break;
     case COCP:                /* �����������           */
       if(oneflag[2]!=1){
         
       ClearScreen();
       FUL_flag=0;
         
       oneflag[2]=1;}
       
       SetSegs(15,0X0A,1); //0
       SetSegs(14,0X0F,1);
       
       SetSegs(13,0X0A,1); //C
       SendData(12,0X09);        
       
       SetSegs(11,0X0E,1); //P
       SendData(10,0X0C);       
       

       
      if(i){
         i=0;
         FB_OFF;
       }else{
        i=1;
         FB_ON;
       }
       break;
     case BOVP:                /* ��ع�ѹ                 */
       FUL_flag=0;
       break;
     
     case BERR:                /* ����쳣                 */
       if(oneflag[3]!=1){
       FUL_flag=0;
       ClearScreen();
           oneflag[3]=1;}
       
//       SetSegs(15,0X0E,1); //b
//       SetSegs(14,0X03,1);
//       
//       SetSegs(13,0X0E,1); //E
//       SendData(12,0X09);        
//       
//       SetSegs(11,0X0E,1); //E
//       SendData(10,0X09);
           
       if(i){
       SetSegs(17,0x01,0);//flt
         i=0;
         FB_OFF;
       }else{
       SetSegs(17,0x01,1);//flt
        i=1;
        FB_ON;
       }

       break;
     case LERR:                /* ����쳣                 */
       
       break;
     case FULL_CHG:            /* ����                     */
     if(oneflag[4]!=1){
       FUL_flag=0;
       ClearScreen();
            oneflag[4]=1;}
       SetSegs(15,0x0E,1); //F
       SendData(14,0X08);
        
       SetSegs(13,0X0A,1); //U
       SetSegs(12,0X07,1); 

       
       SetSegs(11,0X0A,1); //L
       SetSegs(10,0X01,1); 
       
       
       SetSegs(16,0x0F,1);//�ٷְٵ���
       SetSegs(9,0x03,1);

       break;
     case FJBH:                /* ���ӱ���                 */
       
       if(oneflag[5]!=1){
      
       ClearScreen();
       oneflag[6]=0;
       oneflag[5]=1;}
      
       if(i){
       SetSegs(9,0x08,0);//flt
         i=0;
         FB_OFF;
       }else{
       SetSegs(9,0x08,1);//flt
        i=1;
        FB_ON;
       }
     
       break;
       
     case STANDBY:           /* δ����           */
     if(oneflag[6]!=1){
       FUL_flag=0;
       oneflag[5]=0;
       ClearScreen();
     
       oneflag[6]=1;}
      
       SetSegs(15,0X0A,1); //0
       SetSegs(14,0X0F,1);
       
       SetSegs(13,0X0E,1); //F
       SendData(12,0X08);        
         
       SetSegs(11,0X0E,1); //F
       SendData(10,0X08);       
       

     
       if(i){
       SetSegs(9,0x01,0);  //�����߿�
         i=0;
       }else{
       SetSegs(9,0x01,1);  //�����߿�
        i=1;
       }
       
       break;
     
    
    }
      
}



//����λ��С��λ  ����λ�ٷ�Ϊʮλ���͸�λ��    ��ʾ��ʮλ������λ����С��λ���   24V  2400

/*****************************************************************************
[��������]Integer_display  
[��������] ʮλ����ʾ 
[��    ��] num С��100������
[ȫ�ֱ���]��
[�� �� ֵ]��
[��    ע]
*****************************************************************************/
uint8_t tens_display(uint8_t num)
{
    switch(num)
    {
      case 0:
        
        NO1_8_OFF();
//      SetSegs(14,0x0F,1);  
//      SetSegs(15,0x0A,1);
        break;
      
      case 1:
        NO1_8_OFF();
        SetSegs(14,0x06,1);
        SetSegs(15,0x0E,0);
        break;
      case 2:
        NO1_8_OFF();
        SetSegs(14,0x0D,1);  
        SetSegs(15,0x06,1);
        break;
      case 3:
        NO1_8_OFF();
        SetSegs(14,0x0F,1);  
        SetSegs(15,0x04,1);
        break;
      case 4:
        NO1_8_OFF();
        SetSegs(14,0x06,1);  
        SetSegs(15,0x0C,1);
        break;
      case 5:
        NO1_8_OFF();
        SetSegs(14,0x0B,1);  
        SetSegs(15,0x0C,1);
        break;
      case 6:
        NO1_8_OFF();
        SetSegs(14,0x0B,1);  
        SetSegs(15,0x0E,1);
        break;
      case 7:
        NO1_8_OFF();
        SetSegs(14,0x0E,1);
        SetSegs(15,0x0E,0);
        break;
      case 8:
        NO1_8_OFF();
        SetSegs(14,0x0F,1);  
        SetSegs(15,0x0E,1);
        break;
      case 9:
        NO1_8_OFF();
        SetSegs(14,0x0F,1);  
        SetSegs(15,0x0C,1);
        break;
    }
  return 1;
}

/*****************************************************************************
[��������]decimals_display  
[��������] ��λ����ʾ 
[��    ��] num
[ȫ�ֱ���]��
[�� �� ֵ]��
[��    ע]
*****************************************************************************/
uint8_t units_display(uint8_t num1)
{
 
     switch(num1)
      {
        case 0:
          
          NO2_8_OFF();
          SetSegs(12,0x0F,1);  
          SetSegs(13,0x0A,1);
          break;
        
        case 1:
          NO2_8_OFF();
          SetSegs(12,0x06,1);
          SetSegs(13,0x0E,0);
          break;
        case 2:
          NO2_8_OFF();
          SetSegs(12,0x0D,1);  
          SetSegs(13,0x06,1);
          break;
        case 3:
          NO2_8_OFF();
          SetSegs(12,0x0F,1);  
          SetSegs(13,0x04,1);
          break;
        case 4:
          NO2_8_OFF();
          SetSegs(12,0x06,1);  
          SetSegs(13,0x0C,1);
          break;
        case 5:
          NO2_8_OFF();
          SetSegs(12,0x0B,1);  
          SetSegs(13,0x0C,1);
          break;
        case 6:
          NO2_8_OFF();
          SetSegs(12,0x0B,1);  
          SetSegs(13,0x0E,1);
          break;
        case 7:
          NO2_8_OFF();
          SetSegs(12,0x0E,1);
          SetSegs(13,0x0E,0);
          break;
        case 8:
          NO2_8_OFF();
          SetSegs(12,0x0F,1);  
          SetSegs(13,0x0E,1);
          break;
        case 9:
          NO2_8_OFF();
          SetSegs(12,0x0F,1);  
          SetSegs(13,0x0C,1);
          break;
      }
 
   return 1;
}


/*****************************************************************************
[��������]decimals_display  
[��������] ��λ����ʾ 
[��    ��] num
[ȫ�ֱ���]��
[�� �� ֵ]��
[��    ע]
*****************************************************************************/
uint8_t decimal_display(uint8_t num2)
{
 
   
    SetSegs(11,0x01,1);//С���㳣��
   
     switch(num2)
      {
        case 0:
          
          NO3_8_OFF();
          SetSegs(10,0x0F,1);  
          SetSegs(11,0x0A,1);
          break;
        
        case 1:
          NO3_8_OFF();
          SetSegs(10,0x06,1);
          SetSegs(11,0x0E,0);
          break;
        case 2:
          NO3_8_OFF();
          SetSegs(10,0x0D,1);  
          SetSegs(11,0x06,1);
          break;
        case 3:
          NO3_8_OFF();
          SetSegs(10,0x0F,1);  
          SetSegs(11,0x04,1);
          break;
        case 4:
          NO3_8_OFF();
          SetSegs(10,0x06,1);  
          SetSegs(11,0x0C,1);
          break;
        case 5:
          NO3_8_OFF();
          SetSegs(10,0x0B,1);  
          SetSegs(11,0x0C,1);
          break;
        case 6:
          NO3_8_OFF();
          SetSegs(10,0x0B,1);  
          SetSegs(11,0x0E,1);
          break;
        case 7:
          NO3_8_OFF();
          SetSegs(10,0x0E,1);
          SetSegs(11,0x0E,0);
          break;
        case 8:
          NO3_8_OFF();
          SetSegs(10,0x0F,1);  
          SetSegs(11,0x0E,1);
          break;
        case 9:
          NO3_8_OFF();
          SetSegs(10,0x0F,1);  
          SetSegs(11,0x0C,1);
          break;
        default:
          
          NO3_8_OFF();
          SetSegs(10,0x0F,1);  
          SetSegs(11,0x0A,1);
        
          break;
      }
   return 1;
}
/*****************************************************************************
[��������]NO1_8_OFF  
[��������] ȫ�ص�һ�����ְ� �������¶� 
[��    ��]
[ȫ�ֱ���]��
[�� �� ֵ]��
[��    ע]
*****************************************************************************/
void NO1_8_OFF(void)
{
  SetSegs(14,0x0F,0);  //ȫ��
  SetSegs(15,0x0E,0);  //���¶�ͼ������ȫ��
}


/*****************************************************************************
[��������]NO2_8_OFF  
[��������] ȫ�ص�2�����ְ� �������¶� 
[��    ��]
[ȫ�ֱ���]��
[�� �� ֵ]��
[��    ע]
*****************************************************************************/
void NO2_8_OFF(void)
{
  SetSegs(12,0x0F,0);  //ȫ��
  SetSegs(13,0x0E,0);  //���¶�ͼ������ȫ��
}
/*****************************************************************************
[��������]NO3_8_OFF  
[��������] ȫ�ص�һ�����ְ� �������¶� 
[��    ��]
[ȫ�ֱ���]��
[�� �� ֵ]��
[��    ע]
*****************************************************************************/
void NO3_8_OFF(void)
{
  SetSegs(10,0x0F,0);  //ȫ��
  SetSegs(11,0x0E,0);  //���¶�ͼ������ȫ��
}


/*****************************************************************************
[��������]display  
[��������]����ǧλ��ת����10λ����ʾ
[��    ��] uint16_t snum
[ȫ�ֱ���]��
[�� �� ֵ]��
[��    ע]
*****************************************************************************/
void display(uint16_t snum)
{

  uint8_t integer=0,decimals=0,tens=0,units=0;
  
  integer = (uint8_t)(snum/100);   //��ȡ����
  decimals = (snum-(integer*100))/10; //��ȡС��ֻ����һλС��
  
  tens=integer/10;      //��ȡ10λ��
  units=integer-tens*10;//��ȡ��λ��
  
  tens_display(tens);

  units_display(units);
  
  decimal_display(decimals);

} 




/*****************************************************************************
[��������]BatteryStatus  
[��������] ���״̬��ʾ 
[��    ��] num
[ȫ�ֱ���]��
[�� �� ֵ]��
[��    ע]
*****************************************************************************/
void BatteryStatus(void)
{ 
  static uint8_t i=0;
  SetSegs(9,0x01,1);  //�����߿�
  
  if(ChargerTasknum==SIX_mode)//6v��� 
  {
    SetSegs(17,0x08,0); //�ر�12V
    SetSegs(17,0x04,0); //��24V
    
    if(gBat_VoltageValue>720)//100%
    {
      SetSegs(16,0x0F,1);
      SetSegs(9,0x02,1);
      
    }else if(gBat_VoltageValue>680)//80%
    {
      SetSegs(16,0x08,0);
      SetSegs(16,0x03,1);
      SetSegs(16,0x01,1); //
      SetSegs(9,0x02,1);
      if(i == 0)
      {
        i = 1;

        SetSegs(16,0x04,1);
      }
      else
      {
        i = 0;
        SetSegs(16,0x04,0);
      }
    
    }else if(gBat_VoltageValue>630)//60%
    {
      SetSegs(16,0x0C,0);
      SetSegs(16,0x01,1); //
      SetSegs(9,0x02,1);
      if(i == 0)
      {
        i = 1;

        SetSegs(16,0x02,1);
      }
      else
      {
        i = 0;
        SetSegs(16,0x02,0);
      }
    
    }else if(gBat_VoltageValue>600)//40%
    {
      SetSegs(16,0x0E,0); //
      SetSegs(9,0x02,1);
      if(i == 0)
      {
        i = 1;

        SetSegs(16,0x01,1);
      }
      else
      {
        i = 0;
        SetSegs(16,0x01,0);
      }
    
    }else if(gBat_VoltageValue>500)//20%
    {
      
      SetSegs(16,0x0f,0);
      if(i == 0)
      {
        i = 1;
        SetSegs(9,0x02,1);
      }
      else
      {
        i = 0;
        SetSegs(9,0x02,0);
      }
    
    }else{          //20%
    
        SetSegs(16,0x0f,0);
        if(i == 0)
        {
          i = 1;
          SetSegs(9,0x02,1);
        }
        else
        {
          i = 0;
          SetSegs(9,0x02,0);
        }
    }
  }else{
    SetSegs(17,0x08,1); //����12V
    SetSegs(17,0x04,0); //��24V
    
    if(gBat_VoltageValue>1450)//100%
    {
      SetSegs(16,0x0F,1);
      SetSegs(9,0x02,1);
      
    }else if(gBat_VoltageValue>1350)//80%
    {
      SetSegs(16,0x08,0);
      SetSegs(16,0x03,1);
      SetSegs(16,0x01,1); //
      SetSegs(9,0x02,1);
      if(i == 0)
      {
        i = 1;

        SetSegs(16,0x04,1);
      }
      else
      {
        i = 0;
        SetSegs(16,0x04,0);
      }
    
    }else if(gBat_VoltageValue>1250)//60%
    {
      SetSegs(16,0x0C,0);
      SetSegs(16,0x01,1); //
      SetSegs(9,0x02,1);
      if(i == 0)
      {
        i = 1;

        SetSegs(16,0x02,1);
      }
      else
      {
        i = 0;
        SetSegs(16,0x02,0);
      }
    
    }else if(gBat_VoltageValue>1200)//40%
    {
      SetSegs(16,0x0E,0); //
      SetSegs(9,0x02,1);
      if(i == 0)
      {
        i = 1;

        SetSegs(16,0x01,1);
      }
      else
      {
        i = 0;
        SetSegs(16,0x01,0);
      }
    
    }else if(gBat_VoltageValue>1000)//20%
    {
      
      SetSegs(16,0x0f,0);
      if(i == 0)
      {
        i = 1;
        SetSegs(9,0x02,1);
      }
      else
      {
        i = 0;
        SetSegs(9,0x02,0);
      }
    
    }else{          //20%
    
        SetSegs(16,0x0f,0);
        if(i == 0)
        {
          i = 1;
          SetSegs(9,0x02,1);
        }
        else
        {
          i = 0;
          SetSegs(9,0x02,0);
        }
    }
   
  }

      
 }

 
