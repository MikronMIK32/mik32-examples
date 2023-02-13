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


/* Title: Макросы */
/* Title: Сдвиг настройки тактирования таймера в регистре Timer_CFG */
/*
 * Define: MUX_TIM16_1
 * Сдвиг настроек тактирования Timer16_1
 */
#define MUX_TIM16_0    9   
/*
 * Define: MUX_TIM16_1
 * Сдвиг настроек тактирования Timer16_1
 */
#define MUX_TIM16_1    12 
/*
 * Define: MUX_TIM16_2
 * Сдвиг настроек тактирования Timer16_2
 */
#define MUX_TIM16_2    15

/* Title: Выводы таймеров */
/*
 * Define: PORT_AS_TIMER
 * Третья функция вывода (последовательный интерфейс или таймер)
 *
 */
#define PORT_AS_TIMER    0b10
/*
 * Define: TIMER16_0_IN1
 * Timer16_0 - вход 1
 *
 */
#define TIMER16_0_IN1    5      
/*
 * Define: TIMER16_0_IN2
 * Timer16_0 - вход 2
 *
 */
#define TIMER16_0_IN2    6      
/*
 * Define: TIMER16_0_OUT
 * Timer16_0 - выход
 *
 */
#define TIMER16_0_OUT    7      
/*
 * Define: TIMER16_1_IN1
 * Timer16_1 - вход 1
 *
 */
#define TIMER16_1_IN1    8      
/*
 * Define: TIMER16_1_IN2
 * Timer16_1 - вход 2
 *
 */
#define TIMER16_1_IN2    9      
/*
 * Define: TIMER16_1_OUT
 * Timer16_1 - выход
 *
 */
#define TIMER16_1_OUT    10     
/*
 * Define: TIMER16_2_IN1
 * Timer16_2 - вход 1
 *
 */
#define TIMER16_2_IN1    11     
/*
 * Define: TIMER16_2_IN2
 * Timer16_2 - вход 2
 *
 */
#define TIMER16_2_IN2    12
/*
 * Define: TIMER16_2_OUT
 * Timer16_2 - выход
 *
 */
#define TIMER16_2_OUT    13


/* Title: Источник тактирования */
/*
 * Define: TIMER16_SOURCE_INTERNAL_SYSTEM
 * Тактирование от системной частоты (sys_clk)
 *
 */
#define TIMER16_SOURCE_INTERNAL_SYSTEM      0x0
/*
 * Define: TIMER16_SOURCE_INTERNAL_AHB
 * Тактирование от частоты шины AHB
 *
 */
#define TIMER16_SOURCE_INTERNAL_AHB         0x1      
/*
 * Define: TIMER16_SOURCE_INTERNAL_OSC32M
 * Тактирование от частоты внешнего осциллятора OSC32М
 *
 */
#define TIMER16_SOURCE_INTERNAL_OSC32M      0x2      
/*
 * Define: TIMER16_SOURCE_INTERNAL_HSI32M
 * Тактирование от частоты встроенного осциллятора HSI32M
 *
 */
#define TIMER16_SOURCE_INTERNAL_HSI32M      0x3      
/*
 * Define: TIMER16_SOURCE_INTERNAL_OSC32K
 * Тактирование от частоты внешнего осциллятора OSC32К
 *
 */
#define TIMER16_SOURCE_INTERNAL_OSC32K      0x4      
/*
 * Define: TIMER16_SOURCE_INTERNAL_LSI32K
 * Тактирование от частоты встроенного осциллятора LSI32K
 *
 */
#define TIMER16_SOURCE_INTERNAL_LSI32K      0x5
/*
 * Define: TIMER16_SOURCE_EXTERNAL_INPUT1
 * Тактирование от внешнего вывода Input1
 *
 */
#define TIMER16_SOURCE_EXTERNAL_INPUT1      0x6


/* Title: Делитель частоты */
/*
 * Define: TIMER16_PRESCALER_1
 * Делитель частоты 1
 *
 */
#define TIMER16_PRESCALER_1         0b000
/*
 * Define: TIMER16_PRESCALER_2
 * Делитель частоты 2
 *
 */
#define TIMER16_PRESCALER_2         0b001
/*
 * Define: TIMER16_PRESCALER_4
 * Делитель частоты 4
 *
 */
#define TIMER16_PRESCALER_4         0b010
/*
 * Define: TIMER16_PRESCALER_8
 * Делитель частоты 8
 *
 */
