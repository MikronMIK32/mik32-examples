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

void HAL_RTC_SetRegValue(RTC_HandleTypeDef *hrtc, uint32_t RegIndex, uint32_t Value)
{
    hrtc->Instance->REG[RegIndex] = Value;
}

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

void HAL_RTC_AlarmDisable(RTC_HandleTypeDef *hrtc)
{
    hrtc->Instance->TALRM &= ~(RTC_TALRM_CS_M | RTC_TALRM_CM_M | RTC_TALRM_CH_M | RTC_TALRM_CDOW_M);
    HAL_RTC_WaitFlag(hrtc);
    
    hrtc->Instance->DALRM &= ~(RTC_DALRM_CD_M | RTC_DALRM_CM_M | RTC_DALRM_CY_M | RTC_DALRM_CC_M);
    HAL_RTC_WaitFlag(hrtc);
}

void HAL_RTC_AlrmClear(RTC_HandleTypeDef *hrtc)
{
    /* Сброс флага ALRM в RTC */
    hrtc->Instance->CTRL &= ~RTC_CTRL_ALRM_M; 
	HAL_RTC_WaitFlag(hrtc);
}

uint8_t HAL_RTC_GetFlagALRM(RTC_HandleTypeDef *hrtc)
{
    if(hrtc->Instance->CTRL & RTC_CTRL_ALRM_M)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

#ifdef MIK32_RTC_IRQn
void HAL_RTC_IRQnEnable(RTC_HandleTypeDef *hrtc)
{

    if(hrtc->Interrupts.Alarm == RTC_ALARM_IRQn_ENABLE)
    {
        HAL_EPIC_MaskLevelSet(EPIC_RTC_INDEX); // Прерывание по уровню
        xprintf("Прерывание по уровню\n");
        hrtc->Instance->CTRL |= RTC_CTRL_INTE_M; // Разрешение прерывания в RTC
        HAL_RTC_WaitFlag(hrtc);
        xprintf("Разрешить прерывание по Alrm\n");
    }
    
    /* Включение глобальных прерываний */
    HAL_IRQ_EnableInterrupts(); 
    xprintf("Глобальные прерывания включены\n");

}

void HAL_RTC_IRQnDisable(RTC_HandleTypeDef *hrtc)
{

    HAL_EPIC_MaskLevelClear(EPIC_RTC_INDEX); // Выключение прерывания по уровню
    hrtc->Instance->CTRL &= ~RTC_CTRL_INTE_M; // Запрет прерывания в RTC
    HAL_RTC_WaitFlag(hrtc);

}
#endif

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