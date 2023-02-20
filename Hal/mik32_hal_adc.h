#ifndef MIK32_HAL_ADC
#define MIK32_HAL_ADC

#include "def_list.h"
#include "mcu32_memory_map.h"
#include "analog_reg.h"
#include "pad_config.h"
#include "stdbool.h"

#ifdef MIK32_ADC_DEBUG
#include "common.h"
#endif

/* Title: Макросы */

/*
 * Defines: Каналы АЦП
 * 
 * ADC_CHANNEL0 - Канал АЦП0 (PORT_1_5)
 * ADC_CHANNEL1 - Канал АЦП1 (PORT_1_7)
 * ADC_CHANNEL2 - Канал АЦП2 (PORT_0_2)
 * ADC_CHANNEL3 - Канал АЦП3 (PORT_0_4)
 * ADC_CHANNEL4 - Канал АЦП4 (PORT_0_7)
 * ADC_CHANNEL5 - Канал АЦП5 (PORT_0_9)
 * ADC_CHANNEL6 - Канал АЦП6 (PORT_0_11)
 * ADC_CHANNEL7 - Канал АЦП7 (PORT_0_13)    
 *
 */
#define ADC_CHANNEL0        0       /* Канал АЦП0 - PORT_1_5 */
#define ADC_CHANNEL1        1       /* Канал АЦП1 - PORT_1_7 */
#define ADC_CHANNEL2        2       /* Канал АЦП2 - PORT_0_2 */
#define ADC_CHANNEL3        3       /* Канал АЦП3 - PORT_0_4 */
#define ADC_CHANNEL4        4       /* Канал АЦП4 - PORT_0_7 */
#define ADC_CHANNEL5        5       /* Канал АЦП5 - PORT_0_9 */
#define ADC_CHANNEL6        6       /* Канал АЦП6 - PORT_0_11 */
#define ADC_CHANNEL7        7       /* Канал АЦП7 - PORT_0_13 */

/*
 * Defines: Выбор ИОН
 *
 * Выбор источника опорного напряжения.
 *
 * ADC_EXTREF_OFF - Встроенный источник опорного напряжения 1,2 В
 * ADC_EXTREF_ON - Внешний источник опорного напряжения
 *
 */
#define ADC_EXTREF_OFF      0       /* Встроенный источник опорного напряжения 1,2 В */
#define ADC_EXTREF_ON       1       /* Внешний источник опорного напряжения */

/*
 * Defines: Выбор внешнего ИОН
 *
 * Выбор внешнего источника опорного напряжения по отношению к встроенному в АЦП ИОН.
 *
 * ADC_EXTCLB_CLBREF - Настраиваемый ОИН
 * ADC_EXTCLB_ADCREF - Внешний вывод
 *
 */
#define ADC_EXTCLB_CLBREF      0       /* Настраиваемый ОИН */
#define ADC_EXTCLB_ADCREF      1       /* Внешний вывод */

/*
 * Defines: Входы АЦП
 *
 * Номера выводов каналов АЦП и ADC_REF
 *
 * ADC_PORT_AS_FUNC3 - Четвертая функция вывода (аналоговый сигнал)
 * ADC_CHANNEL0_PORT_1_5  - Канал АЦП0 - PORT_1_5 
 * ADC_CHANNEL1_PORT_1_7  - Канал АЦП1 - PORT_1_7 
 * ADC_CHANNEL2_PORT_0_2  - Канал АЦП2 - PORT_0_2 
 * ADC_CHANNEL3_PORT_0_4  - Канал АЦП3 - PORT_0_4 
 * ADC_CHANNEL4_PORT_0_7  - Канал АЦП4 - PORT_0_7 
 * ADC_CHANNEL5_PORT_0_9  - Канал АЦП5 - PORT_0_9 
 * ADC_CHANNEL6_PORT_0_11 - Канал АЦП6 - PORT_0_11
 * ADC_CHANNEL7_PORT_0_13 - Канал АЦП7 - PORT_0_13
 * ADC_REF_PORT_1_10      - Внешний опорный сигнал - PORT_1_10
 *
 */
