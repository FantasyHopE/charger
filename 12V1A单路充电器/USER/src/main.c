#include "main.h"

void bsp_init(void);

 
int main(void)
{
   bsp_init();//硬件初始化
   uint8_t i=0;
  
   while(1){
     
      IWDT_Refresh();//喂狗 超时时间两秒
      for (i=0; i<TASK_MAX; i++)
      {
        if (gTaskDelay[i] == 1)
        {
          switch (i)
          {
            
            case 0:  Task0(); break;
            case 1:  Task1(); break;
            case 2:  Task2(); break;
            case 3:  Task3(); break;
            case 4:  Task4(); break;
            case 5:  Task5(); break;
            case 6:  Task6(); break;

            default: break;
          }
          /* 跳出for语句，实现优先级设定 */
          break;
        }
      }
  }
}


/***********************************************************************
  * @ 函数名  ： bsp_init
  * @ 功能说明： 硬件初始化
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  **********************************************************************/
void bsp_init(void)
{
   SYSCTRL_SYSCLKSRC_Config(SYSCTRL_SYSCLKSRC_HSI);
    
   SYSCTRL_HSI_Enable(SYSCTRL_HSIOSC_DIV1);
  
          /* 1. 设置HCLK和PCLK的分频系数　*/
   SYSCTRL_HCLKPRS_Config(SYSCTRL_HCLK_DIV1);
   SYSCTRL_PCLKPRS_Config(SYSCTRL_PCLK_DIV4);//ADC采样时钟 12mhz
    
   __SYSCTRL_IWDT_CLK_ENABLE(); 
  
   InitTick(48000000);//使用sysTick作为1ms
  
   User_gpio_init();
  
   User_adc_init();


   PWM_OutputConfig();
// UART_Configuration();
  
   TaskDelayInit();
   
   IWDT_SetPeriod(IWDT_2_SECS); 
   
   LED_AllOff();
   
   FEE_init();//内部存储初始化
   
   Chargermode_Flash = (Tasknum)FEE_rd(ChargerMode_ID);
}

/***********************************************************************
  * @ 函数名  ： SysReset
  * @ 功能说明： 软件复位
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  **********************************************************************/
void SysReset(void)
{
    SCB->AIRCR = ((uint32_t)0x05FA << SCB_AIRCR_VECTKEY_Pos) | SCB_AIRCR_SYSRESETREQ_Msk;
    while (1);
}

/*************** EOF (not truncated)********************/
#ifdef  USE_FULL_ASSERT
 /**
   * @brief  Reports the name of the source file and the source line number
   *         where the assert_param error has occurred.
   * @param  file: pointer to the source file name
   * @param  line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
       /* USER CODE END 6 */
}
#endif
