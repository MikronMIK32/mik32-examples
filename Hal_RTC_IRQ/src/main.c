#include "mik32_hal.h"
#include "mik32_hal_rtc.h"
#include "mik32_hal_irq.h"

#include "uart_lib.h"
#include "xprintf.h"

/*
 * В данном примере демонстрируется работа RTC с использованием прерывания.
 * В RTC настраивается текущее время и время срабатывания будильника, которое отличается от текущего на 5 секунд.
 * Будильник проводит сравнение времени и даты по всем полям.
 *
 * Текущее время RTC выводится по UART0. На 5й секунде должен сработать будильник и вызвать прерывание,
 * в обработчике которого выводится "Alarm!" по UART0.
 */

RTC_HandleTypeDef hrtc;

RTC_TimeTypeDef LastTime = {0};
RTC_TimeTypeDef CurrentTime = {0};
RTC_DateTypeDef CurrentDate = {0};

void SystemClock_Config(void);
static void RTC_Init(void);

int main()
{
    HAL_Init();

    SystemClock_Config();

    HAL_EPIC_MaskLevelSet(HAL_EPIC_RTC_MASK);
    HAL_IRQ_EnableInterrupts();

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    RTC_Init();

    LastTime = HAL_RTC_GetTime(&hrtc);
    while (1)
    {
        CurrentTime = HAL_RTC_GetTime(&hrtc);
        if (CurrentTime.Seconds != LastTime.Seconds)
        {
            LastTime = CurrentTime;
            CurrentDate = HAL_RTC_GetDate(&hrtc);
            xprintf("\n%d век\n", CurrentDate.Century);
            xprintf("%d.%d.%d\n", CurrentDate.Day, CurrentDate.Month, CurrentDate.Year);

            switch (CurrentTime.Dow)
            {
            case 1:
                xprintf("Понедельник\n");
                break;
            case 2:
                xprintf("Вторник\n");
                break;
            case 3:
                xprintf("Среда\n");
                break;
            case 4:
                xprintf("Четверг\n");
                break;
            case 5:
                xprintf("Пятница\n");
                break;
            case 6:
                xprintf("Суббота\n");
                break;
            case 7:
                xprintf("Воскресенье\n");
                break;
            }
            xprintf("%d:%d:%d.%d\n", CurrentTime.Hours, CurrentTime.Minutes, CurrentTime.Seconds, hrtc.Instance->TOS);
        }
    }
}

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
    PCC_OscInit.LSI32KCalibrationValue = 8;
    PCC_OscInit.RTCClockSelection = PCC_RTC_CLOCK_SOURCE_OSC32K;
    PCC_OscInit.RTCClockCPUSelection = PCC_CPU_RTC_CLOCK_SOURCE_OSC32K;
    HAL_PCC_Config(&PCC_OscInit);
}

static void RTC_Init(void)
{
    __HAL_PCC_RTC_CLK_ENABLE();

    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    RTC_AlarmTypeDef sAlarm = {0};

    hrtc.Instance = RTC;

    /* Выключение RTC для записи даты и времени */
    HAL_RTC_Disable(&hrtc);

    /* Установка даты и времени RTC */
    sTime.Dow = RTC_WEEKDAY_FRIDAY;
    sTime.Hours = 23;
    sTime.Minutes = 54;
    sTime.Seconds = 0;
    HAL_RTC_SetTime(&hrtc, &sTime);

    sDate.Century = 21;
    sDate.Day = 9;
    sDate.Month = RTC_MONTH_OCTOBER;
    sDate.Year = 22;
    HAL_RTC_SetDate(&hrtc, &sDate);

    /* Включение будильника. Настройка даты и времени будильника */
    sAlarm.AlarmTime.Dow = RTC_WEEKDAY_FRIDAY;
    sAlarm.AlarmTime.Hours = 23;
    sAlarm.AlarmTime.Minutes = 54;
    sAlarm.AlarmTime.Seconds = 5;

    sAlarm.AlarmDate.Century = 21;
    sAlarm.AlarmDate.Day = 9;
    sAlarm.AlarmDate.Month = RTC_MONTH_OCTOBER;
    sAlarm.AlarmDate.Year = 22;

    sAlarm.MaskAlarmTime = RTC_TALRM_CDOW_M | RTC_TALRM_CH_M | RTC_TALRM_CM_M | RTC_TALRM_CS_M;
    sAlarm.MaskAlarmDate = RTC_DALRM_CC_M | RTC_DALRM_CD_M | RTC_DALRM_CM_M | RTC_DALRM_CY_M;

    HAL_RTC_SetAlarm(&hrtc, &sAlarm);

    /* Разрешение прерываний */
    hrtc.Interrupts.Alarm = RTC_ALARM_IRQ_ENABLE;
    HAL_RTC_InterruptInit(&hrtc);

    HAL_RTC_Enable(&hrtc);
}

void trap_handler()
{
    if (EPIC_CHECK_RTC())
    {
        xprintf("\nAlarm!\n");
        HAL_RTC_AlarmDisable(&hrtc);
        HAL_RTC_ClearAlrmFlag(&hrtc);
    }

    /* Сброс прерываний */
    HAL_EPIC_Clear(0xFFFFFFFF);
}
