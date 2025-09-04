#include "User_lcd.h"

// LED����
#define LED_COUNT (sizeof(leds) / sizeof(leds[0]))

// ����ʵ�ʵĿ��ز�������


void LED_OFF_Off(void) { PB00_SETHIGH(); }
void LED_OFF_On(void)  { PB00_SETLOW(); }

void LED_Sealed_Off(void){ PA03_SETHIGH(); }
void LED_Sealed_On(void) { PA03_SETLOW(); }

void LED_AGM_Off(void) { PB01_SETHIGH(); }
void LED_AGM_On(void) { PB01_SETLOW(); }

void LED_Lidian_Off(void) { PB06_SETHIGH(); }
void LED_Lidian_On(void) { PB06_SETLOW(); }

void LED_6V_Off(void) { PA02_SETHIGH(); }
void LED_6V_On(void) { PA02_SETLOW(); }

// ����LED�ṹ������
typedef struct {
    void (*on)(void);  // ��LED�ĺ���ָ��
    void (*off)(void); // �ر�LED�ĺ���ָ��
} LED_t;


// ����LED���飬�������󶨵��ṹ����
LED_t leds[] = {
    {LED_OFF_On, LED_OFF_Off},   // LED OFF
    {LED_Sealed_On, LED_Sealed_Off}, // LED Sealed
    {LED_AGM_On, LED_AGM_Off},   // LED AGM
    {LED_Lidian_On, LED_Lidian_Off}, // LED Lidian
    {LED_6V_On, LED_6V_Off}  // LED Repair
};


// ��ָ��LED
void LED_SetOn(uint8_t index) {
    if (index < LED_COUNT) {
        leds[index].on(); // ���ö�ӦLED�Ĵ򿪺���
    }
}

// �ر�ָ��LED
void LED_SetOff(uint8_t index) {
    if (index < LED_COUNT) {
        leds[index].off(); // ���ö�ӦLED�Ĺرպ���
    }
}

// ������LED
void LED_AllOn(void) {
    for (uint8_t i = 0; i < LED_COUNT; i++) {
        leds[i].on(); // ����ÿ��LED�Ĵ򿪺���
    }
}

// �ر�����LED 
void LED_AllOff(void){
    for (uint8_t i = 0; i < LED_COUNT; i++) {
       leds[i].off(); // ����ÿ��LED�Ĺرպ���.      
    }
}

// �ر�����LED  ����ָ����LED
void LED_AlloneOff(uint8_t index){
    for (uint8_t i = 0; i < LED_COUNT; i++) {
        if(i==index){
        leds[i].on();
        }else{
        leds[i].off(); // ����ÿ��LED�Ĺرպ���.
        }
    }  
}

/**
 * @brief ���ݵ�ص�ѹ�ͳ��ģʽ����LED״̬
 * @note ��Ҫ��֤ChargerTasknum��gBat_VoltageValue����ȷ����
 */
void Update_LED_Status_By_Voltage(void)
{
    // 12VģʽĬ����ֵ (��λ: mV)
    uint16_t low_threshold  = 1300;  // 13.0V
    uint16_t mid_threshold  = 1350;  // 13.5V
    uint16_t high_threshold = 1400;  // 14.0V
    
    // 6Vģʽ��ֵ (��λ: mV)
    if(ChargerTasknum == SIXV) {
        low_threshold  = 580;   // 5.8V (�ٽ�͵���)
        mid_threshold  = 630;   // 6.3V (�еȵ���)
        high_threshold = 700;   // 7.0V (�ӽ�����)
    }

    if (ChargerTasknum == standby)//��������
    {
        StatusLEDs.LED_30 = Lights;  // LED_30������
        StatusLEDs.LED_60 = Lights;    // LED_60Ϩ��
        StatusLEDs.LED_100 = Lights;    // LED_100Ϩ�� 
        return;      
    }
    // ���ݵ�ǰ��ѹ����ֵ����LED״̬
    if(gBat_VoltageValue < low_threshold) {
        // ��ѹ״̬ (<13.0V��<5.8V)
        StatusLEDs.LED_30 = Flicker;  // LED_30������
        StatusLEDs.LED_60 = Lights;    // LED_60Ϩ��
        StatusLEDs.LED_100 = Lights;    // LED_100Ϩ��
    } 
    else if(gBat_VoltageValue < mid_threshold) {
        // ��ѹ״̬ (<13.5V��<6.3V)
        StatusLEDs.LED_30 = Always;    // LED_30����
        StatusLEDs.LED_60 = Flicker;   // LED_60������
        StatusLEDs.LED_100 = Lights;    // LED_100Ϩ��
    } 
    else if(gBat_VoltageValue < high_threshold) {
        // ��ѹ״̬ (<14.0V��<7.0V)
        StatusLEDs.LED_30 = Always;    // LED_30����
        StatusLEDs.LED_60 = Always;    // LED_60����
        StatusLEDs.LED_100 = Flicker;   // LED_100������
    } 
    else {
        // ����״̬ (>=14.0V��>=7.0V)
        StatusLEDs.LED_30 = Always;    // LED_30����
        StatusLEDs.LED_60 = Always;    // LED_60����
        StatusLEDs.LED_100 = Always;    // LED_100����
    }
}

