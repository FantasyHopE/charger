#include "User_gpio.h"

//LED2  PA02
//LED1  PB02
//KEY   PB07
//FB 蜂鸣器  PB06
//FAN 蜂鸣器  PB06


void User_gpio_init(void)
{

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    GPIO_RST2GPIO();                //PB07作为普通IO 使用
    
   __SYSCTRL_GPIOA_CLK_ENABLE();    //Open GPIOA Clk
    
   __SYSCTRL_GPIOB_CLK_ENABLE();    //Open GPIOA Clk
    
    //PB7 输入按键检测
    GPIO_InitStruct.Pins =  GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT_PULLUP;
    GPIO_InitStruct.IT   = GPIO_IT_NONE;
    
    GPIO_Init( CW_GPIOB, &GPIO_InitStruct);

    //PB6 蜂鸣器控制
    GPIO_InitStruct.Pins =  GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.IT   = GPIO_IT_NONE;
    
    GPIO_Init( CW_GPIOB, &GPIO_InitStruct);
    
    //PA0 风扇   PA1 控制输出开关 
    GPIO_InitStruct.Pins =  GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.IT   = GPIO_IT_NONE;
    
    GPIO_Init(CW_GPIOA, &GPIO_InitStruct);
    FB_OFF;
    SD_OFF;
    OUT_OFF;
}



/***********************************************************************
  * @ 函数名  ： led2_deint
  * @ 功能说明： 失能IO
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  **********************************************************************/
void  led2_deint(void)//失能LED2
{
     GPIO_DeInit(CW_GPIOA,GPIO_PIN_2);
}

void  led1_deint(void)//失能LED1 
{
     GPIO_DeInit(CW_GPIOB,GPIO_PIN_2);
}

/***********************************************************************
  * @ 函数名  ： led2_init
  * @ 功能说明： 点亮LED 并选择一个灯亮
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  **********************************************************************/
void led2_init(uint8_t S_R)//使能LED
{
    GPIO_InitTypeDef  LED_InitStruct={0};
      //set PA02 as output
    LED_InitStruct.Pins = GPIO_PIN_2;
    LED_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    
    GPIO_Init( CW_GPIOA, &LED_InitStruct);
    if(S_R==GPIO_Pin_SET)
    {
      GPIO_WritePin( CW_GPIOA, GPIO_PIN_2,GPIO_Pin_SET);
    }
    else{
      GPIO_WritePin( CW_GPIOA, GPIO_PIN_2,GPIO_Pin_RESET);
    }
}

void led1_init(uint8_t S_R)//使能LED
{
    GPIO_InitTypeDef  LED_InitStruct={0};
      //set PA02 as output
    LED_InitStruct.Pins = GPIO_PIN_2;
    LED_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    
    GPIO_Init( CW_GPIOB, &LED_InitStruct);
    
    if(S_R==GPIO_Pin_SET)
    {
      GPIO_WritePin( CW_GPIOB, GPIO_PIN_2,GPIO_Pin_SET);
    }
    else{
      GPIO_WritePin( CW_GPIOB, GPIO_PIN_2,GPIO_Pin_RESET);
    }
}



/***********************************************************************
  * @ 函数名  ： Automobile_ledon
  * @ 功能说明： 点亮LED  硬件原因 同时只能点亮一个LED 
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  **********************************************************************/

void Sealed_ledon(void) //汽车
{
   ALL_LEDOFF();
   led1_init(SET);
}
void Start_stop_ledon(void)//启停
{
   ALL_LEDOFF();
   led1_init(RESET);

}
void Motorcycle_ledon(void)//摩托
{
   ALL_LEDOFF();
   led2_init(SET);
}
void SIX_ledon(void)//修复
{
   ALL_LEDOFF();
   led2_init(RESET);
}

void ALL_LEDOFF(void)
{
  led2_deint();
  led1_deint();
}

void ALL_LEDON(void)
{
  Sealed_ledon();
  SysTickDelay(400);
  Start_stop_ledon();
  SysTickDelay(400);
  Motorcycle_ledon();
  SysTickDelay(400);
  SIX_ledon();
  SysTickDelay(400);
  ALL_LEDOFF();
}

Tasknum  ChargerTasknum = Standby_mode;
Tasknum  Chargermode_Flash = Standby_mode;

