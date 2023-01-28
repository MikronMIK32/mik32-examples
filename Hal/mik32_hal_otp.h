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

/* Выбор напряжения на UPP матрицы */
#define OTP_UPP_READ_2V         0b000            /* 2 В */
#define OTP_UPP_READ_2_5V       0b001            /* 2,5 В */
#define OTP_UPP_READ_3V         0b011            /* 3 В */
#define OTP_UPP_READ_VDD18      0b010            /* VDD18 */
#define OTP_UPP_READ_VDD5       0b110            /* VDD5 */

/* Выбор тока считывания */
#define OTP_READ_CUR_2         0            /* 2 мкА */
#define OTP_READ_CUR_0_2       1            /* 0,2 мкА */

/* Режим пониженного энергопотребления */
#define OTP_POWER_OFF       1            /* Hard IP введен в режим пониженного энергопотребления, операции записи и чтения запрещены */
#define OTP_POWER_ON        0            /* Hard IP выведен из режима пониженного энергопотребления и может выполнять операции чтения и записи */

/* Режим чтения */
#define OPT_READ_2STAGES    0           /* Чтение в 2 этапа. Вводятся такты ожидания APB */
#define OPT_READ_3STAGES    1           /* Чтение в 3 этапа. Опрос флага BSY. Нет тактов ожидания APB */

typedef struct
{
    OTP_TypeDef *Instance;       /* Базоый адрес регистров OTP */

    uint8_t ReadMode;       /* Режим чтения */
    
} OTP_HandleTypeDef;


void HAL_OTP_PowerOff(OTP_HandleTypeDef *hotp, uint8_t PowerOff);
void HAL_OTP_SetUppRead(OTP_HandleTypeDef *hotp, uint8_t UppReadVoltage);
void HAL_OTP_SetReadCur(OTP_HandleTypeDef *hotp, uint8_t ReadCur);
void HAL_OPT_TimeInit(OTP_HandleTypeDef *hotp);
void HAL_OTP_Init(OTP_HandleTypeDef *hotp);
void HAL_OTP_WaitBSY(OTP_HandleTypeDef *hotp);
void HAL_OTP_WriteTestColumn(OTP_HandleTypeDef *hotp, uint8_t Address, uint32_t Data[], uint32_t DataLength);
void HAL_OTP_WriteTestRow(OTP_HandleTypeDef *hotp, uint32_t Data);
void HAL_OTP_WriteTestBit(OTP_HandleTypeDef *hotp, uint32_t Data);
void HAL_OTP_Write(OTP_HandleTypeDef *hotp, uint8_t Address, uint32_t Data[], uint32_t DataLength);
void HAL_OTP_ReadTestColumn(OTP_HandleTypeDef *hotp, uint8_t Address, uint32_t DataRead[], uint32_t DataLength);
uint32_t HAL_OTP_ReadTestRow(OTP_HandleTypeDef *hotp);
uint32_t HAL_OTP_ReadTestBit(OTP_HandleTypeDef *hotp);
void HAL_OTP_Read(OTP_HandleTypeDef *hotp, uint8_t Address, uint32_t DataRead[], uint32_t DataLength);

#endif