#define TIMER16_PRESCALER_8         0b011
/*
 * Define: TIMER16_PRESCALER_16
 * Делитель частоты 16
 *
 */
#define TIMER16_PRESCALER_16        0b100
/*
 * Define: TIMER16_PRESCALER_32
 * Делитель частоты 32
 *
 */
#define TIMER16_PRESCALER_32        0b101
/*
 * Define: TIMER16_PRESCALER_64
 * Делитель частоты 64
 *
 */
#define TIMER16_PRESCALER_64        0b110
/*
 * Define: TIMER16_PRESCALER_128
 * Делитель частоты 128
 *
 */
#define TIMER16_PRESCALER_128       0b111


/* Title: Источник тактового сигнала таймера для синхронизации счетчика */
/*
 * Define: TIMER16_COUNTMODE_INTERNAL
 * Cчетчик инкрементируется после каждого внутреннего тактового импульса
 *
 */
#define TIMER16_COUNTMODE_INTERNAL     0
/*
 * Define: TIMER16_COUNTMODE_EXTERNAL
 * Cчетчик увеличивается после каждого действительного тактового импульса на внешнем выводе lnput1
 *
 */
#define TIMER16_COUNTMODE_EXTERNAL     1


/* 
 * Title: Полярность синхронизации
 * Выбор активного фронта при тактировании от внешнего источника 
 */
/*
 * Define: TIMER16_ACTIVEEDGE_RISING
 * Нарастающий фронт является активным фронтом, используемым для подсчета
 *
 */
#define TIMER16_ACTIVEEDGE_RISING     0b00
/*
 * Define: TIMER16_ACTIVEEDGE_FOLLING
 * Спадающий фронт является активным фронтом, используемым для подсчета
 *
 */
#define TIMER16_ACTIVEEDGE_FOLLING    0b01
/*
 * Define: TIMER16_ACTIVEEDGE_BOTH
 * Оба фронта являются активными фронтами
 *
 */
#define TIMER16_ACTIVEEDGE_BOTH       0b10


/* Title: Источник триггера */
/*
 * Define: TIMER16_TRIGGER_TIM0_GPIO0_7
 * Источник триггера - Timer16_0
 *
 */
#define TIMER16_TRIGGER_TIM0_GPIO0_7             0b000   
/*
 * Define: TIMER16_TRIGGER_TIM1_GPIO1_9
 * Источник триггера - Timer16_1
 *
 */
#define TIMER16_TRIGGER_TIM1_GPIO1_9             0b000
/*
 * Define: TIMER16_TRIGGER_TIM2_GPIO2_3
 * Источник триггера - Timer16_2
 *
 */
#define TIMER16_TRIGGER_TIM2_GPIO2_3             0b000

/*
 * Define: TIMER16_TRIGGER_TIM0_GPIO0_4
 * Источник триггера - Timer16_0
 *
 */
#define TIMER16_TRIGGER_TIM0_GPIO0_4             0b001
/*
 * Define: TIMER16_TRIGGER_TIM1_GPIO1_8
 * Источник триггера - Timer16_1
 *
 */
#define TIMER16_TRIGGER_TIM1_GPIO1_8             0b001   
/*
 * Define: TIMER16_TRIGGER_TIM2_GPIO2_2
 * Источник триггера - Timer16_2
 *
 */
#define TIMER16_TRIGGER_TIM2_GPIO2_2             0b001 

/*
 * Define: TIMER16_TRIGGER_TIM0_GPIO0_15
 * Источник триггера - Timer16_0
 *
 */
#define TIMER16_TRIGGER_TIM0_GPIO0_15            0b010
/*
 * Define: TIMER16_TRIGGER_TIM1_GPIO1_7
 * Источник триггера - Timer16_1
 *
 */
#define TIMER16_TRIGGER_TIM1_GPIO1_7             0b010 
/*
 * Define: TIMER16_TRIGGER_TIM2_GPIO2_1
 * Источник триггера - Timer16_2
 *
 */
#define TIMER16_TRIGGER_TIM2_GPIO2_1             0b010

/*
 * Define: TIMER16_TRIGGER_TIM0_GPIO0_14
 * Источник триггера - Timer16_0
 *
 */
#define TIMER16_TRIGGER_TIM0_GPIO0_14            0b011
/*
 * Define: TIMER16_TRIGGER_TIM1_GPIO1_6
 * Источник триггера - Timer16_1
 *
 */
#define TIMER16_TRIGGER_TIM1_GPIO1_6             0b011
/*
 * Define: TIMER16_TRIGGER_TIM2_GPIO2_0
 * Источник триггера - Timer16_2
 *
 */
