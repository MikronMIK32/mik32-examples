#include "mik32_hal_pcc.h"
#include "wdt.h"
#include "mcu32_memory_map.h"

#include "uart_lib.h"
#include "xprintf.h"



void SystemClock_Config(void);


int main()
{    

    SystemClock_Config();

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    /* Тактирование WDT */
    __HAL_PCC_WDT_CLK_ENABLE(); 

    WDT->KEY = WDT_KEY_UNLOCK;
    WDT->KEY = WDT_KEY_STOP;

    WDT->KEY = WDT_KEY_UNLOCK;
    WDT->CON = WDT_CON_PRESCALE_4096_M | WDT_CON_PRELOAD(0x000); /* Делитель на 4096 */

    WDT->KEY = WDT_KEY_UNLOCK;
    WDT->KEY = WDT_KEY_START;

    xprintf("Start\n");

    while (1)
    {    
        for (volatile int i = 0; i < 1000000; i++);
    }
       
}

void SystemClock_Config(void)
{
    PCC_OscInitTypeDef PCC_OscInit = {0};

    PCC_OscInit.OscillatorEnable = PCC_OSCILLATORTYPE_ALL;   
    PCC_OscInit.OscillatorSystem = PCC_OSCILLATORTYPE_OSC32M;                          
    PCC_OscInit.AHBDivider = 0;                             
    PCC_OscInit.APBMDivider = 0;                             
    PCC_OscInit.APBPDivider = 0;                             
    PCC_OscInit.HSI32MCalibrationValue = 0;                  
    PCC_OscInit.LSI32KCalibrationValue = 0;
    PCC_OscInit.RTCClockSelection = PCC_RTCCLKSOURCE_NO_CLK;
    PCC_OscInit.RTCClockCPUSelection = PCC_RTCCLKCPUSOURCE_NO_CLK;
    HAL_PCC_OscConfig(&PCC_OscInit);
}
