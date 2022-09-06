#include "main.h"

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
            HAL_RTC_Check(&hrtc);
            counter = 1000000;
        }

        if (RTC->CTRL & RTC_CTRL_ALRM_M)
        {
            for (volatile int i = 0; i < 1000000; i++); 
            xprintf("\nAlarm!\n");
            hrtc.Instance->CTRL &= ~RTC_CTRL_ALRM_M;
            HAL_RTC_WaitFlag(&hrtc);
        }
    }
}

void SystemClock_Config(void)
{
    //Включаем тактирование необходимых блоков и модуля выбора режима GPIO 
    //PM->CLK_APB_P_SET |= PM_CLOCK_GPIO_0_M | PM_CLOCK_WDT_M;
    PM->CLK_APB_P_SET |= PM_CLOCK_WDT_M;
    PM->CLK_APB_M_SET |= PM_CLOCK_PAD_CONFIG_M | PM_CLOCK_WU_M | PM_CLOCK_PM_M | PM_CLOCK_RTC_M;
    for (volatile int i = 0; i < 10; i++);

    /*
    * CLOCKS_BU - Регистр управления тактированием батарейного домена
    * 
    * OCS32K_en - Включение/отключение внешнего осцилятора на 32 KГц
    * RC32K_en - Включение/отключение LSI32К
    * Adj_RC32К - Поправочные коэффициенты LSI32К
    * RTC_Clk_Mux - Выбор источника тактирования часов реального времени:
    *               0 – внутренний LSI32К;
    *               1 – внешний осциллятор OSC32K
    * OSC32K_sm - Режим повышенного потребления, активный уровень “0” для OSC32K
    */
    WU->CLOCKS_BU = WU_CLOCKS_BU_RTC_CLK_MUX_OSC32K_M;
    for (volatile int i = 0; i < 100; i++);
    WU->CLOCKS_BU = (0<<WU_CLOCKS_BU_OSC32K_PD_S) | WU_CLOCKS_BU_RTC_CLK_MUX_OSC32K_M;
    xprintf("Запуск с внешним осцилятором OSC32K\n");

    // Сброс RTC
    WU->RTC_CONRTOL = WU_RTC_RESET_CLEAR_M;

}

static void MX_RTC_Init(void)
{

    /* USER CODE BEGIN RTC_Init 0 */

    /* USER CODE END RTC_Init 0 */

    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    RTC_AlarmTypeDef sAlarm = {0};

    /* USER CODE BEGIN RTC_Init 1 */

    /* USER CODE END RTC_Init 1 */
    /** Initialize RTC Only
     */
    hrtc.Instance = RTC;

    /* USER CODE BEGIN Check_RTC_BKUP */

    /* USER CODE END Check_RTC_BKUP */

    /** Initialize RTC and set the Time and Date
     */
    sTime.Dow = RTC_WEEKDAY_MONDAY;
    sTime.Hours = 12;
    sTime.Minutes = 50;
    sTime.Seconds = 0;

    // Выключение RTC для записи даты и времени
    HAL_RTC_Disable(&hrtc);

    HAL_RTC_SetTime(&hrtc, &sTime);

    sDate.Century = 21;
    sDate.Day = 19;
    sDate.Month = RTC_MONTH_JULY;
    sDate.Year = 22;

    HAL_RTC_SetDate(&hrtc, &sDate);

    /* Enable the Alarm */
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

    HAL_RTC_Enable(&hrtc);

    /* USER CODE BEGIN RTC_Init 2 */

    /* USER CODE END RTC_Init 2 */

}