#include "User_lcd.h"

// LED总数
#define LED_COUNT (sizeof(leds) / sizeof(leds[0]))

// 定义实际的开关操作函数


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

// 定义LED结构体类型
typedef struct {
    void (*on)(void);  // 打开LED的函数指针
    void (*off)(void); // 关闭LED的函数指针
} LED_t;


// 定义LED数组，将函数绑定到结构体中
LED_t leds[] = {
    {LED_OFF_On, LED_OFF_Off},   // LED OFF
    {LED_Sealed_On, LED_Sealed_Off}, // LED Sealed
    {LED_AGM_On, LED_AGM_Off},   // LED AGM
    {LED_Lidian_On, LED_Lidian_Off}, // LED Lidian
    {LED_6V_On, LED_6V_Off}  // LED Repair
};


// 打开指定LED
void LED_SetOn(uint8_t index) {
    if (index < LED_COUNT) {
        leds[index].on(); // 调用对应LED的打开函数
    }
}

// 关闭指定LED
void LED_SetOff(uint8_t index) {
    if (index < LED_COUNT) {
        leds[index].off(); // 调用对应LED的关闭函数
    }
}

// 打开所有LED
void LED_AllOn(void) {
    for (uint8_t i = 0; i < LED_COUNT; i++) {
        leds[i].on(); // 调用每个LED的打开函数
    }
}

// 关闭所有LED 
void LED_AllOff(void){
    for (uint8_t i = 0; i < LED_COUNT; i++) {
       leds[i].off(); // 调用每个LED的关闭函数.      
    }
}

// 关闭所有LED  除了指定的LED
void LED_AlloneOff(uint8_t index){
    for (uint8_t i = 0; i < LED_COUNT; i++) {
        if(i==index){
        leds[i].on();
        }else{
        leds[i].off(); // 调用每个LED的关闭函数.
        }
    }  
}

/**
 * @brief 根据电池电压和充电模式更新LED状态
 * @note 需要保证ChargerTasknum和gBat_VoltageValue已正确更新
 */
void Update_LED_Status_By_Voltage(void)
{
    // 12V模式默认阈值 (单位: mV)
    uint16_t low_threshold  = 1300;  // 13.0V
    uint16_t mid_threshold  = 1350;  // 13.5V
    uint16_t high_threshold = 1400;  // 14.0V
    
    // 6V模式阈值 (单位: mV)
    if(ChargerTasknum == SIXV) {
        low_threshold  = 580;   // 5.8V (临界低电量)
        mid_threshold  = 630;   // 6.3V (中等电量)
        high_threshold = 700;   // 7.0V (接近满电)
    }

    if (ChargerTasknum == standby)//待机不亮
    {
        StatusLEDs.LED_30 = Lights;  // LED_30呼吸灯
        StatusLEDs.LED_60 = Lights;    // LED_60熄灭
        StatusLEDs.LED_100 = Lights;    // LED_100熄灭 
        return;      
    }
    // 根据当前电压和阈值更新LED状态
    if(gBat_VoltageValue < low_threshold) {
        // 低压状态 (<13.0V或<5.8V)
        StatusLEDs.LED_30 = Flicker;  // LED_30呼吸灯
        StatusLEDs.LED_60 = Lights;    // LED_60熄灭
        StatusLEDs.LED_100 = Lights;    // LED_100熄灭
    } 
    else if(gBat_VoltageValue < mid_threshold) {
        // 中压状态 (<13.5V或<6.3V)
        StatusLEDs.LED_30 = Always;    // LED_30常亮
        StatusLEDs.LED_60 = Flicker;   // LED_60呼吸灯
        StatusLEDs.LED_100 = Lights;    // LED_100熄灭
    } 
    else if(gBat_VoltageValue < high_threshold) {
        // 高压状态 (<14.0V或<7.0V)
        StatusLEDs.LED_30 = Always;    // LED_30常亮
        StatusLEDs.LED_60 = Always;    // LED_60常亮
        StatusLEDs.LED_100 = Flicker;   // LED_100呼吸灯
    } 
    else {
        // 满电状态 (>=14.0V或>=7.0V)
        StatusLEDs.LED_30 = Always;    // LED_30常亮
        StatusLEDs.LED_60 = Always;    // LED_60常亮
        StatusLEDs.LED_100 = Always;    // LED_100常亮
    }
}

/**
 * @brief 错误状态处理
 */
void Handle_Error_State(void)
{
    static uint32_t lastBlinkTime = 0;
    static bool offLedState  = 0;
    const uint32_t blinkInterval = 250; // 500ms闪烁周期
    
    // 1. 关闭所有百分比指示灯（由原有函数控制）
    StatusLEDs.LED_30 = Lights;
    StatusLEDs.LED_60 = Lights;
    StatusLEDs.LED_100 = Lights;
    
    // 2. 待机指示灯闪烁
    if (GetTick() - lastBlinkTime >= blinkInterval) {
        lastBlinkTime = GetTick();
        
        // 切换待机指示灯状态
        if (offLedState) {
            LED_SetOff(OFF_LED);  // 关闭待机灯
        } else {
            LED_SetOn(OFF_LED);   // 开启待机灯
        }
        offLedState = !offLedState;
        
        // 保持其他模式灯关闭
        LED_SetOff(Sealed_LED);
        LED_SetOff(AGM_LED);
        LED_SetOff(Lidian_LED);
        LED_SetOff(SIXV_LED);
    }
}


/**
 * @brief 设置当前充电模式对应的指示灯
 * @param mode 当前充电模式
 */
void Set_Charge_Mode_LED(void)
{
    switch(ChargerTasknum)
    {          
        case standby:         // 待机模式
            LED_AlloneOff(OFF_LED);
        
            break;
            
        case Sealed:        // 铅酸电池模式
            LED_AlloneOff(Sealed_LED);
            break;
            
        case AGM:           // AGM电池模式
            LED_AlloneOff(AGM_LED);
            break;
            
        case lithium_battery:  // 锂电池模式
            LED_AlloneOff(Lidian_LED);
            break;
            
        case SIXV:            // 6V电池模式
            LED_AlloneOff(SIXV_LED);
            break;
            
        default:              // 未知模式
            LED_AllOff();
            break;
    }
}

void LED_flash(void) // 指示灯刷新函数，在任务中运行 
{
    // 1. 更新呼吸灯效果 - 仅在非错误状态有效
    Breathing_Light_Update();

    // 2. 处理不同系统状态
    switch(gFaultType.Now)
    {
        case FAULT_NULL:   // 正常充电模式
            // 更新电池电压状态（但LED状态可能被后续覆盖）
            Update_LED_Status_By_Voltage();
            
            // 根据充电模式设置LED显示
            Set_Charge_Mode_LED();

            break;

        case FULL_CHG:            // 充满状态
            // 所有百分比指示灯常亮
            StatusLEDs.LED_30 = Always;
            StatusLEDs.LED_60 = Always;
            StatusLEDs.LED_100 = Always;
            
            // 根据充电模式设置主LED
            Set_Charge_Mode_LED();
        
            break;
        
        // 错误状态处理
        case BERR:        // 电池电压错误
        case COVP:        // 充电机输出过压
        case COCP:        // 充电机输出过流
        case HOTP:        // PCB板子过温
        case FJBH:        // 反接保护
        default:          // 其他错误
            Handle_Error_State();
            break;
    }
}

