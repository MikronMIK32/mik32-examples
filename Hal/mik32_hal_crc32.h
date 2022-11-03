#ifndef MIK32_HAL_CRC32
#define MIK32_HAL_CRC32

#include "def_list.h"
#include "mcu32_memory_map.h"
#include "crc.h"

#ifdef MIK32_CRC_DEBUG
#include "common.h"
#endif

/* CRC_Reverse - Перестановка битов/байтов данных */
#define CRC_REVERSE_OFF             0 // Перестановка выключена
#define CRC_REVERSE_BITS            1 // Биты в байтах перестанавливаются, байты не перестанавливаются
#define CRC_REVERSE_BYTES           2 // Биты в байтах не перестанавливаются, байты перестанавливаются 
#define CRC_REVERSE_BITS_BYTES      3 // Перестанавливаются и биты, и байты

/* CRC_OutputInversion - Инверсия контрольной суммы */
#define I2C_OUTPUTINVERSION_OFF     0 // Инверсия выключена
#define I2C_OUTPUTINVERSION_ON      1 // Инверсия включена (операция XOR выполняется)

typedef struct
{
    CRC_TypeDef *Instance;      /* Базоый адрес регистров CRC */

    uint32_t Poly;              /* Значение порождающего многочлена 
                                Этот параметр может быть 32-битным значением */

    uint32_t Init;              /* Стартовое значение данных 
                                Этот параметр может быть 32-битным значением */

    uint8_t InputReverse;       /* Перестановка битов/байтов входных данных 
                                Этот параметр может быть одним из значений CRC_Reverse */

    uint8_t OutputReverse;      /* Перестановка битов/байтов выходных данных 
                                Этот параметр может быть одним из значений CRC_Reverse */

    uint8_t OutputInversion;    /* Инверсия контрольной суммы 
                                Этот параметр может быть одним из значений CRC_OutputInversion */

} CRC_HandleTypeDef;


void HAL_CRC_SetPoly(CRC_HandleTypeDef *hcrc);
void HAL_CRC_SetInputReverse(CRC_HandleTypeDef *hcrc);
void HAL_CRC_SetOutputInversion(CRC_HandleTypeDef *hcrc);
void HAL_CRC_SetOutputReverse(CRC_HandleTypeDef *hcrc);
void HAL_CRC_Init(CRC_HandleTypeDef *hcrc);
void HAL_CRC_WaitBusy(CRC_HandleTypeDef *hcrc);


#endif