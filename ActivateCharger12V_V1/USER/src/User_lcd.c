#include "User_lcd.h"

uint8_t m_au8Value[TM1621D_BYTES]={0};
uint8_t B_V_FLAG=0,B_V_cnt=0;
// 初始化 TM1621D
void TM1621_Init(void)
{
      //打开GPIO时钟
    __SYSCTRL_GPIOA_CLK_ENABLE();
    __SYSCTRL_GPIOB_CLK_ENABLE();
    // 初始化 GPIO 方向，作为输出
    GPIO_InitTypeDef LCD_InitStruct = {0};
   
    LCD_InitStruct.Pins = TM1621_CS_PIN;
    LCD_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    LCD_InitStruct.IT   = GPIO_IT_NONE;
   
    GPIO_Init(CW_GPIOA, &LCD_InitStruct);
    
    LCD_InitStruct.Pins = TM1621_WR_PIN | TM1621_DATA_PIN;
    LCD_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    LCD_InitStruct.IT   = GPIO_IT_NONE;
   
    GPIO_Init(CW_GPIOB, &LCD_InitStruct);
    

    TM1621_CS_HIGH;  // 拉高 CS，不使能
    TM1621_WR_HIGH;  // WR 拉高
    TM1621_DATA_HIGH;  // 数据线拉高
    
}




/*****************************************************************************
[函数名称]SendHighBits
[函数功能]写入高位   一位一位的发送
[参    数]
[全局变量]无
[返 回 值]无
[备    注]
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
[函数名称]SendLowBits
[函数功能]写入低位
[参    数]
[全局变量]无
[返 回 值]无
[备    注] 数据位
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
[函数名称]SendData
[函数功能]写模式 写数据到RAM中
[参    数]
[全局变量]无
[返 回 值]无
[备    注]
*****************************************************************************/
void SendData(uint8_t u8Addr,uint8_t u8Data)
{
	TM1621_CS_LOW; 
	SendHighBits(WRITE<<5,3);		//发送一个101
	SendHighBits(u8Addr<<2,6);		//发送6位的地址
	SendLowBits(u8Data,4);			//发送4位的数据
	TM1621_CS_HIGH;
}

/*****************************************************************************
[函数名称]vSetSeg
[函数功能]
[全局变量]无
[返 回 值]
[备    注] u8Addr seg地址   u8Bit com地址  bOn 置1或0 
*****************************************************************************/
void SetSeg( uint8_t u8Addr,uint8_t u8Bit,uint8_t bOn )
{
	if(u8Addr < TM1621D_BYTES)
	{
		uint8_t u8Write = 0;
		
		u8Write = m_au8Value[u8Addr];//读取对应的seg脚状态
		
		if(bOn)		//数据写1或0
		{
			u8Write |= (0x01 << u8Bit);			//将该位置1
		}
		else
		{
			u8Write &= ~(0x01 << u8Bit);		//将该位置0
		}
		u8Write &= 0x0F;
		m_au8Value[u8Addr] = u8Write;

		SendData(u8Addr,u8Write);		//保存对应的seg脚状态
	}
}
/*****************************************************************************
[函数名称]SetSegs 改进
[函数功能]
[全局变量]无
[返 回 值]
[备    注] u8Addr seg地址   u8Bit com地址  bOn 置1或0 
*****************************************************************************/
void SetSegs(uint8_t u8Addr, uint8_t u8BitMask, uint8_t bOn)
{
    if(u8Addr < TM1621D_BYTES)
    {
        uint8_t u8Write = m_au8Value[u8Addr];  // 获取当前 SEG 引脚的状态
        
        if(bOn)  // 如果要点亮指定位
        {
            u8Write |= u8BitMask;  // 将 u8BitMask 中设置的位全部置 1
        }
        else  // 如果要熄灭指定位
        {
            u8Write &= ~u8BitMask;  // 将 u8BitMask 中设置的位全部清 0
        }
        
        u8Write &= 0x0F;  // 只保留低 4 位，确保不影响其他位
        m_au8Value[u8Addr] = u8Write;  // 更新 m_au8Value 中的状态
        
        SendData(u8Addr, u8Write);  // 将新的状态写入 TM1621D
    }
}

/*****************************************************************************
[函数名称]vClearScreen
[函数功能]
[全局变量]无
[返 回 值]
[备    注]
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
[函数名称]vFillScreen
[函数功能]
[全局变量]无
[返 回 值]
[备    注]
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
[函数名称]vSendCommand   
[函数功能]命令模式
[参    数]
[全局变量]无
[返 回 值]无
[备    注]
*****************************************************************************/
void SendCommand(uint8_t u8Command)
{
	TM1621_CS_LOW;
	SendHighBits(COMMAND << 5,3);		//发送一个100
	SendHighBits(u8Command,9);				//发送 9位命令码
	TM1621_CS_HIGH;
}

uint8_t LcdInit( void )
{

	TM1621_CS_LOW;	
	TM1621_CS_HIGH;
	TM1621_CS_LOW;		//高电平脉冲初始化HT1621串行接口	
	// 初始化液晶参数

	SendCommand(TM1621D_SYSEN);		//打开系统振荡器
  SendCommand(TM1621D_BIAS);			//偏压和端口选择
  
	SendCommand(TM1621D_LCDOFF);		//关闭LCD显示
	SendCommand(TM1621D_LCDON);		//打开LCD显示
	//检查一下段码屏	
	FillScreen();  //全亮
  
  FB_ON;
 
	SysTickDelay(500);	
  FB_OFF;
  SysTickDelay(500);
  ALL_LEDON();
	ClearScreen();//全灭 
  
 
	return 0;
}

