#ifndef MIK32_HAL_RTC
#define MIK32_HAL_RTC


#include "def_list.h"
#include "mcu32_memory_map.h"
#include "rtc.h"

#ifdef MIK32_RTC_IRQn
#include "mik32_hal_irq.h"
#endif

#ifdef MIK32_RTC_DEBUG
#include "common.h"
#endif


/* Title: Макросы */

/*
 * Defines: Месяц
 * RTC_MONTH_JANUARY - Январь
 * RTC_MONTH_FEBRUARY - Февраль
 * RTC_MONTH_MARCH - Март
 * RTC_MONTH_APRIL - Апрель
 * RTC_MONTH_MAY - Май    
 * RTC_MONTH_JUNE - Июнь
 * RTC_MONTH_JULY - Июль
 * RTC_MONTH_AUGUST - Август
 * RTC_MONTH_SEPTEMBER - Сентябрь
 * RTC_MONTH_OCTOBER - Октябрь
 * RTC_MONTH_NOVEMBER - Ноябрь
 * RTC_MONTH_DECEMBER - Декабрь
 *
 */
#define RTC_MONTH_JANUARY              ((uint8_t)0x01)
#define RTC_MONTH_FEBRUARY             ((uint8_t)0x02)
#define RTC_MONTH_MARCH                ((uint8_t)0x03)
#define RTC_MONTH_APRIL                ((uint8_t)0x04)
#define RTC_MONTH_MAY                  ((uint8_t)0x05)
#define RTC_MONTH_JUNE                 ((uint8_t)0x06)
#define RTC_MONTH_JULY                 ((uint8_t)0x07)
#define RTC_MONTH_AUGUST               ((uint8_t)0x08)
#define RTC_MONTH_SEPTEMBER            ((uint8_t)0x09)
#define RTC_MONTH_OCTOBER              ((uint8_t)0x10)
#define RTC_MONTH_NOVEMBER             ((uint8_t)0x11)
#define RTC_MONTH_DECEMBER             ((uint8_t)0x12)


/*
 * Defines: День недели
 * RTC_WEEKDAY_MONDAY - Понедельник
 * RTC_WEEKDAY_TUESDAY - Вторник
 * RTC_WEEKDAY_WEDNESDAY - Среда
 * RTC_WEEKDAY_THURSDAY - Четверг
 * RTC_WEEKDAY_FRIDAY - Пятница
 * RTC_WEEKDAY_SATURDAY - Суббота
 * RTC_WEEKDAY_SUNDAY - Воскресенье
 *
 */
#define RTC_WEEKDAY_MONDAY             ((uint8_t)0x01)
#define RTC_WEEKDAY_TUESDAY            ((uint8_t)0x02)
#define RTC_WEEKDAY_WEDNESDAY          ((uint8_t)0x03)
#define RTC_WEEKDAY_THURSDAY           ((uint8_t)0x04)
#define RTC_WEEKDAY_FRIDAY             ((uint8_t)0x05)
#define RTC_WEEKDAY_SATURDAY           ((uint8_t)0x06)
#define RTC_WEEKDAY_SUNDAY             ((uint8_t)0x07)

/* Прерывание будильника Alarm */
#define RTC_ALARM_IRQn_DISABLE         0
#define RTC_ALARM_IRQn_ENABLE          1

/* Title: Структуры */

typedef struct
{
	uint8_t Alarm; /* Разрешение прерывания при наличии установленного бита ALRM */

} RTC_IRQnTypeDef;

/*
 * Struct: RTC_HandleTypeDef
 * Настройки RTC
 * 
 */
typedef struct
{
	/*
	* Variable: Instance
	* Базоый адрес регистров RTC
	*
	*/
	RTC_TypeDef                 *Instance;  /* Базоый адрес регистров RTC */

	RTC_IRQnTypeDef             Interrupts; /* Прерывания RTC */

} RTC_HandleTypeDef;

/*
 * Struct: RTC_TimeTypeDef
 * Настройки времени и дня недели
 * 
 */
typedef struct
{
	/*
	* Variable: Dow
	* День недели.
	* 
	* Этот параметр должен быть одним из значений:
	*
	* - <RTC_WEEKDAY_MONDAY>   
	* - <RTC_WEEKDAY_TUESDAY>
	* - <RTC_WEEKDAY_WEDNESDAY>
	* - <RTC_WEEKDAY_THURSDAY>
	* - <RTC_WEEKDAY_FRIDAY>
	* - <RTC_WEEKDAY_SATURDAY>
	* - <RTC_WEEKDAY_SUNDAY>
	*
	*/
	uint8_t Dow;

	/*
	* Variable: Hours
	* Часы.
	* 
	* Этот параметр должен быть числом в пределах от 0 до 23.
	*
	*/
	uint8_t Hours;

	/*
	* Variable: Minutes
	* Минуты.
	* 
	* Этот параметр должен быть числом в пределах от 0 до 59.
	*
	*/
	uint8_t Minutes;

	/*
	* Variable: Seconds
	* Секунды.
	* 
	* Этот параметр должен быть числом в пределах от 0 до 59.
	*
	*/
	uint8_t Seconds;

} RTC_TimeTypeDef;