#define TIMER16_TRIGGER_TIM2_GPIO2_0             0b011

/*
 * Define: TIMER16_TRIGGER_TEMPERATURE
 * Источник триггера - Термосенсор
 *
 */
#define TIMER16_TRIGGER_TEMPERATURE         0b100
/*
 * Define: TIMER16_TRIGGER_ADC
 * Источник триггера - Окончание преобразования АЦП
 *
 */
#define TIMER16_TRIGGER_ADC                 0b101
/*
 * Define: TIMER16_TRIGGER_RTC
 * Источник триггера - Прерывание RTC
 *
 */
#define TIMER16_TRIGGER_RTC                 0b110
/*
 * Define: TIMER16_TRIGGER_AlARM
 * Источник триггера - Будильник
 *
 */
#define TIMER16_TRIGGER_AlARM               0b111


/* Title: Выводы триггеров */
/*
 * Define: PORT_AS_GPIO
 * Вторая функция вывода (GPIO)
 *
 */
#define PORT_AS_GPIO    0b01


/* 
 * Title: Полярность синхронизации
 * Выбор активного фронта при тактировании от внешнего источника.
*/

/*
 * Define: TIMER16_TRIGGER_ACTIVEEDGE_SOFTWARE
 * Програмный тригер. Запускается при установленном CNTSTRT или SNGSTRT
 *
 */
#define TIMER16_TRIGGER_ACTIVEEDGE_SOFTWARE    0b00
/*
 * Define: TIMER16_TRIGGER_ACTIVEEDGE_RISING
 * Нарастающий фронт является активным фронтом
 *
 */
#define TIMER16_TRIGGER_ACTIVEEDGE_RISING      0b01
/*
 * Define: TIMER16_TRIGGER_ACTIVEEDGE_FOLLING
 * Спадающий фронт является активным фронтом
 *
 */
#define TIMER16_TRIGGER_ACTIVEEDGE_FOLLING     0b10
/*
 * Define: TIMER16_TRIGGER_ACTIVEEDGE_BOTH
 * Оба фронта являются активными фронтами
 *
 */
#define TIMER16_TRIGGER_ACTIVEEDGE_BOTH        0b11


/* Title: Функция тайм-аут */
/*
 * Define: TIMER16_TIMEOUT_DISABLE
 * Триггерное событие, поступаю-щее, когда таймер уже запущен, будет проигнорировано
 *
 */
#define TIMER16_TIMEOUT_DISABLE       0
/*
 * Define: TIMER16_TIMEOUT_ENABLE
 * Триггерное событие, поступающее, когда таймер уже запущен, сбросит и перезапустит счетчик
 *
 */
#define TIMER16_TIMEOUT_ENABLE        1


/* Title: Режим обновления регисторов ARR и CMP */
/*
 * Define: TIMER16_PRELOAD_AFTERWRITE
 * Регистры обновляются после каждого доступа к записи на шине APB
 *
 */
#define TIMER16_PRELOAD_AFTERWRITE    0
/*
 * Define: TIMER16_PRELOAD_ENDPERIOD
 * Регистры обновляются в конце текущего периода Timer16
 *
 */
#define TIMER16_PRELOAD_ENDPERIOD     1


/* Title: Фильтр для внешнего тактового генератора/триггера */
/*
 * Define: TIMER16_FILTER_NONE
 * Фильтр отключен
 *
 */
#define TIMER16_FILTER_NONE      0b00
/*
 * Define: TIMER16_FILTER_2CLOCK
 * Фильтр на 2 такта
 *
 */
#define TIMER16_FILTER_2CLOCK    0b01
/*
 * Define: TIMER16_FILTER_4CLOCK
 * Фильтр на 4 такта
 *
 */
#define TIMER16_FILTER_4CLOCK    0b10
/*
 * Define: TIMER16_FILTER_8CLOCK
 * Фильтр на 8 тактов
 *
 */
#define TIMER16_FILTER_8CLOCK    0b11


/* Title: Режим энкодера */
/*
 * Define: TIMER16_ENCODER_DISABLE
 * Режим энкодера выключен
 *
 */
#define TIMER16_ENCODER_DISABLE    0
/*
 * Define: TIMER16_ENCODER_ENABLE
 * Режим энкодера включен
 *
 */
#define TIMER16_ENCODER_ENABLE     1

/* Title: Структуры */
/*
 * Struct: Timer16_ClockConfigTypeDef
 * Настройки источника тактирования
 * 
 */
