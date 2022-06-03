
#include "rtc_lib.h"

void RTC_Enable(RTC_TypeDef* rtc)
{
    rtc->CTRL |= RTC_CTRL_EN_M; 
}

void RTC_Disable(RTC_TypeDef* rtc)
{
    rtc->CTRL &= ~RTC_CTRL_EN_M; 
}

void RTC_ClearAlarmFlag(RTC_TypeDef* rtc)
{
    rtc->CTRL &= ~RTC_CTRL_ALRM_M;
}


void RTC_WaitDataLoading(RTC_TypeDef* rtc)
{
    while(rtc->CTRL & RTC_CTRL_FLAG_M) ; 
}

void RTC_WaitAlarm(RTC_TypeDef* rtc)
{
    while((rtc->CTRL & RTC_CTRL_ALRM_M) == 0) ; 
}


void RTC_WriteTimeToReg(RTC_TypeDef* rtc, volatile uint32_t* reg, 
    uint8_t hours, uint8_t minutes, uint8_t seconds, 
    uint16_t miliseconds, uint8_t day_of_week
)
{
    *reg = 
        ((day_of_week << RTC_TIME_DOW_S) & RTC_TIME_DOW_M)  |
        (((hours / 10) << RTC_TIME_TH_S) & RTC_TIME_TH_M)   |
        (((hours % 10) << RTC_TIME_H_S) & RTC_TIME_H_M)     |
        (((minutes / 10) << RTC_TIME_TM_S) & RTC_TIME_TM_M) |
        (((minutes % 10) << RTC_TIME_M_S) & RTC_TIME_M_M)   |
        (((seconds / 10) << RTC_TIME_TS_S) & RTC_TIME_TS_M) |
        (((seconds % 10) << RTC_TIME_S_S) & RTC_TIME_S_M)   |
        (((miliseconds / 100) << RTC_TIME_TOS_S) & RTC_TIME_TOS_M);    
}


void RTC_LoadTime(RTC_TypeDef* rtc, 
    uint8_t hours, uint8_t minutes, uint8_t seconds, 
    uint16_t miliseconds, uint8_t day_of_week
)
{
    RTC_WriteTimeToReg(rtc, &(rtc->TIME), 
        hours, minutes, seconds, miliseconds, day_of_week);
    RTC_WaitDataLoading(rtc);
}


void RTC_LoadTimeAlarm(RTC_TypeDef* rtc, 
    uint8_t hours, uint8_t minutes, uint8_t seconds, 
    uint16_t miliseconds, uint8_t day_of_week
)
{
    RTC_WriteTimeToReg(rtc, &(rtc->TALRM), 
        hours, minutes, seconds, miliseconds, day_of_week);
    RTC_WaitDataLoading(rtc);
}



void RTC_WriteDateToReg(RTC_TypeDef* rtc, volatile uint32_t* reg, 
    uint8_t date, uint8_t month, uint16_t year
)
{
    *reg = 
        (((date / 10) << RTC_DATE_TD_S) & RTC_DATE_TD_M)    |
        (((date % 10) << RTC_DATE_D_S) & RTC_DATE_D_M)      |
        (((month / 10) << RTC_DATE_TM_S) & RTC_DATE_TM_M)   |
        (((month % 10) << RTC_DATE_M_S) & RTC_DATE_M_M)     |
        (((year % 10) << RTC_DATE_Y_S) & RTC_DATE_Y_M)      |
        (((year / 10 % 10) << RTC_DATE_TY_S) & RTC_DATE_TY_M) |
        (((year /100 % 10) << RTC_DATE_C_S) & RTC_DATE_C_M)   |
        (((year / 1000) << RTC_DATE_TC_S) & RTC_DATE_TC_M);       
}

void RTC_LoadDate(RTC_TypeDef* rtc, 
    uint8_t date, uint8_t month, uint16_t year
)
{
    RTC_WriteDateToReg(rtc, &(rtc->DATE), 
        date, month, year);
    RTC_WaitDataLoading(rtc);
}


void RTC_LoadDateAlarm(RTC_TypeDef* rtc, 
    uint8_t date, uint8_t month, uint16_t year
)
{
    RTC_WriteDateToReg(rtc, &(rtc->DALRM), 
        date, month, year);
    RTC_WaitDataLoading(rtc);    
}


void RTC_SetAlarmMask(RTC_TypeDef* rtc,
    bool date, bool month, bool year,
    bool hours, bool minutes, bool seconds, 
    bool miliseconds, bool day_of_week
)
{
    uint32_t temp;
    
    temp = rtc->TALRM;
    temp &= ~(RTC_TALRM_CDOW_M | RTC_TALRM_CH_M | 
            RTC_TALRM_CM_M | RTC_TALRM_CS_M | RTC_TALRM_CTOS_M);
    temp |= (hours ? RTC_TALRM_CH_M : 0) |
            (minutes ? RTC_TALRM_CM_M : 0) |
            (seconds ? RTC_TALRM_CS_M : 0) |
            (miliseconds ? RTC_TALRM_CTOS_M : 0) |
            (day_of_week ? RTC_TALRM_CDOW_M : 0);
    rtc->TALRM = temp;
    
    temp = rtc->DALRM;
    temp &= ~(RTC_DALRM_CC_M | RTC_DALRM_CY_M | 
            RTC_DALRM_CM_M | RTC_DALRM_CD_M);
    temp |= (date ? RTC_DALRM_CD_M : 0) |
            (month ? RTC_DALRM_CM_M : 0) |
            (year ? (RTC_DALRM_CC_M | RTC_DALRM_CY_M) : 0);
    rtc->DALRM = temp;
    
}