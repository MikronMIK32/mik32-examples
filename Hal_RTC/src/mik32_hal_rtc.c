#include "mik32_hal_rtc.h"

void HAL_RTC_WaitFlag(RTC_HandleTypeDef *hrtc)
{
    uint32_t retry_limit = 10000;
    for (uint32_t i = 0; i < retry_limit; i++)
    {
        if ((hrtc->Instance->CTRL & RTC_CTRL_FLAG_M) == 0)
        {
            return;
        }
    }
    
    #ifdef MIK32_RTC_DEBUG
    xprintf("Ожидание установки CTRL.FLAG в 0 превышено\n");
    #endif
}

void HAL_RTC_Disable(RTC_HandleTypeDef *hrtc)
{
    // Для записи даты нужно сбросить бит EN в регистре CTRL
    RTC->CTRL &= ~RTC_CTRL_EN_M;
    HAL_RTC_WaitFlag(hrtc);
}

void HAL_RTC_Enable(RTC_HandleTypeDef *hrtc)
{
    // Установка бита EN включает модуль RTC
    RTC->CTRL |= RTC_CTRL_EN_M;
    HAL_RTC_WaitFlag(hrtc);
}


/** Установка времени
 *
 * \param dow День недели: 1 - Пн; 2 - Вт; 3 - Ср; 4 - Чт; 5 - Пт; 6 - Сб; 7 - Вс
 * \param hour Часы
 * \param minute Минуты
 * \param second Секунды
 * 
 */
void HAL_RTC_SetTime(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *sTime)
{
    uint8_t DOW, TH, H, TM, M, TS, S;
    DOW = sTime->Dow;
    TH = sTime->Hours / 10;
    H = sTime->Hours % 10;
    TM = sTime->Minutes / 10;
    M = sTime->Minutes % 10;
    TS = sTime->Seconds / 10;
    S = sTime->Seconds % 10; 

    uint32_t RTC_time = (DOW << RTC_TIME_DOW_S) | // День недели
                        (TH << RTC_TIME_TH_S)   | // Десятки часов
                        (H << RTC_TIME_H_S)     | // Еденицы часов
                        (TM << RTC_TIME_TM_S)   | // Десятки минут
                        (M << RTC_TIME_M_S)     | // Единицы минут 
                        (TS << RTC_TIME_TS_S)   | // Десятки секунд
                        (S << RTC_TIME_S_S)     | // Единицы секунд
                        (0 << RTC_TIME_TOS_S);    // Десятые секунды
    
    #ifdef MIK32_RTC_DEBUG
    xprintf("Установка времени RTC\n");
    #endif

    hrtc->Instance->TIME = RTC_time;
    HAL_RTC_WaitFlag(hrtc);
}

/** Установка даты. Записывается в виде CC.YY.MM.DD
 * 
 * \param century Век
 * \param day Число
 * \param month Месяц (Ноябрь - 11)
 * \param year Год (2022 год - 22)
 * 
 */
