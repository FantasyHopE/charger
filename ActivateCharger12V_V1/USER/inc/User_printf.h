#ifndef __USER_PRINTF_H
#define __USER_PRINTF_H


#include "User_include.h"

//UARTx
#define  DEBUG_UARTx                   CW_UART1
#define  DEBUG_UART_CLK                SYSCTRL_APB1_PERIPH_UART1
#define  DEBUG_UART_APBClkENx          SYSCTRL_APBPeriphClk_Enable1
#define  DEBUG_UART_BaudRate           115200
#define  DEBUG_UART_UclkFreq           12000000

//UARTx GPIO
#define  DEBUG_UART_GPIO_CLK           (SYSCTRL_AHB_PERIPH_GPIOB)
#define  DEBUG_UART_TX_GPIO_PORT       CW_GPIOB
#define  DEBUG_UART_TX_GPIO_PIN        GPIO_PIN_1
#define  DEBUG_UART_RX_GPIO_PORT       CW_GPIOB
#define  DEBUG_UART_RX_GPIO_PIN        GPIO_PIN_0

//GPIO AF
#define  DEBUG_UART_AFTX               PB01_AFx_UART1TXD()
#define  DEBUG_UART_AFRX               PB00_AFx_UART1RXD()


void GPIO_Config(void);
void RCC_Configuration(void);
void UART_Configuration(void);


 
#ifdef __GNUC__
    /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
    set to 'Yes') calls __io_putchar() */
    #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
    #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */




#endif 