typedef struct
{
    /*
    * Variable: Source
    * Источник тактирования
    *
    */
   
    uint8_t Source;         
    /*
    * Variable: Prescaler
    * Делитель частоты
    *
    */
    uint8_t Prescaler;      

} Timer16_ClockConfigTypeDef;


/*
 * Struct: Timer16_TriggerConfigTypeDef
 * Настройки триггера
 * 
 */
typedef struct
{
    /*
    * Variable: Source
    * Источник триггера
    *
    */
    uint8_t Source;

    /*
    * Variable: ActiveEdge
    * Активный фронт
    *
    */
    uint8_t ActiveEdge;

    /*
    * Variable: TimeOut
    * Функция тайм-аут
    *
    */
    uint8_t TimeOut;

} Timer16_TriggerConfigTypeDef;


/*
 * Struct: Timer16_FilterConfigTypeDef
 * Настройки фильтров
 * 
 */
typedef struct
{
    /*
    * Variable: ExternalClock
    * Фильтр для внешнего тактового генератора
    *
    */
    uint8_t ExternalClock;

    /*
    * Variable: Trigger
    * фильтр для триггера
    *
    */
    uint8_t Trigger;

} Timer16_FilterConfigTypeDef;


/*
 * Struct: Timer16_HandleTypeDef
 * Настройки экземпляра Timer16
 * 
 */
typedef struct
{
    /*
    * Variable: Instance
    * Базовый адрес регистров Timer16
    * 
    * Возможные значения:
    * 
    * - TIMER16_0;
    * - TIMER16_1;
    * - TIMER16_2.
    *
    */
    TIMER16_TypeDef *Instance;

    /*
    * Variable: Clock
    * Настройки тактирования
    *
    */
    Timer16_ClockConfigTypeDef Clock;
    /*
    * Variable: CountMode
    * Источник синхронизации
    *
    */
    uint8_t CountMode;
    /*
    * Variable: ActiveEdge
    * Активный фронт
    *
    */
    uint8_t ActiveEdge;

    /*
    * Variable: Period
    * Верхнее значение счета
    *
    */
    uint16_t Period;
    /*
    * Variable: Preload
    * Режим записи в ARR и CMP
    *
    */
    uint8_t Preload;

    /*
    * Variable: Trigger
    * Настройки триггера
    *
    */
    Timer16_TriggerConfigTypeDef Trigger;

    /*
    * Variable: Filter
    * Настройки фильтра
    *
    */
    Timer16_FilterConfigTypeDef Filter;

    /*
    * Variable: EncoderMode
    * Режим энкодера
    *
    */
    uint8_t EncoderMode;
    
} Timer16_HandleTypeDef;

/* Title: Функции */
/*  
 * Function: HAL_Timer16_Disable
 * 
 * Выключить таймер
 * 
 * Может использоваться для отключения таймера или при записи в регистр CFGR.
 * 
 * Parameters:
 * 
 * htimer16 - Указатель на структуру с настройками Timer16.
 * 
 */
void HAL_Timer16_Disable(Timer16_HandleTypeDef *htimer16);

/*
 * Function: HAL_Timer16_Enable
 * Включить таймер
 *
 * Parameters:
 * htimer16 - Указатель на структуру с настройками Timer16.
 *
 * Returns:
 * void
 */
void HAL_Timer16_Enable(Timer16_HandleTypeDef *htimer16);

/*
 * Function: HAL_Timer16_SetActiveEdge
 * Установить активный фронт для подсчёта или задать подрежим энкодера.
 * 
 * Используется при тактировании Timer16 от внешнего источника тактового сигнала на выводе Input1.
 * 
 * Если оба фронта сконфигурированы как активные, необходимо также обеспечить внутренний тактовый сигнал. 
 * В этом случае частота внутреннего тактового сигнала должна быть как минимум в четыре раза выше частоты внешнего тактового сигнала.
 * При тактировании от Input1 ( HAL_Timer16_SetSourceClock ) счетчик Timer16 может обновляться либо по нарастающему, 
 * либо по спадающему фронту тактового сигнала lnput1, но не по двум (нарастающему и спадающему фронту) одновременно.
 * При использовании данной функции таймер выключается. Это необходимо для записи в регистр CFGR.
 *
 * Parameters:
 * htimer16 - Указатель на структуру с настройками Timer16.
 * ActiveEdge - Активный фронт. 
 * 
 * Возможные значения ActiveEdge:
 * 
 * - <TIMER16_ACTIVEEDGE_RISING>    Нарастающий фронт является активным. Подрежим энкодера 1;
 * - <TIMER16_ACTIVEEDGE_FOLLING>   Спадающий фронт является активным. Подрежим энкодера 2;
 * - <TIMER16_ACTIVEEDGE_BOTH>    Оба фронта являются активными фронтами. Подрежим энкодера 3.
 *
 * Returns:
 * void
 */
