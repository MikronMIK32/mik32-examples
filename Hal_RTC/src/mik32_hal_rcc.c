#include "mik32_hal_rcc.h"


void HAL_RCC_OscConfig(RCC_OscInitTypeDef *RCC_OscInit)
{
    /* Источники 32МГц */
    if(RCC_OscInit->OscillatorEnable & RCC_OSCILLATORTYPE_HSI32M)
    {
        WU->CLOCKS_SYS &= ~(1 << WU_CLOCKS_SYS_HSI32M_PD_S); //Включить HSI32M
        WU->CLOCKS_SYS = WU_CLOCKS_SYS_ADJ_HSI32M(RCC_OscInit->HSI32MCalibrationValue); // Поправочный коэффициент HSI32M
    }
    else
    {
        WU->CLOCKS_SYS |= (1 << WU_CLOCKS_SYS_HSI32M_PD_S); //Выключить HSI32M
    }

    if(RCC_OscInit->OscillatorEnable & RCC_OSCILLATORTYPE_OSC32M)
    {
        WU->CLOCKS_SYS &= ~(1 << WU_CLOCKS_SYS_OSC32M_PD_S); // Включить OSC32M
    }
    else
    {
        WU->CLOCKS_SYS |= (1 << WU_CLOCKS_SYS_OSC32M_PD_S); // Выключить OSC32M
    }

    /* Источники 32кГц */
    if(RCC_OscInit->OscillatorEnable & RCC_OSCILLATORTYPE_LSI32K)
    {
        WU->CLOCKS_BU &= ~(1 << WU_CLOCKS_BU_LSI32K_PD_S); // Включить LSI32K
        WU->CLOCKS_BU = WU_CLOCKS_BU_ADJ_LSI32K(RCC_OscInit->LSI32KCalibrationValue); // Поправочный коэффициент LSI32K
    }
    else
    {
        WU->CLOCKS_BU |= (1 << WU_CLOCKS_BU_LSI32K_PD_S); // Выключить LSI32K
    }

    if(RCC_OscInit->OscillatorEnable & RCC_OSCILLATORTYPE_OSC32K)
    {
        WU->CLOCKS_BU &= ~(1 << WU_CLOCKS_BU_OSC32K_PD_S); // Включить OSC32K
    }
    else
    {
        WU->CLOCKS_BU |= (1 << WU_CLOCKS_BU_OSC32K_PD_S); // Выключить OSC32K
    }
    
    /* Делители частоты */
    PM->DIV_AHB = RCC_OscInit->AHBDivider;
    PM->DIV_APB_M = RCC_OscInit->APBMDivider;
    PM->DIV_APB_P = RCC_OscInit->APBPDivider;

    /* Настройка источника тактирования системы */
    switch (RCC_OscInit->OscillatorSystem)
    {
    case RCC_OSCILLATORTYPE_HSI32M:
        PM->AHB_CLK_MUX = PM_AHB_CLK_MUX_HSI32M_M;
        break;
    case RCC_OSCILLATORTYPE_OSC32M:
        PM->AHB_CLK_MUX = PM_AHB_CLK_MUX_OSC32M_M;
        break;
    case RCC_OSCILLATORTYPE_LSI32K:
        PM->AHB_CLK_MUX = PM_AHB_CLK_MUX_LSI32K_M;
        break;
    case RCC_OSCILLATORTYPE_OSC32K:
        PM->AHB_CLK_MUX = PM_AHB_CLK_MUX_OSC32K_M;
        break;
    }
    
}

void HAL_RCC_ClockConfig(RCC_PeriphCLKInitTypeDef *PeriphClkInit)
{
    /* Управление тактированием устройств на шине AHB */
    PM->CLK_AHB_CLEAR = ~PeriphClkInit->PMClockAHB; // Выключение тактирования необходимых блоков
    PM->CLK_AHB_SET = PeriphClkInit->PMClockAHB; // включение тактирования необходимых блоков

    /* Управление тактированием устройств на шине APB_M */
    PM->CLK_APB_M_CLEAR = ~PeriphClkInit->PMClockAPB_M; // Выключение тактирования необходимых блоков
    PM->CLK_APB_M_SET = PeriphClkInit->PMClockAPB_M; // включение тактирования необходимых блоков

    /* Управление тактированием устройств на шине APB_P */
    PM->CLK_APB_P_CLEAR = ~PeriphClkInit->PMClockAPB_P; // Выключение тактирования необходимых блоков
    PM->CLK_APB_P_SET = PeriphClkInit->PMClockAPB_P; // включение тактирования необходимых блоков

    /* Выбор источника тактирования RTC */
    switch (PeriphClkInit->RTCClockSelection)
    {
    case RCC_RTCCLKSOURCE_LSI32K:
        WU->CLOCKS_BU = WU_CLOCKS_BU_RTC_CLK_MUX_LSI32K_M;
        break;
    
    case RCC_RTCCLKSOURCE_OSC32K:
        WU->CLOCKS_BU = WU_CLOCKS_BU_RTC_CLK_MUX_OSC32K_M;
        break;
    }

    
    /* Выбор источника тактирования RTC в составе ядра*/
    switch (PeriphClkInit->RTCClockSelection)
    {
    case RCC_RTCCLKCPUSOURCE_LSI32K:
        PM->CPU_RTC_CLK_MUX = PM_CPU_RTC_CLK_MUX_LSI32K;
        break;
    
    case RCC_RTCCLKCPUSOURCE_OSC32K:
        PM->CPU_RTC_CLK_MUX = PM_CPU_RTC_CLK_MUX_OSC32K;
        break;
    }

}