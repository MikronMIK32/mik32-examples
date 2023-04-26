#include "mik32_hal_rtc.h"
#include "mik32_hal_rcc.h"
#include "mik32_hal_irq.h"

#include "uart_lib.h"
#include "xprintf.h"


RTC_HandleTypeDef hrtc;

RTC_TimeTypeDef LastTime = {0};
RTC_TimeTypeDef CurrentTime = {0};
RTC_DateTypeDef CurrentDate = {0};

void SystemClock_Config(void);
static void RTC_Init(void);

int main()
{
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
    RCC_OscInitTypeDef RCC_OscInit = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    RCC_OscInit.OscillatorEnable = RCC_OSCILLATORTYPE_OSC32K | RCC_OSCILLATORTYPE_OSC32M;   
    RCC_OscInit.OscillatorSystem = RCC_OSCILLATORTYPE_OSC32M;                          
    RCC_OscInit.AHBDivider = 0;                             
    RCC_OscInit.APBMDivider = 0;                             
    RCC_OscInit.APBPDivider = 0;                             
    RCC_OscInit.HSI32MCalibrationValue = 0;                  
    RCC_OscInit.LSI32KCalibrationValue = 0;
    HAL_RCC_OscConfig(&RCC_OscInit);

    PeriphClkInit.PMClockAHB = PMCLOCKAHB_DEFAULT;    
    PeriphClkInit.PMClockAPB_M = PMCLOCKAPB_M_DEFAULT | PM_CLOCK_WU_M | PM_CLOCK_RTC_M | PM_CLOCK_EPIC_M;     
    PeriphClkInit.PMClockAPB_P = PMCLOCKAPB_P_DEFAULT | PM_CLOCK_UART_0_M;     
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_OSC32K;
    PeriphClkInit.RTCClockCPUSelection = RCC_RTCCLKCPUSOURCE_NO_CLK;
    HAL_RCC_ClockConfig(&PeriphClkInit);
}

static void RTC_Init(void)
{
    
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    RTC_AlarmTypeDef sAlarm = {0};

    hrtc.Instance = RTC;

    /* Выключение RTC для записи даты и времени */
    HAL_RTC_Disable(&hrtc);

    /* Установка даты и времени RTC */
    sTime.Dow       = RTC_WEEKDAY_FRIDAY;
    sTime.Hours     = 23;
    sTime.Minutes   = 54;
    sTime.Seconds   = 0;
    HAL_RTC_SetTime(&hrtc, &sTime);

    sDate.Century   = 21;
    sDate.Day       = 9;
    sDate.Month     = RTC_MONTH_OCTOBER;
    sDate.Year      = 22;
    HAL_RTC_SetDate(&hrtc, &sDate);

    /* Включение будильника. Настройка даты и времени будильника */
    sAlarm.AlarmTime.Dow       = RTC_WEEKDAY_FRIDAY;
    sAlarm.AlarmTime.Hours     = 23;
    sAlarm.AlarmTime.Minutes   = 54;
    sAlarm.AlarmTime.Seconds   = 5;

    sAlarm.AlarmDate.Century   = 21;
    sAlarm.AlarmDate.Day       = 9;
    sAlarm.AlarmDate.Month     = RTC_MONTH_OCTOBER;
    sAlarm.AlarmDate.Year      = 22;

    sAlarm.MaskAlarmTime = RTC_TALRM_CDOW_M | RTC_TALRM_CH_M | RTC_TALRM_CM_M | RTC_TALRM_CS_M;
    sAlarm.MaskAlarmDate = RTC_DALRM_CC_M | RTC_DALRM_CD_M | RTC_DALRM_CM_M | RTC_DALRM_CY_M;

    HAL_RTC_SetAlarm(&hrtc, &sAlarm);

    /* Разрешение прерываний */
    hrtc.Interrupts.Alarm = RTC_ALARM_IRQ_ENABLE;
    HAL_RTC_InterruptInit(&hrtc);

    HAL_RTC_Enable(&hrtc);
}

void RTC_IRQHandler()
{
    xprintf("\nAlarm!\n");

    HAL_RTC_AlarmDisable(&hrtc);
    HAL_RTC_ClearAlrmFlag(&hrtc);
}

