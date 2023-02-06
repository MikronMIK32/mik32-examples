#ifndef MIK32_HAL_TIMER16
#define MIK32_HAL_TIMER16

#include "def_list.h"
#include "mcu32_memory_map.h"
#include "power_manager.h"
#include "timer16.h"
#include "pad_config.h"
#include "stdbool.h"

#ifdef MIK32_HAL_TIMER16_DEBUG
#include "common.h"
#endif


/* Сдвиг настройки тактироания таймера в регистре PM.Timer_CFG */
#define MUX_TIM16_0    9     /* Сдвиг настроек тактирования Timer16_0 */
#define MUX_TIM16_1    12    /* Сдвиг настроек тактирования Timer16_1 */
#define MUX_TIM16_2    15    /* Сдвиг настроек тактирования Timer16_2 */

/* Выводы таймеров */
#define PORT_AS_TIMER    0b10    /* Третья функция вывода (последовательный интерфейс или таймер) */ 
#define TIMER16_0_IN1    5      /* Timer16_0 - вход 1 */
#define TIMER16_0_IN2    6      /* Timer16_0 - вход 2 */
#define TIMER16_0_OUT    7      /* Timer16_0 - выход */
#define TIMER16_1_IN1    8      /* Timer16_1 - вход 1 */
#define TIMER16_1_IN2    9      /* Timer16_1 - вход 2 */
#define TIMER16_1_OUT    10     /* Timer16_1 - выход */
#define TIMER16_2_IN1    11     /* Timer16_2 - вход 1 */
#define TIMER16_2_IN2    12     /* Timer16_2 - вход 2 */
#define TIMER16_2_OUT    13     /* Timer16_2 - выход */

/* TimerMode - Режим работы таймера */
#define TIMER16_MODE_COUNTER    0     /* Режим счетчика */
#define TIMER16_MODE_WAVE       1     /* Режим генераии волновой формы */
#define TIMER16_MODE_TIMEOUT    2     /* Режим Timeout */

/* ClockSource - Источник тактирования */
#define TIMER16_SOURCE_INTERNAL_SYS         0x0      /* Тактирование от системной частоты (sys_clk) */
#define TIMER16_SOURCE_INTERNAL_AHB         0x1      /* Тактирование от частоты шины AHB */
#define TIMER16_SOURCE_INTERNAL_OSC32M      0x2      /* Тактирование от частоты внешнего осцилятора OSC32М */
#define TIMER16_SOURCE_INTERNAL_HSI32M      0x3      /* Тактирование от частоты встроенного осцилятора HSI32M */
#define TIMER16_SOURCE_INTERNAL_OSC32K      0x4      /* Тактирование от частоты внешнего осцилятора OSC32К */
#define TIMER16_SOURCE_INTERNAL_LSI32K      0x5      /* Тактирование от частоты встроенного осцилятора LSI32K */
#define TIMER16_SOURCE_EXTERNAL_INPUT1      0x6      /* Тактирование от внешнего вывода Input1 */

/* Prescaler - Делитель частоты */
#define TIMER16_PRESCALER_1         000     /* Делитель частоты 1 */
#define TIMER16_PRESCALER_2         001     /* Делитель частоты 2 */
#define TIMER16_PRESCALER_4         010     /* Делитель частоты 4 */
#define TIMER16_PRESCALER_8         011     /* Делитель частоты 8 */
#define TIMER16_PRESCALER_16        100     /* Делитель частоты 16 */
#define TIMER16_PRESCALER_32        101     /* Делитель частоты 32 */
#define TIMER16_PRESCALER_64        110     /* Делитель частоты 64 */
#define TIMER16_PRESCALER_128       111     /* Делитель частоты 128 */

/* CountMode - Источник тактового сигнала таймера для синхронизации счетчика */
#define TIMER16_COUNTMODE_INTERNAL     0     /* Cчетчик инкрементируется после каждого внутреннего тактового импульса */
#define TIMER16_COUNTMODE_EXTERNAL     1     /* Cчетчик увеличивается после каждого действительного тактового импульса на внешнем выводе lnput1 */