/**
 * @brief ����״̬����
 */
void Handle_Error_State(void)
{
    static uint32_t lastBlinkTime = 0;
    static bool offLedState  = 0;
    const uint32_t blinkInterval = 250; // 500ms��˸����
    
    // 1. �ر����аٷֱ�ָʾ�ƣ���ԭ�к������ƣ�
    StatusLEDs.LED_30 = Lights;
    StatusLEDs.LED_60 = Lights;
    StatusLEDs.LED_100 = Lights;
    
    // 2. ����ָʾ����˸
    if (GetTick() - lastBlinkTime >= blinkInterval) {
        lastBlinkTime = GetTick();
        
        // �л�����ָʾ��״̬
        if (offLedState) {
            LED_SetOff(OFF_LED);  // �رմ�����
        } else {
            LED_SetOn(OFF_LED);   // ����������
        }
        offLedState = !offLedState;
        
        // ��������ģʽ�ƹر�
        LED_SetOff(Sealed_LED);
        LED_SetOff(AGM_LED);
        LED_SetOff(Lidian_LED);
        LED_SetOff(SIXV_LED);
    }
}


/**
 * @brief ���õ�ǰ���ģʽ��Ӧ��ָʾ��
 * @param mode ��ǰ���ģʽ
 */
void Set_Charge_Mode_LED(void)
{
    switch(ChargerTasknum)
    {          
        case standby:         // ����ģʽ
            LED_AlloneOff(OFF_LED);
        
            break;
            
        case Sealed:        // Ǧ����ģʽ
            LED_AlloneOff(Sealed_LED);
            break;
            
        case AGM:           // AGM���ģʽ
            LED_AlloneOff(AGM_LED);
            break;
            
        case lithium_battery:  // ﮵��ģʽ
            LED_AlloneOff(Lidian_LED);
            break;
            
        case SIXV:            // 6V���ģʽ
            LED_AlloneOff(SIXV_LED);
            break;
            
        default:              // δ֪ģʽ
            LED_AllOff();
            break;
    }
}

void LED_flash(void) // ָʾ��ˢ�º����������������� 
{
    // 1. ���º�����Ч�� - ���ڷǴ���״̬��Ч
    Breathing_Light_Update();

    // 2. ����ͬϵͳ״̬
    switch(gFaultType.Now)
    {
        case FAULT_NULL:   // �������ģʽ
            // ���µ�ص�ѹ״̬����LED״̬���ܱ��������ǣ�
            Update_LED_Status_By_Voltage();
            
            // ���ݳ��ģʽ����LED��ʾ
            Set_Charge_Mode_LED();

            break;

        case FULL_CHG:            // ����״̬
            // ���аٷֱ�ָʾ�Ƴ���
            StatusLEDs.LED_30 = Always;
            StatusLEDs.LED_60 = Always;
            StatusLEDs.LED_100 = Always;
            
            // ���ݳ��ģʽ������LED
            Set_Charge_Mode_LED();
        
            break;
        
        // ����״̬����
        case BERR:        // ��ص�ѹ����
        case COVP:        // ���������ѹ
        case COCP:        // �����������
        case HOTP:        // PCB���ӹ���
        case FJBH:        // ���ӱ���
        default:          // ��������
            Handle_Error_State();
            break;
    }
}