void HAL_Timer16_SetActiveEdge(Timer16_HandleTypeDef *htimer16, uint8_t ActiveEdge);

/*
 * Function: HAL_Timer16_SetSourceClock
 * Выбрать источник тактирования, который будет использовать Timer16.
 * 
 * При использовании данной функции таймер выключается. Это необходимо для записи в регистр CFGR.
 *
 * Parameters:
 * htimer16 - Указатель на структуру с настройками Timer16.
 * SourceClock - Источник тактирования.
 *
 * Returns:
 * void
 */
void HAL_Timer16_SetSourceClock(Timer16_HandleTypeDef *htimer16, uint8_t SourceClock);

/*
 * Function: HAL_Timer16_SetCountMode
 * Выбрать источник тактового сигнала для синхронизации счетчика Timer16.
 * 
 * При тактировании от Input1 Timer16 не нуждается во внутреннем источнике тактового сигнала 
 * (за исключением случаев, когда включены фильтры glitch). Сигнал, подаваемый на lnput1 Timer16, используется в качестве 
 * системного тактового генератора для Timer16. Эта конфигурация подходит для режимов работы, в которых не включен встроенный генератор. 
 * При такой конфигурации счетчик Timer16 может обновляться либо по нарастающему, либо по спадающему фронту тактового сигнала lnput1, 
 * но не по двум (нарастающему и спадающему фронту) одновременно.
 * 
 * При использовании данной функции таймер выключается. Это необходимо для записи в регистр CFGR.
 *
 * Parameters:
 * htimer16 - Указатель на структуру с настройками Timer16.
 * CountMode - Источник тактового сигнала для синхронизации счетчика.
 *
 * Returns:
 * void
 */
void HAL_Timer16_SetCountMode(Timer16_HandleTypeDef *htimer16, uint8_t CountMode);

/*
 * Function: HAL_Timer16_ClockInit
 * Инициализация тактирования в соответствии с параметрами Timer16_HandleTypeDef *htimer16.
 * 
 * При использовании данной функции таймер выключается. Это необходимо для записи в регистр CFGR.
 *
 * Parameters:
 * htimer16 - Указатель на структуру с настройками Timer16.
 *
 * Returns:
 * void
 */
void HAL_Timer16_ClockInit(Timer16_HandleTypeDef *htimer16);

/*
 * Function: HAL_Timer16_SetPreload
 * Задать режим обновления регистров ARR - значение автоматической перезагрузки и CMP - значение сравнения.
 * 
 * Для записи в ARR и CMP таймер должен быть включен.
 * 
 * При использовании данной функции таймер выключается. Это необходимо для записи в регистр CFGR.
 *
 * Parameters:
 * htimer16 - Указатель на структуру с настройками Timer16.
 * Preload - Режим обновления регистров ARR и CMP.
 *
 * Returns:
 * void
 */
void HAL_Timer16_SetPreload(Timer16_HandleTypeDef *htimer16, uint8_t Preload);
/*
 * Function: HAL_Timer16_WaitARROK
 * Ожидание флага ARROK, установка которого означает успешную запись в регистр ARR - значение автоматической перезагрузки.
 *
 * Parameters:
 * htimer16 - Указатель на структуру с настройками Timer16.
 *
 * Returns:
 * void
 */
void HAL_Timer16_WaitARROK(Timer16_HandleTypeDef *htimer16);
/*
 * Function: HAL_Timer16_WaitCMPOK
 * Ожидание флага CMPOK, установка которого означает успешную запись в регистр CMP - значение сравнения.
 *
 * Parameters:
 * htimer16 - Указатель на структуру с настройками Timer16.
 *
 * Returns:
 * void
 */
void HAL_Timer16_WaitCMPOK(Timer16_HandleTypeDef *htimer16);
/*
 * Function: HAL_Timer16_SetARR
 * Записать число в регистр ARR - значение автоматической перезагрузки.
 * 
 * Используется для установки верхнего предела, до которого будет считать счетчик Timer16.
 * 
 * Значение ARR должно быть всегда больше значения CMP.
 *
 * Parameters:
 * htimer16 - Указатель на структуру с настройками Timer16.
 * Period - 16 битное число в пределах от 0 до 65535.
 *
 * Returns:
 * void
 */