/* ActiveEdge - Полярность синхронизации. Выбор активного фронта при тактировании от внешнего источника */
#define TIMER16_ACTIVEEDGE_RISING     0b00     /* Нарастающий фронт является активным фронтом, используемым для подсчета */
#define TIMER16_ACTIVEEDGE_FOLLING    0b01     /* Спадающий фронт является активным фронтом, используемым для подсчета */
#define TIMER16_ACTIVEEDGE_BOTH       0b10     /* Оба фронта являются активными фронтами */

/* TriggerSource - Источник тригера */
#define TIMER16_TRIGGER_TIM0_GPIO0_7             0b000      /* Источник тригера - Timer16_0 */   
#define TIMER16_TRIGGER_TIM1_GPIO1_9             0b000      /* Источник тригера - Timer16_1 */   
#define TIMER16_TRIGGER_TIM2_GPIO2_3             0b000      /* Источник тригера - Timer16_2 */   

#define TIMER16_TRIGGER_TIM0_GPIO0_4             0b001      /* Источник тригера - Timer16_0 */   
#define TIMER16_TRIGGER_TIM1_GPIO1_8             0b001      /* Источник тригера - Timer16_1 */   
#define TIMER16_TRIGGER_TIM2_GPIO2_2             0b001      /* Источник тригера - Timer16_2 */   

#define TIMER16_TRIGGER_TIM0_GPIO0_15            0b010      /* Источник тригера - Timer16_0 */   
#define TIMER16_TRIGGER_TIM1_GPIO1_7             0b010      /* Источник тригера - Timer16_1 */   
#define TIMER16_TRIGGER_TIM2_GPIO2_1             0b010      /* Источник тригера - Timer16_2 */   

#define TIMER16_TRIGGER_TIM0_GPIO0_14            0b011      /* Источник тригера - Timer16_0 */   
#define TIMER16_TRIGGER_TIM1_GPIO1_6             0b011      /* Источник тригера - Timer16_1 */   
#define TIMER16_TRIGGER_TIM2_GPIO2_0             0b011      /* Источник тригера - Timer16_2 */   

#define TIMER16_TRIGGER_TEMPERATURE         0b100      /* Источник тригера - Термосенсор */
#define TIMER16_TRIGGER_ADC                 0b101      /* Источник тригера - Окончание преобразования АЦП */
#define TIMER16_TRIGGER_RTC                 0b110      /* Источник тригера - Прерывание RTC */
#define TIMER16_TRIGGER_AlARM               0b111      /* Источник тригера - Будильник */

/* Выводы триггеров */
#define PORT_AS_GPIO    0b01    /* Вторая функция вывода (GPIO) */ 

/* ActiveEdge - Полярность синхронизации. Выбор активного фронта при тактировании от внешнего источника */
#define TIMER16_TRIGGER_ACTIVEEDGE_SOFTWARE    0b00     /* Програмный тригер. Запускается при установленном CNTSTRT или SNGSTRT */
#define TIMER16_TRIGGER_ACTIVEEDGE_RISING      0b01     /* Нарастающий фронт является активным фронтом */
#define TIMER16_TRIGGER_ACTIVEEDGE_FOLLING     0b10     /* Спадающий фронт является активным фронтом */
#define TIMER16_TRIGGER_ACTIVEEDGE_BOTH        0b11     /* Оба фронта являются активными фронтами */

/* Preload - Режим обновления регисторов ARR и CMP */
#define TIMER16_PRELOAD_AFTERWRITE    0     /* Регистры обновляются после каждого доступа к записи на шине APB */
#define TIMER16_PRELOAD_ENDPERIOD     1     /* Регистры обновляются в конце текущего периода Timer16 */

/* Filter ExternalClock/Trigger - Фильтр для внешнего тактового генератора/триггера */
#define TIMER16_FILTER_NONE      0b00     /* Фильтр отключен */
#define TIMER16_FILTER_2CLOCK    0b01     /* Фильтр на 2 такта */
#define TIMER16_FILTER_4CLOCK    0b10     /* Фильтр на 4 такта */
#define TIMER16_FILTER_8CLOCK    0b11     /* Фильтр на 8 тактов */

typedef struct
{
    uint8_t Source;         /* Источник тактирования */

    uint8_t Prescaler;      /* Делитель частоты */

} Timer16_ClockConfigTypeDef;

