#include "main.h"

I2C_HandleTypeDef hi2c0;
RTC_HandleTypeDef hrtc;

void SystemClock_Config(void);
static void I2C0_Init(void);
static void RTC_Init(void);

int main()
{    

    SystemClock_Config();
    
    I2C0_Init();

    RTC_Init();

    //xprintf("\nИнициализация\n");
    HAL_SSD1306_Init(&hi2c0, BRIGHTNESS_FULL);
    for (volatile int i = 0; i < 100000; i++);

    //xprintf("\nОчистка\n");
    HAL_SSD1306_CLR_SCR(&hi2c0);
    for (volatile int i = 0; i < 100000; i++);

    // Разделитель 3
    HAL_SSD1306_SetBorder(&hi2c0, START_COLUMN_COLON, END_COLUMN_COLON, START_PAGE, END_PAGE);
    HAL_SSD1306_Write(&hi2c0, SYMBOL_COLON);

    // uint8_t TH = hrtc.Instance->TH;
    // uint8_t H = hrtc.Instance->H;
    // uint8_t TM = hrtc.Instance->TM;
    // uint8_t M = hrtc.Instance->M;

    while (1)
    {    

            // часы 1
            HAL_SSD1306_SetBorder(&hi2c0, START_COLUMN_TH, END_COLUMN_TH, START_PAGE, END_PAGE);
            HAL_SSD1306_Write(&hi2c0, (uint8_t)hrtc.Instance->TH);

            // часы 2
            HAL_SSD1306_SetBorder(&hi2c0, START_COLUMN_H, END_COLUMN_H, START_PAGE, END_PAGE);
            HAL_SSD1306_Write(&hi2c0, (uint8_t)hrtc.Instance->H);

            // минуты 4
            HAL_SSD1306_SetBorder(&hi2c0, START_COLUMN_TM, END_COLUMN_TM, START_PAGE, END_PAGE);
            HAL_SSD1306_Write(&hi2c0, (uint8_t)hrtc.Instance->TM);

            // минуты 5
            HAL_SSD1306_SetBorder(&hi2c0, START_COLUMN_M, END_COLUMN_M, START_PAGE, END_PAGE);
            HAL_SSD1306_Write(&hi2c0, (uint8_t)hrtc.Instance->M);

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
    PeriphClkInit.PMClockAPB_M = PMCLOCKAPB_M_DEFAULT | PM_CLOCK_WU_M | PM_CLOCK_RTC_M;      
    PeriphClkInit.PMClockAPB_P = PMCLOCKAPB_P_DEFAULT | PM_CLOCK_UART_0_M | PM_CLOCK_I2C_0_M;     
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_OSC32K;
    PeriphClkInit.RTCClockCPUSelection = RCC_RTCCLKCPUSOURCE_NO_CLK;
    HAL_RCC_ClockConfig(&PeriphClkInit);
}

static void I2C0_Init(void)
{
    //hi2c0.Init.ClockSpeed = 165;
    
    /*Общие настройки*/
    hi2c0.Instance = I2C_0;
    hi2c0.Mode = HAL_I2C_MODE_MASTER;
    hi2c0.ShiftAddress = SHIFT_ADDRESS_DISABLE;
    hi2c0.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c0.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE; // При ENABLE в режиме ведущего значение AddressingMode не влияет
    hi2c0.Init.Filter = I2C_FILTER_OFF;

    /*Настройка частоты*/
    hi2c0.Clock.PRESC = 5;
    hi2c0.Clock.SCLDEL = 10;
    hi2c0.Clock.SDADEL = 10;
    hi2c0.Clock.SCLH = 16;
    hi2c0.Clock.SCLL = 16;
    //hi2c0.Init.ClockSpeed = 175;

    /*Настройки ведомого*/
    hi2c0.Init.OwnAddress1 = 0;
    hi2c0.Init.OwnAddress2 = 0;
    hi2c0.Init.OwnAddress2Mask = I2C_OWNADDRESS2_MASK_DISABLE;
    hi2c0.Init.SBCMode = I2C_SBC_DISABLE;
    hi2c0.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    /*Нстройки ведущего*/
    hi2c0.Init.AutoEnd = AUTOEND_DISABLE;

    HAL_I2C_Init(&hi2c0);

}

static void RTC_Init(void)
{
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};

    hrtc.Instance = RTC;

    sTime.Dow = RTC_WEEKDAY_MONDAY;
    sTime.Hours = 20;
    sTime.Minutes = 26;
    sTime.Seconds = 0;

    // Выключение RTC для записи даты и времени
    HAL_RTC_Disable(&hrtc);

    HAL_RTC_SetTime(&hrtc, &sTime);

    sDate.Century = 21;
    sDate.Day = 26;
    sDate.Month = RTC_MONTH_DECEMBER;
    sDate.Year = 22;

    HAL_RTC_SetDate(&hrtc, &sDate);

    HAL_RTC_Enable(&hrtc);

}