void HAL_Timer16_SetARR(Timer16_HandleTypeDef *htimer16, uint16_t Period);
/*
 * Function: HAL_Timer16_SetCMP
 * Записать число в регистр CMP - значение сравнения.
 * 
 * Используется для сравнения текущего значения счетчика в регистре CNT со значением в регистре CMP. 
 * При совпадении CNT и CMP устанавливается флаг CMPM.
 * 
 * В режиме генерации волновой формы при совпадении значений регистров CMP и CNT сигнал на выводе output меняет свое состояние.
 * Значение ARR должно быть всегда больше значения CMP.
 *
 * Parameters:
 * htimer16 - Указатель на структуру с настройками Timer16.
 * Compare - 16 битное число в пределах от 0 до 65534.
 *
 * Returns:
 * void
 */
void HAL_Timer16_SetCMP(Timer16_HandleTypeDef *htimer16, uint16_t Compare);

/*
 * Function: HAL_Timer16_ClearCMPFlag
 * Очистить флаг CMPM, который устанавливается при совпадении значений в регистрах CNT с CMP.
 *
 * Parameters:
 * htimer16 - Указатель на структуру с настройками Timer16.
 *
 * Returns:
 * void
 */
void HAL_Timer16_ClearCMPFlag(Timer16_HandleTypeDef *htimer16);

/*
 * Function: HAL_Timer16_SelectTrigger
 * Выбрать источник триггера.
 * 
 * Счетчик Timer16 может быть запущен либо программно, либо после обнаружения активного фронта импульса на одном из 8 триггерных входов.
 * 
 * При использовании данной функции таймер выключается. Это необходимо для записи в регистр CFGR.
 *
 * Parameters:
 * htimer16 - Указатель на структуру с настройками Timer16.
 * TriggerSource - Источник триггера.
 *
 * Returns:
 * void
 */
void HAL_Timer16_SelectTrigger(Timer16_HandleTypeDef *htimer16, uint8_t TriggerSource);

/*
 * Function: HAL_Timer16_SetTriggerEdge
 * Выбрать разрешение работы и активный фронт триггера.
 * 
 * Счетчик Timer16 может быть запущен либо программно, либо после обнаружения активного фронта импульса на одном из 8 триггерных входов.
 * 
 * При использовании данной функции таймер выключается. Это необходимо для записи в регистр CFGR.
 *
 * Parameters:
 * htimer16 - Указатель на структуру с настройками Timer16.
 * TriggerEdge - Активный фронт триггера.
 *
 * Returns:
 * void
 */
void HAL_Timer16_SetTriggerEdge(Timer16_HandleTypeDef *htimer16, uint8_t TriggerEdge);

/*
 * Function: HAL_Timer16_SetTimeOut
 * Включить или выключить функцию time-out.
 * 
 * С помощью включенной функции time-out активный фронт триггера может перезапустить отсчет таймера. Иначе повторное срабатывание триггера во время счета будет проигнорировано.
 * 
 * Может быть реализована функция time-out с низким энергопотреблением. Значение time-out соответствует значению сравнения - CMP. 
 * Если в течение ожидаемого периода времени триггер не срабатывает, MCU пробуждается по событию совпадения сравнения.
 * 
 * При использовании данной функции таймер выключается. Это необходимо для записи в регистр CFGR.
 *
 * Parameters:
 * htimer16 - Указатель на структуру с настройками Timer16.
 * TimeOut - Режим time-out.
 *
 * Returns:
 * void
 */
void HAL_Timer16_SetTimeOut(Timer16_HandleTypeDef *htimer16, uint8_t TimeOut);

/*
 * Function: HAL_Timer16_SetFilterExternalClock
 * Задать чувствительность фильтра для внешнего тактового генератора.
 * 
 * Прежде чем активировать цифровые фильтры, на Timer16 сначала должен быть подан внутренний источник синхронизации.
 * В случае отсутствия внутреннего тактового сигнала цифровой фильтр должен быть деактивирован.
 * 
 * При использовании данной функции таймер выключается. Это необходимо для записи в регистр CFGR.
 *
 * Parameters:
 * htimer16 - Указатель на структуру с настройками Timer16.
 * FilterExternalClock - Чувствительность фильтра.
 *
 * Returns:
 * void
 */
