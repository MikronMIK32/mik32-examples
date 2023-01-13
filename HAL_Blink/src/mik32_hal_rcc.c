#include "mik32_hal_rcc.h"


void HAL_RCC_OscEnable(uint8_t ocillator)
{
    switch (ocillator)
    {
    case RCC_OSCILLATORTYPE_HSI32M:
        WU->CLOCKS_SYS &= ~(1 << WU_CLOCKS_SYS_HSI32M_PD_S); //Включить HSI32M
        break;
    case RCC_OSCILLATORTYPE_OSC32M:
        WU->CLOCKS_SYS &= ~(1 << WU_CLOCKS_SYS_OSC32M_PD_S); // Включить OSC32M
        break;
    case RCC_OSCILLATORTYPE_LSI32K:
        WU->CLOCKS_BU &= ~(1 << WU_CLOCKS_BU_LSI32K_PD_S); // Включить LSI32K
        break;
    case RCC_OSCILLATORTYPE_OSC32K:
        WU->CLOCKS_BU &= ~(1 << WU_CLOCKS_BU_OSC32K_PD_S); // Включить OSC32K
        break;
    }
}

void HAL_RCC_OscDisable(uint8_t ocillator)
{
    switch (ocillator)
    {
    case RCC_OSCILLATORTYPE_HSI32M:
        WU->CLOCKS_SYS |= (1 << WU_CLOCKS_SYS_HSI32M_PD_S); //Включить HSI32M
        break;
    case RCC_OSCILLATORTYPE_OSC32M:
        WU->CLOCKS_SYS |= (1 << WU_CLOCKS_SYS_OSC32M_PD_S); // Включить OSC32M
        break;
    case RCC_OSCILLATORTYPE_LSI32K:
        WU->CLOCKS_BU |= (1 << WU_CLOCKS_BU_LSI32K_PD_S); // Включить LSI32K
        break;
    case RCC_OSCILLATORTYPE_OSC32K:
        WU->CLOCKS_BU |= (1 << WU_CLOCKS_BU_OSC32K_PD_S); // Включить OSC32K
        break;
    }
}

void HAL_RCC_SetOscSystem(uint8_t ocillator_system)
{
    /* Настройка источника тактирования системы */
    switch (ocillator_system)
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

void HAL_RCC_DividerAHB(uint8_t divider_AHB)
{
    PM->DIV_AHB = divider_AHB;
}

void HAL_RCC_DividerAPB_M(uint8_t divider_APB_M)
{
    PM->DIV_APB_M = divider_APB_M;
}

void HAL_RCC_DividerAPB_P(uint8_t divider_APB_P)
{
    PM->DIV_APB_P = divider_APB_P;
}  

void HAL_RCC_OscConfig(RCC_OscInitTypeDef *RCC_OscInit)
{
    /* Источники 32МГц */
    /* Внутренний */
    if(RCC_OscInit->OscillatorEnable & RCC_OSCILLATORTYPE_HSI32M)
    {
        HAL_RCC_OscEnable(RCC_OSCILLATORTYPE_HSI32M); //Включить HSI32M
        WU->CLOCKS_SYS = WU_CLOCKS_SYS_ADJ_HSI32M(RCC_OscInit->HSI32MCalibrationValue); // Поправочный коэффициент HSI32M
    }
    else
    {
        HAL_RCC_OscDisable(RCC_OSCILLATORTYPE_HSI32M);  //Выключить HSI32M
    }

    /* Внешний */
    if(RCC_OscInit->OscillatorEnable & RCC_OSCILLATORTYPE_OSC32M)
    {
        HAL_RCC_OscEnable(RCC_OSCILLATORTYPE_OSC32M);   // Включить OSC32M
    }
    else
    {
        HAL_RCC_OscDisable(RCC_OSCILLATORTYPE_OSC32M);   // Выключить OSC32M
    }

    /* Источники 32кГц */
    /* Внутренний  */
    if(RCC_OscInit->OscillatorEnable & RCC_OSCILLATORTYPE_LSI32K)
    {
        HAL_RCC_OscEnable(RCC_OSCILLATORTYPE_LSI32K);   // Включить LSI32K
        WU->CLOCKS_BU = WU_CLOCKS_BU_ADJ_LSI32K(RCC_OscInit->LSI32KCalibrationValue); // Поправочный коэффициент LSI32K
    }
    else
    {
        HAL_RCC_OscDisable(RCC_OSCILLATORTYPE_LSI32K); // Выключить LSI32K
    }

    /* Внешний */
    if(RCC_OscInit->OscillatorEnable & RCC_OSCILLATORTYPE_OSC32K)
    {
        HAL_RCC_OscEnable(RCC_OSCILLATORTYPE_OSC32K); // Включить OSC32K
    }
    else
    {
        HAL_RCC_OscDisable(RCC_OSCILLATORTYPE_OSC32K); // Выключить OSC32K
    }
    
    /* Делители частоты */
    HAL_RCC_DividerAHB(RCC_OscInit->AHBDivider);
    HAL_RCC_DividerAPB_M(RCC_OscInit->APBMDivider);
    HAL_RCC_DividerAPB_P(RCC_OscInit->APBPDivider);

    /* Настройка источника тактирования системы */
    HAL_RCC_SetOscSystem(RCC_OscInit->OscillatorSystem);
    
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

void HAL_RCC_ClockEnable(uint32_t periphery)
{
    uint32_t mask = 0b11 << 30;
    switch (periphery & mask)
    {
    case HAL_CLOCK_AHB_MASK:
        periphery &= ~mask;
        PM->CLK_AHB_SET |= periphery; // включение тактирования необходимых блоков
        break;
    case HAL_CLOCK_APB_M_MASK:
        periphery &= ~mask;
        PM->CLK_APB_M_SET |= periphery; // включение тактирования необходимых блоков
        break;
    case HAL_CLOCK_APB_P_MASK:
        periphery &= ~mask;
        PM->CLK_APB_P_SET |= periphery; // включение тактирования необходимых блоков
        break;
    }
}

void HAL_RCC_ClockDisable(uint32_t periphery)
{
    uint32_t mask = 0b11 << 30;
    switch (periphery & mask)
    {
    case HAL_CLOCK_AHB_MASK:
        periphery &= ~mask;
        PM->CLK_AHB_CLEAR |= periphery; // включение тактирования необходимых блоков
        break;
    case HAL_CLOCK_APB_M_MASK:
        periphery &= ~mask;
        PM->CLK_APB_M_CLEAR |= periphery; // включение тактирования необходимых блоков
        break;
    case HAL_CLOCK_APB_P_MASK:
        periphery &= ~mask;
        PM->CLK_APB_P_CLEAR |= periphery; // включение тактирования необходимых блоков
        break;
    }
}