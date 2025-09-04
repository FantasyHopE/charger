#include "User_gpio.h"

//OFFLED        PB0
//12VAGM        PB1
//�ٷ�֮30      PB2
//��������      PB3
//�ٷ�֮100     PB4
//����PWM       PB5
//12V﮵��     PB6
//��������            PB7

//�ٷ�֮60      PA0
//�������      PA1
//6V���        PA2
//12VǦ��       PA3
//�¶Ȳ���      PA4
//B-            PA5
//B+            PA6


void User_gpio_init(void)
{

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
   __SYSCTRL_GPIOA_CLK_ENABLE();    //Open GPIOA Clk
    
   __SYSCTRL_GPIOB_CLK_ENABLE();    //Open GPIOA Clk
    
    GPIO_RST2GPIO();//��PB07���ó�GPIO��ֻ������
    
    //PB7 ���밴�����
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

volatile uint8_t buttonState = BUTTON_IDLE;  // ����״̬

Tasknum  ChargerTasknum = standby;
Tasknum  Chargermode_Flash = standby;

// ģ���ȡϵͳʱ��ĺ���������Ը��ݾ���Ӳ��ʵ�֣�
uint32_t GetSysTime(void) {
    return GetTick();  // ���� currentTime ����ʱ�����
}

void ButtonTask(void) {
    static uint8_t key_flag = 1;
    static uint32_t pressStartTime = 0;
    static bool longPressExecuted = false;  // ��������ִ�б�־

    const uint32_t currentTime = GetSysTime();
    
    key_flag = (GPIO_ReadPin(CW_GPIOB, GPIO_PIN_7) == 1) ? 1 : 0;

    if (key_flag == 0) {  // ��������
        if (!longPressExecuted) {  // ����δִ�г���ʱ�������¼�
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
                        longPressExecuted = true;  // ��ǳ�����ִ��
                        buttonState = BUTTON_IDLE; // ������λ״̬��
                        pressStartTime = 0;        // �����ʱ
                        key_flag = 1;              // ģ�ⰴ���ͷ�
                    }
                    break;
            }
        }
    } else {  // �����ɿ�
        if (buttonState == BUTTON_PRESSED && !longPressExecuted) {
            // ����δִ�г���ʱ����̰�
            HandleShortPress();
        }
        // ��λ״̬�����־λ
        buttonState = BUTTON_IDLE;
        longPressExecuted = false;
    }

    key_flag = GPIO_ReadPin(CW_GPIOB, GPIO_PIN_7);
}

// ��������������
void HandleLongPressImmediate(void) {

  gChgType.Now = WAIT_BATT;
  ChargerTasknum =(Tasknum)4;
  FEE_wr((uint16_t)ChargerTasknum, ChargerMode_ID);
}

// �̰�������
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
