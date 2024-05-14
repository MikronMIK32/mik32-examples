#include "mik32_hal.h"

#include "mik32_hal_timer16.h"
#include "mik32_hal_timer32.h"
#include "mik32_hal_scr1_timer.h"

/*
Пример демонстрирует возможности работы с временем в библиотеке HAL.
Функции системного времени:
- uint32_t HAL_Micros() - возвращает время в микросекундах
- uint32_t HAL_Millis() - возвращает время в миллисекундах
Функции временных задержек:
- void HAL_DelayUs(uint32_t time_us) - задержка в микросекундах
- void HAL_DelayMs(uint32_t time_ms) - задержка в миллисекундах

По умолчанию временные задержки реализованы в виде цикла с изменяемым количеством проходов,
а функции системного времени возвращают 0. Однако можно применить один из 16- или 32-р
таймеров, или таймер SCR1 для организации подсчета системного времени. В этом случае необхлдимо
переопределить функции системных часов, а также проинициализировать нужный таймер в теле программы.
*/

/* Тип системных часов */
//#define SYSTEM_TIME_TIMER16
//#define SYSTEM_TIME_TIMER32
//#define SYSTEM_TIME_SCR1_TIMER


/* Тип платы */
#define BOARD_LITE
//#define BOARD_DIP


/* Переопределения функций системных часов */
#if defined SYSTEM_TIME_TIMER16
uint32_t HAL_Micros()
{
    return HAL_Time_TIM16_Micros();
}
uint32_t HAL_Millis()
{
    return HAL_Time_TIM16_Millis();
}
void HAL_DelayUs(uint32_t time_us)
{
    HAL_Time_TIM16_DelayUs(time_us);
}
void HAL_DelayMs(uint32_t time_ms)
{
    HAL_Time_TIM16_DelayMs(time_ms);
}
#endif

#if defined SYSTEM_TIME_TIMER32
uint32_t HAL_Micros()
{
    return HAL_Time_TIM32_Micros();
}
uint32_t HAL_Millis()
{
    return HAL_Time_TIM32_Millis();
}
void HAL_DelayUs(uint32_t time_us)
{
    HAL_Time_TIM32_DelayUs(time_us);
}
void HAL_DelayMs(uint32_t time_ms)
{
    HAL_Time_TIM32_DelayMs(time_ms);
}
#endif

#if defined SYSTEM_TIME_SRC1_TIMER
uint32_t HAL_Micros()
{
    return HAL_Time_SCR1TIM_Micros();
}
uint32_t HAL_Millis()
{
    return HAL_Time_SCR1TIM_Millis();
}
void HAL_DelayUs(uint32_t time_us)
{
    HAL_Time_SCR1TIM_DelayUs(time_us);
}
void HAL_DelayMs(uint32_t time_ms)
{
    HAL_Time_SCR1TIM_DelayMs(time_ms);
}
#endif


void SystemClock_Config(void);
void GPIO_Init();



int main()
{    
    HAL_Init();
    
    SystemClock_Config();

    GPIO_Init();

#if defined SYSTEM_TIME_TIMER16
    HAL_Time_TIM16_Init(TIMER16_0);
#endif
#if defined SYSTEM_TIME_TIMER32
    HAL_Time_TIM32_Init(TIMER32_0);
#endif
#if defined SYSTEM_TIMER_SCR1_TIMER
    HAL_Time_SCR1TIM_Init();
#endif

    while (1)
    {
#if defined BOARD_LITE
        HAL_GPIO_TogglePin(GPIO_2, GPIO_PIN_7);
#endif
#if defined BOARD_DIP
        HAL_GPIO_TogglePin(GPIO_0, GPIO_PIN_3);
        HAL_GPIO_TogglePin(GPIO_1, GPIO_PIN_3);
#endif
        /* Задержка */
        HAL_DelayMs(500);
    }
       
}

/* System timer16 uses interrupts */
#if defined SYSTEM_TIME_TIMER16
void trap_handler()
{
    HAL_Time_TIM16_InterruptHandler();
}
#endif

void SystemClock_Config(void)
{
    PCC_InitTypeDef PCC_OscInit = {0};

    PCC_OscInit.OscillatorEnable = PCC_OSCILLATORTYPE_ALL;
    PCC_OscInit.FreqMon.OscillatorSystem = PCC_OSCILLATORTYPE_OSC32M;
    PCC_OscInit.FreqMon.ForceOscSys = PCC_FORCE_OSC_SYS_UNFIXED;
    PCC_OscInit.FreqMon.Force32KClk = PCC_FREQ_MONITOR_SOURCE_OSC32K;
    PCC_OscInit.AHBDivider = 0;
    PCC_OscInit.APBMDivider = 0;
    PCC_OscInit.APBPDivider = 0;
    PCC_OscInit.HSI32MCalibrationValue = 128;
    PCC_OscInit.LSI32KCalibrationValue = 128;
    PCC_OscInit.RTCClockSelection = PCC_RTC_CLOCK_SOURCE_AUTO;
    PCC_OscInit.RTCClockCPUSelection = PCC_CPU_RTC_CLOCK_SOURCE_OSC32K;
    HAL_PCC_Config(&PCC_OscInit);
}


void GPIO_Init()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_PCC_GPIO_0_CLK_ENABLE();
    __HAL_PCC_GPIO_1_CLK_ENABLE();
    __HAL_PCC_GPIO_2_CLK_ENABLE();

#ifdef BOARD_LITE
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    HAL_GPIO_Init(GPIO_2, &GPIO_InitStruct);
#endif

#ifdef BOARD_DIP
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    HAL_GPIO_Init(GPIO_1, &GPIO_InitStruct);
#endif
}
