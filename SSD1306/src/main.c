#include "mik32_hal_rcc.h"
#include "mik32_hal_i2c.h"
#include "mik32_hal_rtc.h"
#include "mik32_hal_ssd1306.h"


#define SSD1306_128x32
//#define ssd1306_128x64

I2C_HandleTypeDef hi2c0;
RTC_HandleTypeDef hrtc;

RTC_TimeTypeDef LastTime = {0};
RTC_TimeTypeDef CurrentTime = {0};

void SystemClock_Config(void);
static void I2C0_Init(void);
static void RTC_Init(void);

int main()
{    

    SystemClock_Config();

    RTC_Init();
    I2C0_Init();

    /* Задержка для включения экрана */
    for (volatile int i = 0; i < 1000; i++); 
    
    /* Инициализация */
    HAL_SSD1306_Init(&hi2c0, BRIGHTNESS_FULL);

    /* Очистка */
    HAL_SSD1306_SetBorder(&hi2c0, START_COLUMN, END_COLUMN, START_PAGE, END_PAGE);
    HAL_SSD1306_CLR_SCR(&hi2c0);

    // Разделитель. Область 3
    HAL_SSD1306_SetBorder(&hi2c0, START_COLUMN_COLON, END_COLUMN_COLON, START_PAGE, END_PAGE);
    HAL_SSD1306_Write(&hi2c0, SYMBOL_COLON);

    LastTime = HAL_RTC_GetTime(&hrtc);

    while (1)
    {    
        CurrentTime = HAL_RTC_GetTime(&hrtc);

        if (CurrentTime.Seconds != LastTime.Seconds)
        {
            // Часы. Область 1
            HAL_SSD1306_SetBorder(&hi2c0, START_COLUMN_TH, END_COLUMN_TH, START_PAGE, END_PAGE);
            HAL_SSD1306_Write(&hi2c0, CurrentTime.Hours/10);

            // Часы. Область 2
            HAL_SSD1306_SetBorder(&hi2c0, START_COLUMN_H, END_COLUMN_H, START_PAGE, END_PAGE);
            HAL_SSD1306_Write(&hi2c0, CurrentTime.Hours%10);

            // Минуты. Область 4
            HAL_SSD1306_SetBorder(&hi2c0, START_COLUMN_TM, END_COLUMN_TM, START_PAGE, END_PAGE);
            HAL_SSD1306_Write(&hi2c0, CurrentTime.Minutes/10);


            // Минуты. Область 5
            HAL_SSD1306_SetBorder(&hi2c0, START_COLUMN_M, END_COLUMN_M, START_PAGE, END_PAGE);
            HAL_SSD1306_Write(&hi2c0, CurrentTime.Minutes%10);
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
    RCC_OscInit.RTCClockSelection = RCC_RTCCLKSOURCE_OSC32K;
    RCC_OscInit.RTCClockCPUSelection = RCC_RTCCLKCPUSOURCE_OSC32K;
    HAL_RCC_OscConfig(&RCC_OscInit);

    PeriphClkInit.PMClockAHB = PMCLOCKAHB_DEFAULT;    
    PeriphClkInit.PMClockAPB_M = PMCLOCKAPB_M_DEFAULT | PM_CLOCK_WU_M | PM_CLOCK_RTC_M | PM_CLOCK_PAD_CONFIG_M;      
    PeriphClkInit.PMClockAPB_P = PMCLOCKAPB_P_DEFAULT | PM_CLOCK_I2C_0_M;     
    HAL_RCC_ClockConfig(&PeriphClkInit);
}

static void I2C0_Init(void)
{
    /* Общие настройки */
    hi2c0.Instance = I2C_0;

    hi2c0.Init.Mode = HAL_I2C_MODE_MASTER;

    hi2c0.Init.DigitalFilter = I2C_DIGITALFILTER_OFF;
    hi2c0.Init.AnalogFilter = I2C_ANALOGFILTER_DISABLE;
    hi2c0.Init.AutoEnd = I2C_AUTOEND_ENABLE;

    /* Настройка частоты */
    hi2c0.Clock.PRESC = 5;
    hi2c0.Clock.SCLDEL = 10;
    hi2c0.Clock.SDADEL = 10;
    hi2c0.Clock.SCLH = 16;
    hi2c0.Clock.SCLL = 16;


    if (HAL_I2C_Init(&hi2c0) != HAL_OK)
    {
        xprintf("I2C_Init error\n");
    }

}

static void RTC_Init(void)
{
    
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};

    hrtc.Instance = RTC;

    /* Выключение RTC для записи даты и времени */
    HAL_RTC_Disable(&hrtc);

    /* Установка даты и времени RTC */
    sTime.Dow       = RTC_WEEKDAY_FRIDAY;
    sTime.Hours     = 12;
    sTime.Minutes   = 0;
    sTime.Seconds   = 0;
    HAL_RTC_SetTime(&hrtc, &sTime);

    sDate.Century   = 21;
    sDate.Day       = 22;
    sDate.Month     = RTC_MONTH_JUNE;
    sDate.Year      = 23;
    HAL_RTC_SetDate(&hrtc, &sDate);

    HAL_RTC_Enable(&hrtc);


}
