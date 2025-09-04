#include "User_gpio.h"

//OFFLED        PB0
//12VAGM        PB1
//百分之30      PB2
//电流采样      PB3
//百分之100     PB4
//电流PWM       PB5
//12V锂电池     PB6
//按键输入            PB7

//百分之60      PA0
//输出开关      PA1
//6V电池        PA2
//12V铅酸       PA3
//温度采样      PA4
//B-            PA5
//B+            PA6


void User_gpio_init(void)
{

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
   __SYSCTRL_GPIOA_CLK_ENABLE();    //Open GPIOA Clk
    
   __SYSCTRL_GPIOB_CLK_ENABLE();    //Open GPIOA Clk
    
    GPIO_RST2GPIO();//将PB07设置成GPIO，只能输入
    
    //PB7 输入按键检测
    GPIO_InitStruct.Pins =  GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT_PULLUP;
    GPIO_InitStruct.IT   = GPIO_IT_NONE;
    
    GPIO_Init(CW_GPIOB, &GPIO_InitStruct);
    
    
    //PA1  PA2  PA3 
    GPIO_InitStruct.Pins =  GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.IT   = GPIO_IT_NONE;
    
    GPIO_Init(CW_GPIOA, &GPIO_InitStruct);

    //PB0  1  6
    GPIO_InitStruct.Pins =  GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.IT   = GPIO_IT_NONE;
    
    GPIO_Init( CW_GPIOB, &GPIO_InitStruct);
   

}



volatile uint32_t pressStartTime = 0;
volatile uint32_t currentTime = 0;

volatile uint8_t buttonState = BUTTON_IDLE;  // 按键状态

Tasknum  ChargerTasknum = standby;
Tasknum  Chargermode_Flash = standby;

// 模拟获取系统时间的函数（你可以根据具体硬件实现）
uint32_t GetSysTime(void) {
    return GetTick();  // 假设 currentTime 随着时间递增
}

void ButtonTask(void) {
    static uint8_t key_flag = 1;
    static uint32_t pressStartTime = 0;
    static bool longPressExecuted = false;  // 新增长按执行标志

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

  gChgType.Now = WAIT_BATT;
  ChargerTasknum =(Tasknum)4;
  FEE_wr((uint16_t)ChargerTasknum, ChargerMode_ID);
}

// 短按处理函数
void HandleShortPress(void) {
  
    gChgType.Now = WAIT_BATT;

    if(ChargerTasknum==0x04){
      ChargerTasknum=(Tasknum)0;
    }else{
     ChargerTasknum += 1;
     ChargerTasknum %= 4;
    }

    FEE_wr((uint16_t)ChargerTasknum, ChargerMode_ID);

}