/*****************************************************************************
[函数名称]Screen_refresh   
[函数功能]Lcd屏幕刷新  温度，电压，电流交替显示
[参    数]
[全局变量]无
[返 回 值]无
[备    注]
*****************************************************************************/
void Screen_refresh( void )
{
 
  static uint8_t FUL_flag=0,REP_flag=0,Standby_flag=0; //  清屏标志位
  static uint8_t oneflag[10]={0};
  static uint8_t i=0;
  if(gFaultType.Now!=FAULT_NULL)
  {
   Standby_flag=0;Standby_flag=0;FUL_flag=0;

  }
   switch(gFaultType.Now)
    {
      case FAULT_NULL:          /* 无故障                   */
        

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
             
             SetSegs(9,0x01,1);  //点亮边框
             SetSegs(16,0x0F,1);//百分百电量
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
                  
                   switch(B_V_FLAG) //电量电压显示
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
                       SendData(18, 0x08);//℃
                     
                       break;
                   }

                   //电量显示
                   BatteryStatus();
                   
                   
                  if(jwen_v==1){   //冬夏显示
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
     case INF:                 /* 输入异常                 */
       break;
     case HOTP:                /* 硬件过温                 */
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
       
     case ROTP:                /* 软件过温                 */
       
       break;
     case COVP:                /* 充电机输出过压           */
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
     case COCP:                /* 充电机输出过流           */
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
     case BOVP:                /* 电池过压                 */
       FUL_flag=0;
       break;
     
     case BERR:                /* 电池异常                 */
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
     case LERR:                /* 电池异常                 */
       
       break;
     case FULL_CHG:            /* 充满                     */
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
       
       
       SetSegs(16,0x0F,1);//百分百电量
       SetSegs(9,0x03,1);

       break;
     case FJBH:                /* 反接保护                 */
       
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
       
     case STANDBY:           /* 未连接           */
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
       SetSegs(9,0x01,0);  //点亮边框
         i=0;
       }else{
       SetSegs(9,0x01,1);  //点亮边框
        i=1;
       }
       
       break;
     
    
    }
      
}



//整数位加小数位  整数位再分为十位数和个位数    显示由十位数，个位数，小数位组成   24V  2400

/*****************************************************************************
[函数名称]Integer_display  
[函数功能] 十位数显示 
[参    数] num 小于100的整数
[全局变量]无
[返 回 值]无
[备    注]
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
[函数名称]decimals_display  
[函数功能] 个位数显示 
[参    数] num
[全局变量]无
[返 回 值]无
[备    注]
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
[函数名称]decimals_display  
[函数功能] 个位数显示 
[参    数] num
[全局变量]无
[返 回 值]无
[备    注]
*****************************************************************************/
uint8_t decimal_display(uint8_t num2)
{
 
   
    SetSegs(11,0x01,1);//小数点常亮
   
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
[函数名称]NO1_8_OFF  
[函数功能] 全关第一个数字八 但不关温度 
[参    数]
[全局变量]无
[返 回 值]无
[备    注]
*****************************************************************************/
void NO1_8_OFF(void)
{
  SetSegs(14,0x0F,0);  //全关
  SetSegs(15,0x0E,0);  //除温度图标其余全灭
}


/*****************************************************************************
[函数名称]NO2_8_OFF  
[函数功能] 全关第2个数字八 但不关温度 
[参    数]
[全局变量]无
[返 回 值]无
[备    注]
*****************************************************************************/
void NO2_8_OFF(void)
{
  SetSegs(12,0x0F,0);  //全关
  SetSegs(13,0x0E,0);  //除温度图标其余全灭
}
/*****************************************************************************
[函数名称]NO3_8_OFF  
[函数功能] 全关第一个数字八 但不关温度 
[参    数]
[全局变量]无
[返 回 值]无
[备    注]
*****************************************************************************/
void NO3_8_OFF(void)
{
  SetSegs(10,0x0F,0);  //全关
  SetSegs(11,0x0E,0);  //除温度图标其余全灭
}


/*****************************************************************************
[函数名称]display  
[函数功能]传入千位数转换成10位数显示
[参    数] uint16_t snum
[全局变量]无
[返 回 值]无
[备    注]
*****************************************************************************/
void display(uint16_t snum)
{

  uint8_t integer=0,decimals=0,tens=0,units=0;
  
  integer = (uint8_t)(snum/100);   //获取整数
  decimals = (snum-(integer*100))/10; //获取小数只保留一位小数
  
  tens=integer/10;      //获取10位数
  units=integer-tens*10;//获取个位数
  
  tens_display(tens);

  units_display(units);
  
  decimal_display(decimals);

} 




/*****************************************************************************
[函数名称]BatteryStatus  
[函数功能] 充电状态显示 
[参    数] num
[全局变量]无
[返 回 值]无
[备    注]
*****************************************************************************/
void BatteryStatus(void)
{ 
  static uint8_t i=0;
  SetSegs(9,0x01,1);  //点亮边框
  
  if(ChargerTasknum==SIX_mode)//6v电池 
  {
    SetSegs(17,0x08,0); //关闭12V
    SetSegs(17,0x04,0); //关24V
    
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
    SetSegs(17,0x08,1); //点亮12V
    SetSegs(17,0x04,0); //关24V
    
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

 