#define ADC_PORT_AS_FUNC3             0b11    /* Четвертая функция вывода (аналоговый сигнал) */ 
#define ADC_CHANNEL0_PORT_1_5         5       /* Канал АЦП0 - PORT_1_5 */
#define ADC_CHANNEL1_PORT_1_7         7       /* Канал АЦП1 - PORT_1_7 */
#define ADC_CHANNEL2_PORT_0_2         2       /* Канал АЦП2 - PORT_0_2 */
#define ADC_CHANNEL3_PORT_0_4         4       /* Канал АЦП3 - PORT_0_4 */
#define ADC_CHANNEL4_PORT_0_7         7       /* Канал АЦП4 - PORT_0_7 */
#define ADC_CHANNEL5_PORT_0_9         9       /* Канал АЦП5 - PORT_0_9 */
#define ADC_CHANNEL6_PORT_0_11        11      /* Канал АЦП6 - PORT_0_11 */
#define ADC_CHANNEL7_PORT_0_13        13      /* Канал АЦП7 - PORT_0_13 */
#define ADC_REF_PORT_1_10             10      /* Внешний опорный сигнал - PORT_1_10 */


/* Title: Структуры */

/*
 * Struct: ADC_InitTypeDef
 * 
 * Настройки АЦП при инициализации
 * 
 */
typedef struct
{
    /*
    * Variable: Sel
    * Выбор канала АЦП
    * 
    * Этот параметр должен быть одним из значений:
    * 
    * - <ADC_CHANNEL0>;
    * - <ADC_CHANNEL1>;
    * - <ADC_CHANNEL2>;
    * - <ADC_CHANNEL3>;
    * - <ADC_CHANNEL4>;
    * - <ADC_CHANNEL5>;
    * - <ADC_CHANNEL6>;
    * - <ADC_CHANNEL7>.
    *
    */
    uint8_t Sel;

    /*
    * Variable: EXTRef
    * Выбор источника опорного напряжения
    * 
    * Этот параметр должен быть одним из значений:
    * 
    * - <ADC_EXTREF_OFF>;
    * - <ADC_EXTREF_ON>.
    *
    */
    uint8_t EXTRef;

    /*
    * Variable: EXTClb
    * Выбор внешнего источника опорного напряжения
    * 
    * Этот параметр должен быть одним из значений:
    * 
    * - <ADC_EXTCLB_CLBREF>;
    * - <ADC_EXTCLB_ADCREF>.
    *
    */
    uint8_t EXTClb;

} ADC_InitTypeDef;

/*
 * Struct: ADC_HandleTypeDef
 * Настройки АЦП
 * 
 */
typedef struct
{
    /*
    * Variable: Instance
    * Базоый адрес регистров ADC.
    *
    */
    ANALOG_REG_TypeDef *Instance;

    /*
    * Variable: Init
    * Настройки АЦП при инициализации
    *
    */
    ADC_InitTypeDef Init;
    
} ADC_HandleTypeDef;


/*
 * Включить калибруемый ИОН
 */
void HAL_ADC_CLBEnable(ADC_HandleTypeDef *hadc);

/*
 * Выключить калибруемый ИОН
 */
void HAL_ADC_CLBDisable(ADC_HandleTypeDef *hadc);

/*
 * Задать коэфициент настройки ОИН
 */
void HAL_ADC_VCLBSet(ADC_HandleTypeDef *hadc, uint8_t v_coef);

/*
 * Задать коэфициент настройки опорного источника тока
 */
void HAL_ADC_ICLBSet(ADC_HandleTypeDef *hadc, uint8_t i_coef);

/* Title: Функции */

/*
 * Function: HAL_ADC_ResetEnable
 * Установить бит Rn, управление сбросом АЦП, в 1.
 *
 * Parameters:
 * hadc - Указатель на структуру с настройками ADC.
 *
 * Returns:
 * void
 */