void HAL_Timer16_SetFilterExternalClock(Timer16_HandleTypeDef *htimer16, uint8_t FilterExternalClock);

/*
 * Function: HAL_Timer16_SetFilterTrigger
 * Задать чувствительность фильтра для триггера.
 * 
 * Прежде чем активировать цифровые фильтры, на LPTIM сначала должен быть подан внутренний источник синхронизации.
 * В случае отсутствия внутреннего тактового сигнала цифровой фильтр должен быть деактивирован.
 * 
 * При использовании данной функции таймер выключается. Это необходимо для записи в регистр CFGR.
 *
 * Parameters:
 * htimer16 - Указатель на структуру с настройками Timer16.
 * FilterTrigger - Чувствительность фильтра.
 *
 * Returns:
 * void
 */
void HAL_Timer16_SetFilterTrigger(Timer16_HandleTypeDef *htimer16, uint8_t FilterTrigger);

/*
 * Function: HAL_Timer16_SetEncoderMode
 * Включить или выключить режим энкодера.
 * 
 * Режим энкодера доступен только в том случае, если Timer16 работает от внутреннего источника синхронизации. 
 * Частота сигналов на обоих входах lnput1 и lnput2 не должна превышать частоту внутреннего тактового генератора Timer16 , деленную на 4. 
 * Кроме того, коэффициент деления предделителя должен быть равен его начальному значению, которое равно 1. 
 * Это обязательно для обеспечения нормальной работы Timer16.
 * 
 * При использовании данной функции таймер выключается. Это необходимо для записи в регистр CFGR.
 *
 * Parameters:
 * htimer16 - Указатель на структуру с настройками Timer16.
 * EncoderMode - Режим энкодера.
 *
 * Returns:
 * void
 */
void HAL_Timer16_SetEncoderMode(Timer16_HandleTypeDef *htimer16, uint8_t EncoderMode);


/*
 * Function: HAL_Timer16_SetPrescaler
 * Установить делитель частоты.
 *
 * Parameters:
 * htimer16 - Указатель на структуру с настройками Timer16.
 * Prescaler - Делитель чатоты.
 *
 * Returns:
 * void
 */
void HAL_Timer16_SetPrescaler(Timer16_HandleTypeDef *htimer16, uint8_t Prescaler);

/*
 * Function: HAL_Timer16_Init
 * Инициализировать Timer16 в соответствии с настройками <Timer16_HandleTypeDef> *htimer16.
 *
 * Parameters:
 * htimer16 - Указатель на структуру с настройками Timer16.
 *
 * Returns:
 * void
 */
void HAL_Timer16_Init(Timer16_HandleTypeDef *htimer16);

/*
 * Function: HAL_Timer16_GetCounterValue
 * Получить текущее значение счетчика из регистра CNT.
 *
 * Parameters:
 * htimer16 - Указатель на структуру с настройками Timer16.
 *
 * Returns:
 * (uint16_t ) - Текущее значение счетчика CNT
 *
 */
uint16_t HAL_Timer16_GetCounterValue(Timer16_HandleTypeDef *htimer16);

/*
 * Function: HAL_Timer16_CheckCMP
 * Проверить состояние флага сравнения CMPM.
 *
 * Parameters:
 * htimer16 - Указатель на структуру с настройками Timer16.
 *
 * Returns:
 * (uint8_t ) - Текущее состояние флага CMPM.
 *
 */
uint8_t HAL_Timer16_CheckCMP(Timer16_HandleTypeDef *htimer16);

/*
 * Function: HAL_Timer16_WaitCMP
 * Ожидать когда счетчик достигнет значения сравнения CMP.
 *
 * Parameters:
 * htimer16 - Указатель на структуру с настройками Timer16.
 *
 * Returns:
 * void
 */
void HAL_Timer16_WaitCMP(Timer16_HandleTypeDef *htimer16);

/*
 * Function: HAL_Timer16_StartSingleMode
 * Запустить таймер в одиночном режиме.
 * 
 * Счетчик будет считать от 0 до значения в регистре ARR, а затем остановится.
 *
 * Parameters:
 * htimer16 - Указатель на структуру с настройками Timer16.
 *
 * Returns:
 * void
 */
void HAL_Timer16_StartSingleMode(Timer16_HandleTypeDef *htimer16);

/*
 * Function: HAL_Timer16_StartContinuousMode
 * Запустить таймер в непрерывном режиме.
 * 
 * Счетчик будет считать от 0 до значения в регистре ARR, а затем начнет заново.
 *
 * Parameters:
 * htimer16 - Указатель на структуру с настройками Timer16.
 *
 * Returns:
 * void
 */
