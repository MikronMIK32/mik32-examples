#include "mik32_hal_rtc.h"
#include "mik32_hal_rcc.h"

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

            CurrentTime = HAL_RTC_GetTime(&hrtc);
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

        if (HAL_RTC_GetAlrmFlag(&hrtc))
        {
             
            xprintf("\nAlarm!\n");

            HAL_RTC_AlarmDisable(&hrtc);
            HAL_RTC_ClearAlrmFlag(&hrtc);
        }
    }
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInit = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    PeriphClkInit.PMClockAHB = PMCLOCKAHB_DEFAULT;    
    PeriphClkInit.PMClockAPB_M = PMCLOCKAPB_M_DEFAULT | PM_CLOCK_PAD_CONFIG_M | PM_CLOCK_PM_M | PM_CLOCK_WU_M | PM_CLOCK_RTC_M;     
    PeriphClkInit.PMClockAPB_P = PMCLOCKAPB_P_DEFAULT | PM_CLOCK_UART_0_M;     
    HAL_RCC_ClockConfig(&PeriphClkInit);


    RCC_OscInit.OscillatorEnable = RCC_OSCILLATORTYPE_OSC32K | RCC_OSCILLATORTYPE_OSC32M;
    RCC_OscInit.OscillatorSystem = RCC_OSCILLATORTYPE_OSC32M;                          
    RCC_OscInit.AHBDivider = 0;                             
    RCC_OscInit.APBMDivider = 0;                             
    RCC_OscInit.APBPDivider = 0;                             
    RCC_OscInit.HSI32MCalibrationValue = 128;               
    RCC_OscInit.LSI32KCalibrationValue = 0;
    RCC_OscInit.RTCClockSelection = RCC_RTCCLKSOURCE_OSC32K;
    RCC_OscInit.RTCClockCPUSelection = RCC_RTCCLKCPUSOURCE_OSC32K;
    HAL_RCC_OscConfig(&RCC_OscInit);


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
    sTime.Dow       = RTC_WEEKDAY_TUESDAY;
    sTime.Hours     = 20;
    sTime.Minutes   = 30;
    sTime.Seconds   = 0;
    HAL_RTC_SetTime(&hrtc, &sTime);

    sDate.Century   = 21;
    sDate.Day       = 19;
    sDate.Month     = RTC_MONTH_JULY;
    sDate.Year      = 22;
    HAL_RTC_SetDate(&hrtc, &sDate);

    /* Включение будильника. Настройка даты и времени будильника */
    sAlarm.AlarmTime.Dow       = RTC_WEEKDAY_TUESDAY;
    sAlarm.AlarmTime.Hours     = 20;
    sAlarm.AlarmTime.Minutes   = 30;
    sAlarm.AlarmTime.Seconds   = 5;

    sAlarm.AlarmDate.Century   = 21;
    sAlarm.AlarmDate.Day       = 19;
    sAlarm.AlarmDate.Month     = RTC_MONTH_JULY;
    sAlarm.AlarmDate.Year      = 22;

    sAlarm.MaskAlarmTime = RTC_TALRM_CDOW_M | RTC_TALRM_CH_M | RTC_TALRM_CM_M | RTC_TALRM_CS_M;
    sAlarm.MaskAlarmDate = RTC_DALRM_CC_M | RTC_DALRM_CD_M | RTC_DALRM_CM_M | RTC_DALRM_CY_M;

    HAL_RTC_SetAlarm(&hrtc, &sAlarm);

    HAL_RTC_Enable(&hrtc);


}
