#ifndef RTC_LIB_H_INCLUDED
#define RTC_LIB_H_INCLUDED

/** \file
    Библиотека для работы с контроллером часов реального времени RTC.
*/

#include "inttypes.h"
#include "stdbool.h"

#include "rtc.h"


/** Запускает RTC
 
    \note Дата и время должны быть заданы предварительно.
    
    \param rtc Указатель для доступа к RTC
*/
void RTC_Enable(RTC_TypeDef* rtc);

/** Останавливает RTC
 
    \param rtc Указатель для доступа к RTC
*/
void RTC_Disable(RTC_TypeDef* rtc);

/** Сбрасывает флаг будильника
 
    \note Если текущее время все еще подходит 
    для срабатвания будильника, то флаг выставится вновь.
    Чтобы этого избежать, можно, например, 
    обнулить маску будильника.
    
    \param rtc Указатель для доступа к RTC
*/
void RTC_ClearAlarmFlag(RTC_TypeDef* rtc);


/** Ожидает завершение синхронизации после записи в регистры RTC

    Функция опрашивает поле FLAG регистра RRTC_CTRL,
    пока он не станет равным 0. 
    
    \note Когда флаг равен 1, 
    то идет синхронизация данных между доменами 
    и нельзя производить запись в другие регистры RTC.
    
    \param rtc Указатель для доступа к RTC
*/
void RTC_WaitDataLoading(RTC_TypeDef* rtc);


/** Ожидает срабатывания будильника
 
    Функция опрашивает флаг ALRM регистра RRTC_CTRL,
    пока он не станет равным 1.
    
    \param rtc Указатель для доступа к RTC
*/
void RTC_WaitAlarm(RTC_TypeDef* rtc);


/** Загружает время и день недели
    
    \warning Загружаемые значения должны быть корректными.
    Функция не производит их проверку.
    
    \note RTC работает с сотнями милисекунд.
    Значения не кратные 100 будут округляются вниз.
    
    \param rtc Указатель для доступа к RTC
    
    \param hours        часы
    \param minutes      минуты
    \param seconds      секунды
    \param miliseconds  милисекунды
    \param day_of_week  порядковый номер дня недели
*/
void RTC_LoadTime(RTC_TypeDef* rtc, 
    uint8_t hours, uint8_t minutes, uint8_t seconds, 
    uint16_t miliseconds, uint8_t day_of_week
);

/** Загружает время и день недели для будильника
    
    \warning Загружаемые значения должны быть корректными.
    Функция не производит их проверку.
    
    \note RTC работает с сотнями милисекунд.
    Значения не кратные 100 округляются вниз.
    
    \param rtc Указатель для доступа к RTC
    
    \param hours        часы
    \param minutes      минуты
    \param seconds      секунды
    \param miliseconds  милисекунды
    \param day_of_week  порядковый номер дня недели
*/
void RTC_LoadTimeAlarm(RTC_TypeDef* rtc, 
    uint8_t hours, uint8_t minutes, uint8_t seconds, 
    uint16_t miliseconds, uint8_t day_of_week
);


/** Загружает дату
    
    \warning Загружаемые значения должны быть корректными.
    Функция не производит их проверку.
    
    \param rtc Указатель для доступа к RTC
    
    \param date         число
    \param month        месяц
    \param year         год
*/
void RTC_LoadDate(RTC_TypeDef* rtc, 
    uint8_t date, uint8_t month, uint16_t year
);


/** Загружает дату будильника
    
    \warning Загружаемые значения должны быть корректными.
    Функция не производит их проверку.
    
    \param rtc Указатель для доступа к RTC
    
    \param date         число
    \param month        месяц
    \param year         год
*/
void RTC_LoadDateAlarm(RTC_TypeDef* rtc, 
    uint8_t date, uint8_t month, uint16_t year
);


/** Устанавливает маску будильника
    
    Если значение маски равно true,
    то поле участвует в сравнении при 
    определении срабатывания будильника.
    
    \param rtc Указатель для доступа к RTC
    
    \param date         число
    \param month        месяц
    \param year         год
    \param hours        часы
    \param minutes      минуты
    \param seconds      секунды
    \param miliseconds  милисекунды
    \param day_of_week  порядковый номер дня недели
*/
void RTC_SetAlarmMask(RTC_TypeDef* rtc,
    bool date, bool month, bool year,
    bool hours, bool minutes, bool seconds, 
    bool miliseconds, bool day_of_week
);

#endif // RTC_LIB_H_INCLUDED