typedef struct
{
    uint8_t Source;         /* Источник тригера */

    uint8_t ActiveEdge;     /* Активный фронт */

} Timer16_TriggerConfigTypeDef;

typedef struct
{
    uint8_t ExternalClock;    /* Фильтр для внешнего тактового генератора */

    uint8_t Trigger;          /* фильтр для триггера */

} Timer16_FilterConfigTypeDef;

typedef struct
{
    TIMER16_TypeDef *Instance;       /* Базовый адрес регистров Timer16 */

    uint8_t TimerMode;                       /* Режим работы таймера */
    Timer16_ClockConfigTypeDef Clock;        /* Настройки тактирования */
    uint8_t CountMode;                       /* Источник синхронизации */
    uint8_t ActiveEdge;                      /* Активный фронт */

    uint16_t Period;                         /* Верхнее значение счета */
    uint8_t Preload;                         /* Режим записи в ARR и CMP */

    Timer16_TriggerConfigTypeDef Trigger;    /* Настройки тригера */

    Timer16_FilterConfigTypeDef Filter;      /* НАстройки фильтра */
    
} Timer16_HandleTypeDef;


void HAL_Timer16_Disable(Timer16_HandleTypeDef *htimer16);
void HAL_Timer16_Enable(Timer16_HandleTypeDef *htimer16);
void HAL_Timer16_SetPad(Timer16_HandleTypeDef *htimer16);
void HAL_Timer16_SetActiveEdge(Timer16_HandleTypeDef *htimer16, uint8_t ActiveEdge);
void HAL_Timer16_SetSourceClock(Timer16_HandleTypeDef *htimer16, uint8_t SourceClock);
void HAL_Timer16_SetCountMode(Timer16_HandleTypeDef *htimer16, uint8_t CountMode);
void HAL_Timer16_ClockInit(Timer16_HandleTypeDef *htimer16);
void HAL_Timer16_SetPreload(Timer16_HandleTypeDef *htimer16, uint8_t Preload);
void HAL_Timer16_WaitARROK(Timer16_HandleTypeDef *htimer16);
void HAL_Timer16_WaitCMPOK(Timer16_HandleTypeDef *htimer16);
void HAL_Timer16_SetARR(Timer16_HandleTypeDef *htimer16, uint16_t Period);
void HAL_Timer16_SetCMP(Timer16_HandleTypeDef *htimer16, uint16_t Compare);
void HAL_Timer16_SelectTrigger(Timer16_HandleTypeDef *htimer16, uint8_t TriggerSource);
void HAL_Timer16_SetTriggerEdge(Timer16_HandleTypeDef *htimer16, uint8_t TriggerEdge);
void HAL_Timer16_CounterModeInit(Timer16_HandleTypeDef *htimer16);
void HAL_Timer16_SetFilterExternalClock(Timer16_HandleTypeDef *htimer16, uint8_t FilterExternalClock);
void HAL_Timer16_SetFilterTrigger(Timer16_HandleTypeDef *htimer16, uint8_t FilterTrigger);
void HAL_Timer16_Init(Timer16_HandleTypeDef *htimer16);
uint16_t HAL_Timer16_GetCounterValue(Timer16_HandleTypeDef *htimer16);
void HAL_Timer16_StartSingleMode(Timer16_HandleTypeDef *htimer16);
void HAL_Timer16_StartContinuousMode(Timer16_HandleTypeDef *htimer16);
void HAL_Timer16_InvertOutput(Timer16_HandleTypeDef *htimer16);
void HAL_Timer16_OutConfig(Timer16_HandleTypeDef *htimer16);
void HAL_Timer16_StartPWM(Timer16_HandleTypeDef *htimer16, uint16_t Period, uint16_t Compare);
void HAL_Timer16_StartOneShot(Timer16_HandleTypeDef *htimer16, uint16_t Period, uint16_t Compare);
void HAL_Timer16_StartSetOnes(Timer16_HandleTypeDef *htimer16, uint16_t Period, uint16_t Compare);
void HAL_Timer16_ClearTriggerFlag(Timer16_HandleTypeDef *htimer16);
void HAL_Timer16_WaitTrigger(Timer16_HandleTypeDef *htimer16);


#endif