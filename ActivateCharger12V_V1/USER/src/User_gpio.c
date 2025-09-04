#include "User_gpio.h"

//LED2  PA02
//LED1  PB02
//KEY   PB07
//FB ������  PB06
//FAN ������  PB06


void User_gpio_init(void)
{

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    GPIO_RST2GPIO();                //PB07��Ϊ��ͨIO ʹ��
    
   __SYSCTRL_GPIOA_CLK_ENABLE();    //Open GPIOA Clk
    
   __SYSCTRL_GPIOB_CLK_ENABLE();    //Open GPIOA Clk
    
    //PB7 ���밴�����
    GPIO_InitStruct.Pins =  GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT_PULLUP;
    GPIO_InitStruct.IT   = GPIO_IT_NONE;
    
    GPIO_Init( CW_GPIOB, &GPIO_InitStruct);

    //PB6 ����������
    GPIO_InitStruct.Pins =  GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.IT   = GPIO_IT_NONE;
    
    GPIO_Init( CW_GPIOB, &GPIO_InitStruct);
    
    //PA0 ����   PA1 ����������� 
    GPIO_InitStruct.Pins =  GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.IT   = GPIO_IT_NONE;
    
    GPIO_Init(CW_GPIOA, &GPIO_InitStruct);
    FB_OFF;
    SD_OFF;
    OUT_OFF;
}



/***********************************************************************
  * @ ������  �� led2_deint
  * @ ����˵���� ʧ��IO
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  **********************************************************************/
void  led2_deint(void)//ʧ��LED2
{
     GPIO_DeInit(CW_GPIOA,GPIO_PIN_2);
}

void  led1_deint(void)//ʧ��LED1 
{
     GPIO_DeInit(CW_GPIOB,GPIO_PIN_2);
}

/***********************************************************************
  * @ ������  �� led2_init
  * @ ����˵���� ����LED ��ѡ��һ������
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  **********************************************************************/
void led2_init(uint8_t S_R)//ʹ��LED
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

void led1_init(uint8_t S_R)//ʹ��LED
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
  * @ ������  �� Automobile_ledon
  * @ ����˵���� ����LED  Ӳ��ԭ�� ͬʱֻ�ܵ���һ��LED 
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  **********************************************************************/

void Sealed_ledon(void) //����
{
   ALL_LEDOFF();
   led1_init(SET);
}
void Start_stop_ledon(void)//��ͣ
{
   ALL_LEDOFF();
   led1_init(RESET);

}
void Motorcycle_ledon(void)//Ħ��
{
   ALL_LEDOFF();
   led2_init(SET);
}
void SIX_ledon(void)//�޸�
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

volatile uint8_t buttonState = BUTTON_IDLE;  // ����״̬



// ģ���ȡϵͳʱ��ĺ���������Ը��ݾ���Ӳ��ʵ�֣�
uint32_t GetSysTime(void) {
    return GetTick();  // ���� currentTime ����ʱ�����
}

void ButtonTask(void) {
    static uint8_t fb_flag = 0;
    static uint8_t key_flag = 1;
    static uint32_t pressStartTime = 0;
    static bool longPressExecuted = false;  // ��������ִ�б�־

    if (fb_flag) {
        FB_OFF;
        fb_flag = 0;
    }
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
    // ״̬�л��߼�
    gChgType.Now = WAIT_BATT;
    B_V_FLAG = 0;
    B_V_cnt = 0;
    ClearScreen();
//    FB_ON;fb_flag=1; // ����������

    // ģʽ�л��߼�����ԭ�ɿ�ʱ�߼�һ�£�
  
  
   if((ChargerTasknum==power_supply_mode)||(ChargerTasknum==Repair_mode))
    {
      ChargerTasknum=Standby_mode;
    }else{
          if(gFaultType.Now==STANDBY)   //�ȴ��޴���
          {
           ChargerTasknum = power_supply_mode;   
          }else{
           ChargerTasknum = Repair_mode;
          }
    }
        
}

// �̰�������
void HandleShortPress(void) {
    gChgType.Now = WAIT_BATT;
    B_V_FLAG = 0;
    B_V_cnt = 0;
    ClearScreen();
//   FB_ON;fb_flag=1;  // ����������

    // ģʽ�л��߼�
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

    if(ChargerTasknum >= Sealed_mode && ChargerTasknum <= SIX_mode)//��ǰģʽ����flash��ֻ��������ģʽ
    {
      FEE_wr((uint16_t)ChargerTasknum, ChargerMode_ID);
    }else{
      FEE_wr((uint16_t)Standby_mode, ChargerMode_ID);//����ģʽͳһ����Ϊ����
    }
    
    ChargerTasknum %= 5; 
}
