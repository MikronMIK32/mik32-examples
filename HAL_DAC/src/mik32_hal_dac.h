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

/* Выбор ЦАП */
#define HAL_DAC1      &(ANALOG_REG->DAC0)       /* DAC1 - PORT_1_12 */
#define HAL_DAC2      &(ANALOG_REG->DAC1)       /* DAC2 - PORT_1_13 */

/* EXTRef - Выбор источника опорного напряжения */
#define DAC_EXTREF_OFF      0       /* Встроенный источник опорного напряжения 1,2 В */
#define DAC_EXTREF_ON       1       /* Внешний источник опорного напряжения */

/* EXTClb - Выбор источника внешнего опорного напряжения */
#define DAC_EXTCLB_CLBREF       0       /* Настраиваемый ОИН */
#define DAC_EXTCLB_DACREF       1       /* Внешний вывод */

/* Выводы ЦАП */
#define DAC_PORT_AS_FUNC3       0b11    /* Четвертая функция вывода (аналоговый сигнал) */ 
#define DAC1_PORT_1_12          12      /* DAC1 - PORT_1_12 */
#define DAC2_PORT_1_13          13      /* DAC2 - PORT_1_13 */
#define DAC_REF_PORT_1_11       11      /* Внешний опорный сигнал - PORT_1_11 */

typedef struct
{
    uint8_t EXTRef;                     /* Выбор источника опорного напряжения */ 

    uint8_t EXTClb;                     /* Выбор внешнего источника опорного напряжения */

    uint8_t DIV;                        /* Значение делителя тактового сигнала. Частота определяется как T_ЦАП = F_IN/(Div+1). Значением может быть 7-битное число */

} DAC_InitTypeDef;

typedef struct
{
    ANALOG_REG_TypeDef *Instance;       /* Базоый адрес регистров ANALOG_REG */

    volatile DAC_TypeDef *Instance_dac; /* Базоый адрес регистров DAC */

    DAC_InitTypeDef Init;               /* Параметры DAC */
    
} DAC_HandleTypeDef;


void HAL_DAC_CLBEnable(DAC_HandleTypeDef *hdac);
void HAL_DAC_CLBDisable(DAC_HandleTypeDef *hdac);
void HAL_DAC_VCLBSet(DAC_HandleTypeDef *hdac, uint8_t v_coef);
void HAL_DAC_ICLBSet(DAC_HandleTypeDef *hdac, uint8_t i_coef);
void HAL_DAC_SetDiv(DAC_HandleTypeDef *hdac, uint8_t div);
void HAL_DAC_ResetEnable(DAC_HandleTypeDef *hdac);
void HAL_DAC_ResetDisable(DAC_HandleTypeDef *hdac);
void HAL_DAC_Disable(DAC_HandleTypeDef *hdac);
void HAL_DAC_Enable(DAC_HandleTypeDef *hdac);
void HAL_DAC_Init(DAC_HandleTypeDef *hdac);
void HAL_DAC_SetValue(DAC_HandleTypeDef *hdac, uint16_t DAC_Value);

#endif