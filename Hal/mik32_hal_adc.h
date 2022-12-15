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

/* Sel - Выбор канала */
#define ADC_CHANNEL0        0       /* Канал АЦП0 - PORT_1_5 */
#define ADC_CHANNEL1        1       /* Канал АЦП1 - PORT_1_7 */
#define ADC_CHANNEL2        2       /* Канал АЦП2 - PORT_0_2 */
#define ADC_CHANNEL3        3       /* Канал АЦП3 - PORT_0_4 */
#define ADC_CHANNEL4        4       /* Канал АЦП4 - PORT_0_7 */
#define ADC_CHANNEL5        5       /* Канал АЦП5 - PORT_0_9 */
#define ADC_CHANNEL6        6       /* Канал АЦП6 - PORT_0_11 */
#define ADC_CHANNEL7        7       /* Канал АЦП7 - PORT_0_13 */

/* EXTRef - Выбор источника опорного напряжения */
#define ADC_EXTREF_OFF      0       /* Встроенный источник опорного напряжения 1,2 В */
#define ADC_EXTREF_ON       1       /* Внешний источник опорного напряжения */

/* EXTClb - Выбор источника внешнего опорного напряжения */
#define ADC_EXTCLB_CLBREF       0       /* Настраиваемый ОИН */
#define ADC_EXTCLB_ADCREF      1       /* Внешний вывод */

/* Вход АЦП */
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

typedef struct
{
    uint8_t Sel;            /* Выбор канала АЦП */

    uint8_t EXTRef;         /* Выбор источника опорного напряжения */ 

    uint8_t EXTClb;         /* Выбор внешнего источника опорного напряжения */

} ADC_InitTypeDef;

typedef struct
{
    ANALOG_REG_TypeDef *Instance;       /* Базоый адрес регистров ADC */

    ADC_InitTypeDef Init;               /* Параметры ADC */
    
} ADC_HandleTypeDef;


void HAL_ADC_CLBEnable(ADC_HandleTypeDef *hadc);
void HAL_ADC_CLBDisable(ADC_HandleTypeDef *hadc);
void HAL_ADC_VCLBSet(ADC_HandleTypeDef *hadc, uint8_t v_coef);
void HAL_ADC_ICLBSet(ADC_HandleTypeDef *hadc, uint8_t i_coef);
void HAL_ADC_ResetEnable(ADC_HandleTypeDef *hadc);
void HAL_ADC_ResetDisable(ADC_HandleTypeDef *hadc);
void HAL_ADC_Disable(ADC_HandleTypeDef *hadc);
void HAL_ADC_Enable(ADC_HandleTypeDef *hadc);
void HAL_ADC_ChannelSet(ADC_HandleTypeDef *hadc);
void HAL_ADC_Init(ADC_HandleTypeDef *hadc);
void HAL_ADC_Single(ADC_HandleTypeDef *hadc);
void HAL_ADC_ContiniusDisabled(ADC_HandleTypeDef *hadc);
void HAL_ADC_ContiniusEnable(ADC_HandleTypeDef *hadc);
void HAL_ADC_WaitValid(ADC_HandleTypeDef *hadc);
uint16_t HAL_ADC_WaitAndGetValue(ADC_HandleTypeDef *hadc);
uint16_t HAL_ADC_GetValue(ADC_HandleTypeDef *hadc);

#endif