void HAL_Timer16_StartContinuousMode(Timer16_HandleTypeDef *htimer16);

/*
 * Function: HAL_Timer16_InvertOutput
 * Инвертировать сигнал на выводе Output.
 * 
 * Используется при генерации волновой формы.
 *
 * Parameters:
 * htimer16 - Указатель на структуру с настройками Timer16.
 *
 * Returns:
 * void
 */
void HAL_Timer16_InvertOutput(Timer16_HandleTypeDef *htimer16);

/*
 * Function: HAL_Timer16_StartPWM
 * Запустить таймер с ШИМ сигналом.
 * 
 * Счетчик будет считать от 0 до значения в регистре ARR, а затем начнет заново. При достижении значения CMP сигнал на выводе Output 
 * сменит свое состояние. При достижении значения ARR сигнал на выводе Output вернется в исходное состояние.
 *
 * Parameters:
 * htimer16 - Указатель на структуру с настройками Timer16.
 * Period - Период ШИМ сигнала. Число от 0 до 65535;
 * Compare - Значение при достижении которого сигнал на выводе Output сменит свое состояние. 
 * Число от 0 до 65534; Данное число всегда должно быть меньше значения Period.
 *
 * Returns:
 * void
 */
void HAL_Timer16_StartPWM(Timer16_HandleTypeDef *htimer16, uint16_t Period, uint16_t Compare);

/*
 * Function: HAL_Timer16_StartOneShot
 * Запустить таймер в одноимпульсном режиме.
 * 
 * Счетчик будет считать от 0 до значения в регистре ARR. При достижении значения CMP сигнал на выводе Output сменит свое состояние. При достижении значения ARR сигнал на выводе Output вернется в исходное состояние.
 *
 * Parameters:
 * htimer16 - Указатель на структуру с настройками Timer16.
 * Period - Период ШИМ сигнала. Число от 0 до 65535;
 * Compare - Значение при достижении которого сигнал на выводе Output сменит свое состояние. 
 * Число от 0 до 65534; Данное число всегда должно быть меньше значения Period.
 *
 * Returns:
 * void
 */
void HAL_Timer16_StartOneShot(Timer16_HandleTypeDef *htimer16, uint16_t Period, uint16_t Compare);

/*
 * Function: HAL_Timer16_StartSetOnes
 * Запустить таймер в однократном режиме.
 * 
 * При достижении значения CMP сигнал на выводе Output сменит свое состояние.
 *
 * Parameters:
 * htimer16 - Указатель на структуру с настройками Timer16.
 * Period - Период ШИМ сигнала. Число от 0 до 65535;
 * Compare - Значение при достижении которого сигнал на выводе Output сменит свое состояние. 
 * Число от 0 до 65534; Данное число всегда должно быть меньше значения Period.
 *
 * Returns:
 * void
 */
void HAL_Timer16_StartSetOnes(Timer16_HandleTypeDef *htimer16, uint16_t Period, uint16_t Compare);

/*
 * Function: HAL_Timer16_ClearTriggerFlag
 * Очистить флаг триггера - EXTTRIG.
 *
 * Parameters:
 * htimer16 - Указатель на структуру с настройками Timer16.
 *
 * Returns:
 * void
 */
void HAL_Timer16_ClearTriggerFlag(Timer16_HandleTypeDef *htimer16);

/*
 * Function: HAL_Timer16_WaitTrigger
 * TОжидать флаг триггера - EXTTRIG.
 *
 * Parameters:
 * htimer16 - Указатель на структуру с настройками Timer16.
 *
 * Returns:
 * void
 */
void HAL_Timer16_WaitTrigger(Timer16_HandleTypeDef *htimer16);

/*
 * Function: HAL_Timer16_ClearUpFlag
 * Очистить флаг смены направления счета - UP.
 *
 * Parameters:
 * htimer16 - Указатель на структуру с настройками Timer16.
 *
 * Returns:
 * void
 */
void HAL_Timer16_ClearUpFlag(Timer16_HandleTypeDef *htimer16);

/*
 * Function: HAL_Timer16_ClearDownFlag
 * Очистить флаг смены направления счета - DOWN.
 *
 * Parameters:
 * htimer16 - Указатель на структуру с настройками Timer16.
 *
 * Returns:
 * void.
 */
void HAL_Timer16_ClearDownFlag(Timer16_HandleTypeDef *htimer16);


#endif