volatile uint32_t pressStartTime = 0;
volatile uint32_t currentTime = 0;

volatile uint8_t buttonState = BUTTON_IDLE;  // 按键状态



// 模拟获取系统时间的函数（你可以根据具体硬件实现）
uint32_t GetSysTime(void) {
    return GetTick();  // 假设 currentTime 随着时间递增
}

void ButtonTask(void) {
    static uint8_t fb_flag = 0;
    static uint8_t key_flag = 1;
    static uint32_t pressStartTime = 0;
    static bool longPressExecuted = false;  // 新增长按执行标志

    if (fb_flag) {
        FB_OFF;
        fb_flag = 0;
    }
    const uint32_t currentTime = GetSysTime();
    
    key_flag = (GPIO_ReadPin(CW_GPIOB, GPIO_PIN_7) == 1) ? 1 : 0;

    if (key_flag == 0) {  // 按键按下
        if (!longPressExecuted) {  // 仅在未执行长按时处理按下事件
            switch (buttonState) {
                case BUTTON_IDLE:
                    buttonState = BUTTON_DEBOUNCE;
                    pressStartTime = currentTime;
                    break;

                case BUTTON_DEBOUNCE:
                    if ((currentTime - pressStartTime) >= DEBOUNCE_DELAY_MS) {
                        buttonState = BUTTON_PRESSED;
                    }
                    break;

                case BUTTON_PRESSED:
                    if ((currentTime - pressStartTime) >= LONG_PRESS_THRESHOLD_MS) {
                        HandleLongPressImmediate();
                        longPressExecuted = true;  // 标记长按已执行
                        buttonState = BUTTON_IDLE; // 立即复位状态机
                        pressStartTime = 0;        // 清除计时
                        key_flag = 1;              // 模拟按键释放
                    }
                    break;
            }
        }
    } else {  // 按键松开
        if (buttonState == BUTTON_PRESSED && !longPressExecuted) {
            // 仅在未执行长按时处理短按
            HandleShortPress();
        }
        // 复位状态机与标志位
        buttonState = BUTTON_IDLE;
        longPressExecuted = false;
    }

    key_flag = GPIO_ReadPin(CW_GPIOB, GPIO_PIN_7);
}

// 长按立即处理函数
void HandleLongPressImmediate(void) {
    // 状态切换逻辑
    gChgType.Now = WAIT_BATT;
    B_V_FLAG = 0;
    B_V_cnt = 0;
    ClearScreen();
//    FB_ON;fb_flag=1; // 触发蜂鸣器

    // 模式切换逻辑（与原松开时逻辑一致）
  
  
   if((ChargerTasknum==power_supply_mode)||(ChargerTasknum==Repair_mode))
    {
      ChargerTasknum=Standby_mode;
    }else{
          if(gFaultType.Now==STANDBY)   //等待无错误
          {
           ChargerTasknum = power_supply_mode;   
          }else{
           ChargerTasknum = Repair_mode;
          }
    }
        
}

// 短按处理函数
void HandleShortPress(void) {
    gChgType.Now = WAIT_BATT;
    B_V_FLAG = 0;
    B_V_cnt = 0;
    ClearScreen();
//   FB_ON;fb_flag=1;  // 触发蜂鸣器

    // 模式切换逻辑
    if (ChargerTasknum == Standby_mode) {
        ChargerTasknum = (gFaultType.Now == FAULT_NULL) ? Sealed_mode : Standby_mode;
    } else if (ChargerTasknum == power_supply_mode) {
        ChargerTasknum = Standby_mode;
    } else if (ChargerTasknum == Repair_mode) {
        ChargerTasknum = Sealed_mode;
    } else {
        ChargerTasknum  += 1;
    }
    
    if((FaultLock==1)||(BERR_flag==1))
    {
      ChargerTasknum = Standby_mode;
      FaultLock=0;
      BERR_flag=0;
    }

    if(ChargerTasknum >= Sealed_mode && ChargerTasknum <= SIX_mode)//当前模式存入flash，只存三种主模式
    {
      FEE_wr((uint16_t)ChargerTasknum, ChargerMode_ID);
    }else{
      FEE_wr((uint16_t)Standby_mode, ChargerMode_ID);//其他模式统一保存为待机
    }
    
    ChargerTasknum %= 5; 
}