void HAL_ADC_ResetEnable(ADC_HandleTypeDef *hadc);

/*
 * Function: HAL_ADC_ResetDisable
 * Сбросить бит Rn, управление сбросом АЦП, в 0.
 *
 * Parameters:
 * hadc - Указатель на структуру с настройками ADC.
 *
 * Returns:
 * void
 */
void HAL_ADC_ResetDisable(ADC_HandleTypeDef *hadc);

/*
 * Function: HAL_ADC_Disable
 * Выключть питание АЦП
 *
 * Parameters:
 * hadc - Указатель на структуру с настройками ADC.
 *
 * Returns:
 * void
 */
void HAL_ADC_Disable(ADC_HandleTypeDef *hadc);

/*
 * Function: HAL_ADC_Enable
 * Включть питание АЦП
 *
 * Parameters:
 * hadc - Указатель на структуру с настройками ADC.
 *
 * Returns:
 * void
 */
void HAL_ADC_Enable(ADC_HandleTypeDef *hadc);

/*
 * Function: HAL_ADC_ChannelSet
 * Задать активный канал АЦП и перевести соответсвующий вывод в аналоговую функцию.
 *
 * Parameters:
 * hadc - Указатель на структуру с настройками ADC.
 *
 * Returns:
 * void
 */
void HAL_ADC_ChannelSet(ADC_HandleTypeDef *hadc);

/*
 * Function: HAL_ADC_Init
 * Инициализировать АЦП в соответсвии с настройками в hadc.
 *
 * Parameters:
 * hadc - Указатель на структуру с настройками ADC.
 *
 * Returns:
 * void
 */
void HAL_ADC_Init(ADC_HandleTypeDef *hadc);

/*
 * Function: HAL_ADC_Single
 * Запустить однократное измерение АЦП.
 *
 * Parameters:
 * hadc - Указатель на структуру с настройками ADC.
 *
 * Returns:
 * void.
 */
void HAL_ADC_Single(ADC_HandleTypeDef *hadc);

/*
 * Function: HAL_ADC_ContiniusDisabled
 * Выключить непрерывное измерение АЦП.
 *
 * Parameters:
 * hadc - Указатель на структуру с настройками ADC.
 *
 * Returns:
 * void
 */
void HAL_ADC_ContinuousDisabled(ADC_HandleTypeDef *hadc);

/*
 * Function: HAL_ADC_ContiniusEnable
 * Выключить непрерывное измерение АЦП.
 *
 * Parameters:
 * hadc - Указатель на структуру с настройками ADC.
 *
 * Returns:
 * void
 */
void HAL_ADC_ContinuousEnable(ADC_HandleTypeDef *hadc);

/*
 * Function: HAL_ADC_WaitValid
 * Ожидать установку флага актуальных данных.
 * 
 * Флаг VALID - Признак наличия актуальных данных.
 *
 * Parameters:
 * hadc - Указатель на структуру с настройками ADC.
 *
 * Returns:
 * void
 */
void HAL_ADC_WaitValid(ADC_HandleTypeDef *hadc);

/*
 * Function: HAL_ADC_GetValue
 * Получить результат преобразовния АЦП.
 *
 * Parameters:
 * hadc - Указатель на структуру с настройками ADC.
 *
 * Returns:
 * (uint16_t ) - Результат преобразования АЦП.
 *
 */
uint16_t HAL_ADC_GetValue(ADC_HandleTypeDef *hadc);

/*
 * Function: HAL_ADC_WaitAndGetValue
 * Ожидать актуальный результат преобразования АЦП и получить его значение.
 *
 * Parameters:
 * hadc - Указатель на структуру с настройками ADC.
 *
 * Returns:
 * (uint16_t ) - Результат преобразования АЦП.
 *
 */
uint16_t HAL_ADC_WaitAndGetValue(ADC_HandleTypeDef *hadc);

#endif