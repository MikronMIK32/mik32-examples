#ifndef MIK32_HAL_OTP
#define MIK32_HAL_OTP

#include "def_list.h"
#include "mcu32_memory_map.h"
#include "power_manager.h"
#include "otp.h"
#include "pad_config.h"
#include "stdbool.h"

#ifdef MIK32_OTP_DEBUG
#include "common.h"
#endif


#define MIK32_FREQ          32000000      /* Входная частота, Гц */
#define MIK32_FREQ_MHZ      32            /* Входная частота, МГц */


typedef struct
{
    OTP_TypeDef *Instance;       /* Базоый адрес регистров OTP */
    
} OTP_HandleTypeDef;


void HAL_OPT_TimeInit(OTP_HandleTypeDef *hotp);
void HAL_OTP_Init(OTP_HandleTypeDef *hotp);
void HAL_OTP_WaitBSY(OTP_HandleTypeDef *hotp);
void HAL_OTP_WriteTestColumn(OTP_HandleTypeDef *hotp, uint8_t address, uint32_t data[], uint32_t data_length);
void HAL_OTP_WriteTestRow(OTP_HandleTypeDef *hotp, uint32_t data);
void HAL_OTP_WriteTestBit(OTP_HandleTypeDef *hotp, uint32_t data);
void HAL_OTP_Write(OTP_HandleTypeDef *hotp, uint8_t address, uint32_t data[], uint32_t data_length);
void HAL_OTP_ReadTestColumn(OTP_HandleTypeDef *hotp, uint8_t address, uint32_t data_read[], uint32_t data_length);
uint32_t HAL_OTP_ReadTestRow(OTP_HandleTypeDef *hotp);
uint32_t HAL_OTP_ReadTestBit(OTP_HandleTypeDef *hotp);
void HAL_OTP_Read(OTP_HandleTypeDef *hotp, uint8_t address, uint32_t data_read[], uint32_t data_length);

#endif