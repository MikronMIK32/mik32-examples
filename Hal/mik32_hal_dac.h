#ifndef MIK32_HAL_DAC
#define MIK32_HAL_DAC

#include "def_list.h"
#include "mcu32_memory_map.h"
#include "analog_reg.h"
#include "pad_config.h"
#include "stdbool.h"

#ifdef MIK32_DAC_DEBUG
#include "common.h"
#endif

/* Title: Макросы */

/*
 * Defines: Каналы ЦАП
 * 
 * HAL_DAC1 - Канал ЦАП1 (PORT_1_12)
 * HAL_DAC2 - Канал ЦАП2 (PORT_1_13) 
 *
 */
#define HAL_DAC1      &(ANALOG_REG->DAC0)       /* Канал DAC1 - PORT_1_12 */
#define HAL_DAC2      &(ANALOG_REG->DAC1)       /* Канал DAC2 - PORT_1_13 */

/*
 * Defines: Выбор ИОН
 *
 * Выбор источника опорного напряжения.
 *
 * DAC_EXTREF_OFF - Встроенный источник опорного напряжения 1,2 В
 * DAC_EXTREF_ON - Внешний источник опорного напряжения
 *
 */
#define DAC_EXTREF_OFF      0       /* Встроенный источник опорного напряжения 1,2 В */
#define DAC_EXTREF_ON       1       /* Внешний источник опорного напряжения */

/*
 * Defines: Выбор внешнего ИОН
 *
 * Выбор внешнего источника опорного напряжения по отношению к встроенному в АЦП ИОН.
 *
 * DAC_EXTCLB_CLBREF - Настраиваемый ОИН
 * DAC_EXTCLB_DACREF - Внешний вывод
 *
 */
#define DAC_EXTCLB_CLBREF       0       /* Настраиваемый ОИН */
#define DAC_EXTCLB_DACREF       1       /* Внешний вывод */

/* Выводы ЦАП */
/*
 * Defines: Входы АЦП
 *
 * Номера выводов каналов ЦАП и DAC_REF
 *
 * DAC_PORT_AS_FUNC3 - Четвертая функция вывода (аналоговый сигнал)
 * DAC1_PORT_1_12  - ЦАП1 - PORT_1_12 
 * DAC2_PORT_1_13  - ЦАП2 - PORT_1_13
 * DAC_REF_PORT_1_11  - Внешний опорный сигнал DAC_REF - PORT_1_11
 *
 */
#define DAC_PORT_AS_FUNC3       0b11    /* Четвертая функция вывода (аналоговый сигнал) */ 
#define DAC1_PORT_1_12          12      /* DAC1 - PORT_1_12 */
#define DAC2_PORT_1_13          13      /* DAC2 - PORT_1_13 */
#define DAC_REF_PORT_1_11       11      /* Внешний опорный сигнал - PORT_1_11 */

/* Title: Структуры */

/*
 * Struct: DAC_InitTypeDef
 * 
 * Настройки ЦАП при инициализации
 * 
 */
typedef struct
{
    /*
    * Variable: EXTRef
    * Выбор источника опорного напряжения
    * 
    * Этот параметр должен быть одним из значений:
    * 
    * - <DAC_EXTREF_OFF>;
    * - <DAC_EXTREF_ON>.
    *
    */
    uint8_t EXTRef;

    /*
    * Variable: EXTClb
    * Выбор внешнего источника опорного напряжения
    * 
    * Этот параметр должен быть одним из значений:
    * 
    * - <DAC_EXTCLB_CLBREF>;
    * - <DAC_EXTCLB_DACREF>.
    *
    */
    uint8_t EXTClb;

    /*
    * Variable: DIV
    * Значение делителя тактового сигнала.
    * 
    * Частота определяется как F_ЦАП = F_IN/(Div+1). 
    * 
    * Значением может быть 8-битное число в пределах от 0 до 255
    *
    */
    uint8_t DIV; 

} DAC_InitTypeDef;

/*
 * Struct: DAC_HandleTypeDef
 * 
 * Настройки ЦАП
 * 
 */
typedef struct
{
    /*
    * Variable: Instance
    * Базоый адрес регистров ANALOG_REG.
    *
    */
    ANALOG_REG_TypeDef *Instance;

    /*
    * Variable: Instance_dac
    * Базоый адрес регистров DAC
    *
    */
    volatile DAC_TypeDef *Instance_dac;

    /*
    * Variable: Init
    * Настройки ЦАП при инициализации
    *
    */
    DAC_InitTypeDef Init;
    
} DAC_HandleTypeDef;


void HAL_DAC_CLBEnable(DAC_HandleTypeDef *hdac);
void HAL_DAC_CLBDisable(DAC_HandleTypeDef *hdac);
void HAL_DAC_VCLBSet(DAC_HandleTypeDef *hdac, uint8_t v_coef);
void HAL_DAC_ICLBSet(DAC_HandleTypeDef *hdac, uint8_t i_coef);

/* Title: Функции */

/*
 * Function: HAL_DAC_SetDiv
 * Установить значение делителя тактового сигнала ЦАП.
 * 
 * Частота определяется как F_ЦАП = F_IN/(Div+1). 
 * 
 * Значением может быть 8-битное число в пределах от 0 до 255
 *
 * Parameters:
 * hdac - Указатель на структуру с настройками ЦАП.
 * div - Делитель частоты.
 *
 * Returns:
 * void
 */
void HAL_DAC_SetDiv(DAC_HandleTypeDef *hdac, uint8_t div);

/*
 * Function: HAL_DAC_ResetEnable
 * Установить бит Rn, управление сбросом ЦАП, в 1.
 *
 * Parameters:
 * hdac - Указатель на структуру с настройками ЦАП.
 *
 * Returns:
 * void
 */
void HAL_DAC_ResetEnable(DAC_HandleTypeDef *hdac);

/*
 * Function: HAL_DAC_ResetDisable
 * Сбросить бит Rn, управление сбросом ЦАП, в 0.
 *
 * Parameters:
 * hdac - Указатель на структуру с настройками ЦАП.
 *
 * Returns:
 * void
 */
void HAL_DAC_ResetDisable(DAC_HandleTypeDef *hdac);

/*
 * Function: HAL_DAC_Disable
 * Выключть питание ЦАП
 *
 * Parameters:
 * hdac - Указатель на структуру с настройками ЦАП.
 *
 * Returns:
 * void
 */
void HAL_DAC_Disable(DAC_HandleTypeDef *hdac);

/*
 * Function: HAL_ADC_Enable
 * Включть питание ЦАП
 *
 * Parameters:
 * hdac - Указатель на структуру с настройками ЦАП.
 *
 * Returns:
 * void
 */
void HAL_DAC_Enable(DAC_HandleTypeDef *hdac);

/*
 * Function: HAL_DAC_Init
 * Инициализировать ЦАП в соответсвии с настройками в hdac.
 *
 * Parameters:
 * hdac - Указатель на структуру с настройками ЦАП.
 *
 * Returns:
 * void
 */
void HAL_DAC_Init(DAC_HandleTypeDef *hdac);

/*
 * Function: HAL_DAC_SetValue
 * Задать входные данные для преобразования ЦАП.
 * 
 * Входные данные могут быть в пределах от 0 до 4 095.
 *
 * Parameters:
 * hdac - Указатель на структуру с настройками ЦАП.
 * DAC_Value - Входные данные для преобразования ЦАП.
 *
 * Returns:
 * void
 * 
 */
void HAL_DAC_SetValue(DAC_HandleTypeDef *hdac, uint16_t DAC_Value);

#endif