/*
 * Struct: RTC_DateTypeDef
 * Настройки даты
 * 
 */
typedef struct
{
	/*
	* Variable: Century
	* Век.
	* 
	* Этот параметр должен быть числом в пределах от 0 до 99.
	*
	*/
	uint8_t Century;
	
	/*
	* Variable: Day
	* Число.
	*
	* Этот параметр должен быть числом в пределах от 1 до 31.
	*
	*/
	uint8_t Day;
	
	
	/*
	* Variable: Month
	* Месяц.
	*
	* Этот параметр должен быть одним из значений:
	*
	* - <RTC_MONTH_JANUARY> 
	* - <RTC_MONTH_FEBRUARY>
	* - <RTC_MONTH_MARCH>
	* - <RTC_MONTH_APRIL>
	* - <RTC_MONTH_MAY>
	* - <RTC_MONTH_JUNE>
	* - <RTC_MONTH_JULY>
	* - <RTC_MONTH_AUGUST>
	* - <RTC_MONTH_SEPTEMBER>
	* - <RTC_MONTH_OCTOBER>
	* - <RTC_MONTH_NOVEMBER>
	* - <RTC_MONTH_DECEMBER>
	*
	*/
	uint8_t Month;
	
	
	/*
	* Variable: Year
	* Год.
	*
	* Этот параметр должен быть числом в пределах от 0 до 99.
	*
	*/
	uint8_t Year;

} RTC_DateTypeDef;

/*
 * Struct: RTC_AlarmTypeDef
 * Настройки времени и даты будильника.
 * 
 */
typedef struct
{
	/*
	* Variable: AlarmTime
	* Время будильника.
	*
	*/
	RTC_TimeTypeDef AlarmTime;

	/*
	* Variable: AlarmDate
	* Дата будильника.
	*
	*/
	RTC_DateTypeDef AlarmDate;

	/*
	* Variable: MaskAlarmTime
	* Маска сравнения будильника по времени.
	*
	* Параметр задает по каким значениям будет сраниваться текущее время RTC с временем будильника.
	*
	* Возможные значения:
	* 
	* - RTC_TALRM_CDOW_M Сравнивать по дню недели;
	* - RTC_TALRM_CH_M Сравнивать по часам;
	* - RTC_TALRM_CM_M Сравнивать по минутам;
	* - RTC_TALRM_CS_M Сравнивать по секундам.
	*
	*/
	uint32_t MaskAlarmTime;
	/*
	* Variable: MaskAlarmDate
	* Маска сравнения будильника по дате
	* 
	* Параметр задает по каким значениям будет сраниваться текущая дата RTC с датой будильника.
	*
	* Возможные значения: 
	*
	* - RTC_DALRM_CC_M Сравнивать по веку;
	* - RTC_DALRM_CD_M Сравнивать по числу;
	* - RTC_DALRM_CM_M Сравнивать по месяцу;
	* - RTC_DALRM_CY_M Сравнивать по году.
	*
	*/
	uint32_t MaskAlarmDate;

} RTC_AlarmTypeDef;

/* Title: Функции */

/*
 * Function: HAL_RTC_WaitFlag
 * Ожидание сброса флага FLAG.
 * 
 * Сигнал проведения синхронизации между тактовыми доменами. После записи в любой регистр 
 * и пока данный флаг читается равным «1», запрещено выполнять любую новую запись
 *
 * Parameters:
 * hrtc - Указатель на структуру с настройками RTC.
 *
 * Returns:
 * void
 */
void HAL_RTC_WaitFlag(RTC_HandleTypeDef *hrtc);

/*
 * Function: HAL_RTC_Disable
 * Выключить RTC.
 * 
 * Перед установкой даты и времени RTC нужно использовать эту функцию.
 *
 * Parameters:
 * hrtc - Указатель на структуру с настройками RTC.
 *
 * Returns:
 * void
 */
void HAL_RTC_Disable(RTC_HandleTypeDef *hrtc);

/*
 * Function: HAL_RTC_Enable
 * Включить RTC.
 *
 * Parameters:
 * hrtc - Указатель на структуру с настройками RTC.
 *
 * Returns:
 * void
 */
void HAL_RTC_Enable(RTC_HandleTypeDef *hrtc);

/*
 * Function: HAL_RTC_SetTime
 * Установить время RTC в соответсвии с параметрами sTime.
 *
 * Parameters:
 * hrtc - Указатель на структуру с настройками RTC.
 * sTime - Указатель на структуру с параметрами времени.
 *
 * Returns:
 * void
 */
void HAL_RTC_SetTime(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *sTime);

