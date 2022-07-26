#include "common.h"
#include "rtc.h"
#include "wakeup.h"
#include "power_manager.h"
// #include "epic.h"

void rtc_wait_flag()
{
    uint32_t retry_limit = 10000;
    for (uint32_t i = 0; i < retry_limit; i++)
    {
        if ((RTC->CTRL & RTC_CTRL_FLAG_M) == 0)
        {
            return;
        }
    }
    
    xprintf("Ожидание установки CTRL.FLAG в 0 превышено\n");
}

void rtc_disable()
{
    // Для записи даты нужно сбросить бит EN в регистре CTRL
    RTC->CTRL &= ~RTC_CTRL_EN_M;
    rtc_wait_flag();
}

void rtc_enable()
{
    // Установка бита EN включает модуль RTC
    RTC->CTRL |= RTC_CTRL_EN_M;
    rtc_wait_flag();
}

void rtc_init()
{
    //Включаем тактирование необходимых блоков и модуля выбора режима GPIO 
    PM->CLK_APB_P_SET |= PM_CLOCK_GPIO_0_M | PM_CLOCK_WDT_M;
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

/** Установка времени
 *
 * \param dow День недели: 1 - Пн; 2 - Вт; 3 - Ср; 4 - Чт; 5 - Пт; 6 - Сб; 7 - Вс
 * \param hour Часы
 * \param minute Минуты
 * \param second Секунды
 * 
 */
void rtc_set_time(uint8_t dow, uint8_t hour, uint8_t minute, uint8_t second)
{
    uint8_t DOW, TH, H, TM, M, TS, S;
    DOW = dow;
    TH = hour / 10;
    H = hour % 10;
    TM = minute / 10;
    M = minute % 10;
    TS = second / 10;
    S = second % 10; 

    uint32_t RTC_time = (DOW << RTC_TIME_DOW_S) | // День недели
                        (TH << RTC_TIME_TH_S)   | // Десятки часов
                        (H << RTC_TIME_H_S)     | // Еденицы часов
                        (TM << RTC_TIME_TM_S)   | // Десятки минут
                        (M << RTC_TIME_M_S)     | // Единицы минут 
                        (TS << RTC_TIME_TS_S)   | // Десятки секунд
                        (S << RTC_TIME_S_S)     | // Единицы секунд
                        (0 << RTC_TIME_TOS_S);    // Десятые секунды

    xprintf("Установка времени RTC\n");
    RTC->TIME = RTC_time;
    rtc_wait_flag();
}

/** Установка даты. Записывается в виде CC.YY.MM.DD
 * 
 * \param century Век
 * \param day Число
 * \param month Месяц (Ноябрь - 11)
 * \param year Год (2022 год - 22)
 * 
 */
void rtc_set_date( uint8_t century, uint8_t day, uint8_t month, uint8_t year)
{
    uint8_t TC, C, TY, Y, TM, M, TD, D;
    TC = century / 10;
    C = century % 10;
    TY = year / 10;
    Y = year % 10;
    TM = month / 10;
    M = month % 10;
    TD = day / 10;
    D = day % 10;

    uint32_t RTC_data = (TC << RTC_DATE_TC_S)  | // Десятки века
                        (C << RTC_DATE_C_S)   | // Единицы века
                        (TY << RTC_DATE_TY_S)  | // Десятки года
                        (Y << RTC_DATE_Y_S)   | // Единицы года
                        (TM << RTC_DATE_TM_S)  | // Десятки месяца
                        (M << RTC_DATE_M_S)   | // Единицы месяца
                        (TD << RTC_DATE_TD_S)  | // Десятки числа
                        (D << RTC_DATE_D_S);    // Единицы числа

    xprintf("Установка даты RTC\n");
    RTC->DATE = RTC_data;
    rtc_wait_flag();
}

/** Установка времени будильника
 *
 * \param dow День недели: 1 - Пн; 2 - Вт; 3 - Ср; 4 - Чт; 5 - Пт; 6 - Сб; 7 - Вс
 * \param hour Часы
 * \param minute Минуты
 * \param second Секунды
 * \param alarm_mask Маска сравнения. CDOW - день недели; CH - часы; CM - минуты; CS - секунды
 * 
 */
void rtc_alarm_set_time(uint8_t dow, uint8_t hour, uint8_t minute, uint8_t second, uint32_t alarm_mask)
{
    uint8_t DOW, TH, H, TM, M, TS, S;
    DOW = dow;
    TH = hour / 10;
    H = hour % 10;
    TM = minute / 10;
    M = minute % 10;
    TS = second / 10;
    S = second % 10; 

    uint32_t RTC_alarm_time = (DOW << RTC_TIME_DOW_S) | // День недели
                              (TH << RTC_TIME_TH_S)   | // Десятки часов
                              (H << RTC_TIME_H_S)     | // Еденицы часов
                              (TM << RTC_TIME_TM_S)   | // Десятки минут
                              (M << RTC_TIME_M_S)     | // Единицы минут 
                              (TS << RTC_TIME_TS_S)   | // Десятки секунд
                              (S << RTC_TIME_S_S)     | // Единицы секунд
                              (0 << RTC_TIME_TOS_S);    // Десятые секунды

    xprintf("Установка времени будильника\n");
    RTC->TALRM = RTC_alarm_time | alarm_mask;
    rtc_wait_flag();
}

/** Установка даты будильника. Дата в виде CC.YY.MM.DD
 * 
 * \param century Век
 * \param day Число
 * \param month Месяц (Ноябрь - 11)
 * \param year Год (2022 год - 22)
 * \param alarm_mask Маска сравнения. CC - век; CD - день; CM - месяц; CY - год
 * 
 */
void rtc_alarm_set_date( uint8_t century, uint8_t day, uint8_t month, uint8_t year, uint32_t alarm_mask) 
{
    uint8_t TC, C, TY, Y, TM, M, TD, D;
    TC = century / 10;
    C = century % 10;
    TY = year / 10;
    Y = year % 10;
    TM = month / 10;
    M = month % 10;
    TD = day / 10;
    D = day % 10;

    uint32_t RTC_alarm_data = (TC << RTC_DATE_TC_S)  | // Десятки века
                              (C << RTC_DATE_C_S)   | // Единицы века
                              (TY << RTC_DATE_TY_S)  | // Десятки года
                              (Y << RTC_DATE_Y_S)   | // Единицы года
                              (TM << RTC_DATE_TM_S)  | // Десятки месяца
                              (M << RTC_DATE_M_S)   | // Единицы месяца
                              (TD << RTC_DATE_TD_S)  | // Десятки числа
                              (D << RTC_DATE_D_S);    // Единицы числа

    xprintf("Установка даты будильника\n");
    RTC->DALRM = RTC_alarm_data | alarm_mask;
    rtc_wait_flag();
}

void rtc_check_date()
{
    uint8_t TC, C, TY, Y, TM, M, TD, D;
    uint32_t rtc_date_read = RTC->DATE;
    TC = (rtc_date_read & RTC_DATE_TC_M) >> RTC_DATE_TC_S;
    C = (rtc_date_read & RTC_DATE_C_M) >> RTC_DATE_C_S;
    TY = (rtc_date_read & RTC_DATE_TY_M) >> RTC_DATE_TY_S;
    Y = (rtc_date_read & RTC_DATE_Y_M) >> RTC_DATE_Y_S;
    TM = (rtc_date_read & RTC_DATE_TM_M) >> RTC_DATE_TM_S;
    M = (rtc_date_read & RTC_DATE_M_M) >> RTC_DATE_M_S;
    TD = (rtc_date_read & RTC_DATE_TD_M) >> RTC_DATE_TD_S;
    D = (rtc_date_read & RTC_DATE_D_M) >> RTC_DATE_D_S;

    xprintf("\n%d%d век\n", TC, C);
    xprintf("%d%d.%d%d.%d%d\n", TD, D, TM, M, TY, Y);
}

void rtc_check_time()
{
    switch (RTC->DOW)
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
    xprintf("%d%d:%d%d:%d%d.%d\n", RTC->TH, RTC->H, RTC->TM, RTC->M, RTC->TS, RTC->S, RTC->TOS);

}

void rtc_check()
{
    rtc_check_date();
    rtc_check_time();
}

int main()
{

    rtc_init();
    rtc_disable();
    
    uint8_t dow = 2;
    uint8_t hour = 20;
    uint8_t minute = 30;
    uint8_t second = 00;

    uint8_t century = 21;
    uint8_t day = 19;
    uint8_t month = 07;
    uint8_t year = 22;
    rtc_set_time(dow, hour, minute, second);
    rtc_set_date(century, day, month, year);

    uint32_t alarm_time_mask = RTC_TALRM_CDOW_M | RTC_TALRM_CH_M | RTC_TALRM_CM_M | RTC_TALRM_CS_M;
    uint32_t alarm_date_mask = RTC_DALRM_CC_M | RTC_DALRM_CD_M | RTC_DALRM_CM_M | RTC_DALRM_CY_M;
    rtc_alarm_set_time(dow, hour, minute + 1, second, alarm_time_mask);
    rtc_alarm_set_date(century, day, month, year, alarm_date_mask);

    xprintf("Включнеие RTC\n");
    rtc_enable();

    int counter = 1000000;

    while (1)
    {
        if (--counter < 0)
        {
            rtc_check();
            counter = 1000000;
        }

        if (RTC->CTRL & RTC_CTRL_ALRM_M)
        {
            for (volatile int i = 0; i < 1000000; i++); 
            xprintf("\nAlarm!\n");
            RTC->CTRL &= ~RTC_CTRL_ALRM_M;
            rtc_wait_flag();
        }
    }
}