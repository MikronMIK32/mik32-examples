#include "common.h"
#include "rtc.h"
#include "wakeup.h"
#include "power_manager.h"
// #include "epic.h"

typedef struct __RTC_HandleTypeDef
{
  RTC_TypeDef                 *Instance;  /*!< Register base address    */

} RTC_HandleTypeDef;

typedef struct
{
  uint8_t Dow;            /*  Параметр RTC. День недели.
                              Этот параметр должен быть числом между Min = 1 и Max = 7 */

  uint8_t Hours;           /*  Параметр RTC. Часы.
                              Этот параметр должен быть числом между Min = 0 и Max = 23 */

  uint8_t Minutes;         /*  Параметр RTC. Минуты.
                              Этот параметр должен быть числом между Min = 0 и Max = 59 */

  uint8_t Seconds;         /*  Параметр RTC. Секунды.
                              Этот параметр должен быть числом между Min = 0 и Max = 59 */

} RTC_TimeTypeDef;

typedef struct
{
  uint8_t Century;        /*  Параметр RTC. Век.
                              Этот параметр должен быть числом между Min = 0 и Max = 99 */


  uint8_t Day;            /*  Параметр RTC. Число.
                              Этот параметр должен быть числом между Min = 1 и Max = 31 */


  uint8_t Month;          /*  Параметр RTC. Месяц.
                              Этот параметр должен быть числом между Min = 1 и Max = 12 */


  uint8_t Year;            /*  Параметр RTC. Год.
                              Этот параметр должен быть числом между Min = 0 и Max = 99 */

} RTC_DateTypeDef;

typedef struct
{
  RTC_TimeTypeDef AlarmTime;  /* Параметр RTC. Время будильника */

  RTC_DateTypeDef AlarmDate;  /* Параметр RTC. Время будильника */

  uint32_t MaskAlarmTime;   /* Параметр RTC. Маска сравнения будильника по времени */
  uint32_t MaskAlarmDate;   /* Параметр RTC. Маска сравнения будильника по дате */

} RTC_AlarmTypeDef;

/* Месяц */
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

/* День недели */
#define RTC_WEEKDAY_MONDAY             ((uint8_t)0x01)
#define RTC_WEEKDAY_TUESDAY            ((uint8_t)0x02)
#define RTC_WEEKDAY_WEDNESDAY          ((uint8_t)0x03)
#define RTC_WEEKDAY_THURSDAY           ((uint8_t)0x04)
#define RTC_WEEKDAY_FRIDAY             ((uint8_t)0x05)
#define RTC_WEEKDAY_SATURDAY           ((uint8_t)0x06)
#define RTC_WEEKDAY_SUNDAY             ((uint8_t)0x07)




void HAL_RTC_WaitFlag(RTC_HandleTypeDef *hrtc);
void HAL_RTC_Disable(RTC_HandleTypeDef *hrtc);
void HAL_RTC_Enable(RTC_HandleTypeDef *hrtc);
//void HAL_RTC_Init(RTC_HandleTypeDef *hrtc);
void HAL_RTC_SetTime(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *sTime);
void HAL_RTC_SetDate(RTC_HandleTypeDef *hrtc, RTC_DateTypeDef *sDate);
void HAL_RTC_Alarm_SetTime(RTC_HandleTypeDef *hrtc, RTC_AlarmTypeDef *sAlarm);
void HAL_RTC_Alarm_SetDate(RTC_HandleTypeDef *hrtc, RTC_AlarmTypeDef *sAlarm);
void HAL_RTC_SetAlarm(RTC_HandleTypeDef *hrtc, RTC_AlarmTypeDef *sAlarm);
void HAL_RTC_CheckDate(RTC_HandleTypeDef *hrtc);
void HAL_RTC_CheckTime(RTC_HandleTypeDef *hrtc);
void HAL_RTC_Check(RTC_HandleTypeDef *hrtc);