/*
 * Function: HAL_RTC_SetDate
 * Установить дату RTC в соответсвии с параметрами sTime.
 *
 * Parameters:
 * hrtc - Указатель на структуру с настройками RTC.
 * sDate - Указатель на структуру с параметрами даты.
 *
 * Returns:
 * void
 */
void HAL_RTC_SetDate(RTC_HandleTypeDef *hrtc, RTC_DateTypeDef *sDate);

/*
 * Function: HAL_RTC_SetRegValue
 * Задать значение регистра общего пользования.
 *
 * Parameters:
 * hrtc - Указатель на структуру с настройками RTC.
 * RegIndex - Номер регистра.
 * Value - Значние.
 *
 * Returns:
 * void
 */
void HAL_RTC_SetRegValue(RTC_HandleTypeDef *hrtc, uint32_t RegIndex, uint32_t Value);

/*
 * Function: HAL_RTC_Alarm_SetTime
 * Установить время будильника и маску сравнения будильника по времени в соответсвии с параметрами sAlarm.
 *
 * Parameters:
 * hrtc - Указатель на структуру с настройками RTC.
 * sAlarm - Указатель на структуру с параметрами будильника.
 *
 * Returns:
 * void
 */
void HAL_RTC_Alarm_SetTime(RTC_HandleTypeDef *hrtc, RTC_AlarmTypeDef *sAlarm);

/*
 * Function: HAL_RTC_Alarm_SetDate
 * Установить дату  будильника и маску сравнения будильника по дате в соответсвии с параметрами sAlarm.
 *
 * Parameters:
 * hrtc - Указатель на структуру с настройками RTC.
 * sAlarm - Указатель на структуру с параметрами будильника.
 *
 * Returns:
 * void
 */
void HAL_RTC_Alarm_SetDate(RTC_HandleTypeDef *hrtc, RTC_AlarmTypeDef *sAlarm);

/*
 * Function: HAL_RTC_SetAlarm
 * Установить время, дату и маску сравнения будильника в соответсвии с параметрами sAlarm.
 *
 * Parameters:
 * hrtc - Указатель на структуру с настройками RTC.
 * sAlarm - Указатель на структуру с параметрами будильника.
 *
 * Returns:
 * void
 */
void HAL_RTC_SetAlarm(RTC_HandleTypeDef *hrtc, RTC_AlarmTypeDef *sAlarm);

/*
 * Function: HAL_RTC_AlarmDisable
 * Отключить маску сравнения будильника по времени и дате.
 *
 * Parameters:
 * hrtc - Указатель на структуру с настройками RTC.
 *
 * Returns:
 * void
 */
void HAL_RTC_AlarmDisable(RTC_HandleTypeDef *hrtc);

/*
 * Function: HAL_RTC_AlrmClear
 * Очистить флаг будильника ALRM.
 * 
 * Установленный флаг ALRM свидетельствует о совпадении одного разрешенного или всех разрешённых полей будильника. 
 * Бит необходимо сбрасывать программно.
 *
 * Parameters:
 * hrtc - Указатель на структуру с настройками RTC.
 *
 * Returns:
 * void
 */
void HAL_RTC_AlrmClear(RTC_HandleTypeDef *hrtc);

/*
 * Function: HAL_RTC_GetFlagALRM
 * Проверить состояние флага будильника ALRM
 *
 * Parameters:
 * hrtc - Указатель на структуру с настройками RTC.
 *
 * Returns:
 * (uint8_t ) - Состояние флага будильника ALRM
 *
 */
uint8_t HAL_RTC_GetFlagALRM(RTC_HandleTypeDef *hrtc);

#ifdef MIK32_RTC_IRQn
void HAL_RTC_IRQnEnable(RTC_HandleTypeDef *hrtc);
void HAL_RTC_IRQnDisable(RTC_HandleTypeDef *hrtc);
#endif

#ifdef MIK32_RTC_DEBUG
/*
 * Function: HAL_RTC_CheckDate
 * Вывести дату RTC в UART.
 * 
 * Используется при включенном UART и 
 *
 * Parameters:
 * hrtc - Указатель на структуру с настройками RTC.
 *
 * Returns:
 * void
 */
void HAL_RTC_CheckDate(RTC_HandleTypeDef *hrtc);

/*
 * Function: HAL_RTC_CheckTime
 * Вывести время RTC в UART.
 * 
 * Используется при включенном UART.
 *
 * Parameters:
 * hrtc - Указатель на структуру с настройками RTC.
 *
 * Returns:
 * void
 */
void HAL_RTC_CheckTime(RTC_HandleTypeDef *hrtc);

/*
 * Function: HAL_RTC_Check
 * Вывести время и дату RTC в UART
 *
 * Parameters:
 * hrtc - Указатель на структуру с настройками RTC.
 * 
 * Используется при включенном UART.
 *
 * Returns:
 * void
 */
void HAL_RTC_Check(RTC_HandleTypeDef *hrtc);
#endif

#endif