void HAL_RTC_SetDate(RTC_HandleTypeDef *hrtc, RTC_DateTypeDef *sDate)
{
    uint8_t TC, C, TY, Y, TM, M, TD, D;
    TC = sDate->Century / 10;
    C = sDate->Century % 10;
    TY = sDate->Year / 10;
    Y = sDate->Year % 10;
    TM = sDate->Month / 10;
    M = sDate->Month % 10;
    TD = sDate->Day / 10;
    D = sDate->Day % 10;

    uint32_t RTC_data = (TC << RTC_DATE_TC_S)  | // Десятки века
                        (C << RTC_DATE_C_S)   | // Единицы века
                        (TY << RTC_DATE_TY_S)  | // Десятки года
                        (Y << RTC_DATE_Y_S)   | // Единицы года
                        (TM << RTC_DATE_TM_S)  | // Десятки месяца
                        (M << RTC_DATE_M_S)   | // Единицы месяца
                        (TD << RTC_DATE_TD_S)  | // Десятки числа
                        (D << RTC_DATE_D_S);    // Единицы числа

    #ifdef MIK32_RTC_DEBUG
    xprintf("Установка даты RTC\n");
    #endif

    hrtc->Instance->DATE = RTC_data;
    HAL_RTC_WaitFlag(hrtc);
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
void HAL_RTC_Alarm_SetTime(RTC_HandleTypeDef *hrtc, RTC_AlarmTypeDef *sAlarm)
{
    uint8_t DOW, TH, H, TM, M, TS, S;
    DOW = sAlarm->AlarmTime.Dow;
    TH = sAlarm->AlarmTime.Hours / 10;
    H = sAlarm->AlarmTime.Hours % 10;
    TM = sAlarm->AlarmTime.Minutes / 10;
    M = sAlarm->AlarmTime.Minutes % 10;
    TS = sAlarm->AlarmTime.Seconds / 10;
    S = sAlarm->AlarmTime.Seconds % 10; 

    uint32_t RTC_alarm_time = (DOW << RTC_TIME_DOW_S) | // День недели
                              (TH << RTC_TIME_TH_S)   | // Десятки часов
                              (H << RTC_TIME_H_S)     | // Еденицы часов
                              (TM << RTC_TIME_TM_S)   | // Десятки минут
                              (M << RTC_TIME_M_S)     | // Единицы минут 
                              (TS << RTC_TIME_TS_S)   | // Десятки секунд
                              (S << RTC_TIME_S_S)     | // Единицы секунд
                              (0 << RTC_TIME_TOS_S);    // Десятые секунды

    #ifdef MIK32_RTC_DEBUG
    xprintf("Установка времени будильника\n");
    #endif

    hrtc->Instance->TALRM = RTC_alarm_time | sAlarm->MaskAlarmTime;
    HAL_RTC_WaitFlag(hrtc);
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
void HAL_RTC_Alarm_SetDate(RTC_HandleTypeDef *hrtc, RTC_AlarmTypeDef *sAlarm) 
{
    uint8_t TC, C, TY, Y, TM, M, TD, D;
    TC = sAlarm->AlarmDate.Century / 10;
    C = sAlarm->AlarmDate.Century % 10;
    TY = sAlarm->AlarmDate.Year / 10;
    Y = sAlarm->AlarmDate.Year % 10;
    TM = sAlarm->AlarmDate.Month / 10;
    M = sAlarm->AlarmDate.Month % 10;
    TD = sAlarm->AlarmDate.Day / 10;
    D = sAlarm->AlarmDate.Day % 10;

    uint32_t RTC_alarm_data = (TC << RTC_DATE_TC_S)  | // Десятки века
                              (C << RTC_DATE_C_S)   | // Единицы века
                              (TY << RTC_DATE_TY_S)  | // Десятки года
                              (Y << RTC_DATE_Y_S)   | // Единицы года
                              (TM << RTC_DATE_TM_S)  | // Десятки месяца
                              (M << RTC_DATE_M_S)   | // Единицы месяца
                              (TD << RTC_DATE_TD_S)  | // Десятки числа
                              (D << RTC_DATE_D_S);    // Единицы числа

    #ifdef MIK32_RTC_DEBUG
    xprintf("Установка даты будильника\n");
    #endif

    hrtc->Instance->DALRM = RTC_alarm_data | sAlarm->MaskAlarmDate;
    HAL_RTC_WaitFlag(hrtc);
}

void HAL_RTC_SetAlarm(RTC_HandleTypeDef *hrtc, RTC_AlarmTypeDef *sAlarm)
{
    HAL_RTC_Alarm_SetTime(hrtc, sAlarm);
    HAL_RTC_Alarm_SetDate(hrtc, sAlarm);
}

#ifdef MIK32_RTC_DEBUG

void HAL_RTC_CheckDate(RTC_HandleTypeDef *hrtc)
{
    uint8_t TC, C, TY, Y, TM, M, TD, D;
    uint32_t rtc_date_read = hrtc->Instance->DATE;
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

void HAL_RTC_CheckTime(RTC_HandleTypeDef *hrtc)
{
    switch (hrtc->Instance->DOW)
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
    xprintf("%d%d:%d%d:%d%d.%d\n", hrtc->Instance->TH, hrtc->Instance->H, hrtc->Instance->TM, 
                                    hrtc->Instance->M, hrtc->Instance->TS, hrtc->Instance->S, hrtc->Instance->TOS);

}

void HAL_RTC_Check(RTC_HandleTypeDef *hrtc)
{
    HAL_RTC_CheckDate(hrtc);
    HAL_RTC_CheckTime(hrtc);
}

#endif