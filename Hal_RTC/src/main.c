#include "main.h"
#include "common.h"

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_RTC_Init(void);

int main()
{
    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_RTC_Init();

    int counter = 1000000;

    while (1)
    {
        if (--counter < 0)
        {
            #ifdef MIK32_RTC_DEBUG
            HAL_RTC_Check(&hrtc);
            #endif
            
            counter = 1000000;
        }

        // if (hrtc.Instance->CTRL & RTC_CTRL_ALRM_M)
        // {
        //     for (volatile int i = 0; i < 1000000; i++); 

        //     #ifdef MIK32_RTC_DEBUG
        //     xprintf("\nAlarm!\n");
        //     #endif
            
        //     hrtc.Instance->CTRL &= ~RTC_CTRL_ALRM_M;
        //     HAL_RTC_WaitFlag(&hrtc);
        // }
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

static void MX_RTC_Init(void)
{
    
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    RTC_AlarmTypeDef sAlarm = {0};

    hrtc.Instance = RTC;

    /* Установка даты и времени RTC */
    sTime.Dow = RTC_WEEKDAY_FRIDAY;
    sTime.Hours = 23;
    sTime.Minutes = 54;
    sTime.Seconds = 0;

    /* Выключение RTC для записи даты и времени */
    HAL_RTC_Disable(&hrtc);

    HAL_RTC_SetTime(&hrtc, &sTime);

    sDate.Century = 21;
    sDate.Day = 9;
    sDate.Month = RTC_MONTH_OCTOBER;
    sDate.Year = 22;

    HAL_RTC_SetDate(&hrtc, &sDate);

    /* Включение будильника. Настройка даты и времени будильника */
    sAlarm.AlarmTime.Dow = sTime.Dow;
    sAlarm.AlarmTime.Hours = sTime.Hours;
    sAlarm.AlarmTime.Minutes = sTime.Minutes;
    sAlarm.AlarmTime.Seconds = sTime.Seconds + 5;

    sAlarm.AlarmDate.Century = 0;
    sAlarm.AlarmDate.Day = 0;
    sAlarm.AlarmDate.Month = 0;
    sAlarm.AlarmDate.Year = 0;
    sAlarm.AlarmDate = sDate;

    sAlarm.MaskAlarmTime = RTC_TALRM_CDOW_M | RTC_TALRM_CH_M | RTC_TALRM_CM_M | RTC_TALRM_CS_M;
    sAlarm.MaskAlarmDate = RTC_DALRM_CC_M | RTC_DALRM_CD_M | RTC_DALRM_CM_M | RTC_DALRM_CY_M;

    HAL_RTC_SetAlarm(&hrtc, &sAlarm);

    /* Настройка прерываний RTC */
    hrtc.Interrupts.Alarm = RTC_ALARM_IRQn_ENABLE;
    HAL_RTC_IRQnEnable(&hrtc);

    HAL_RTC_Enable(&hrtc);


}

void trap_handler() 
{
    HAL_IRQ_DisableInterrupts();

    uint32_t epic_mask = (1 << EPIC_RTC_INDEX);

    if ((EPIC->RAW_STATUS & epic_mask) == 0)
    {
        xprintf("Флаг прерывнаия в EPIC равный 0 не ожидалось\n");
    } 
    
    //HAL_RTC_Disable(&hrtc);
    HAL_RTC_AlarmDisable(&hrtc);
    //HAL_RTC_Enable(&hrtc);

    HAL_RTC_AlrmClear(&hrtc);

    if ((EPIC->RAW_STATUS & epic_mask) != 0)
    {
        xprintf("Флаг прерывнаия в EPIC не был сброшен\n");
    } 

    xprintf("\nAlarm!\n");
    HAL_IRQ_EnableInterrupts();
}