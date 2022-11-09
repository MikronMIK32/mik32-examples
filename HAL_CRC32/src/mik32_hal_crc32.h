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
#define CRC_REVERSE_BITS_BYTES      2 // Перестанавливаются и биты, и байты
#define CRC_REVERSE_BYTES           3 // Биты в байтах не перестанавливаются, байты перестанавливаются 

/* Модификаторы */
#define CRC_REFIN_FALSE     CRC_REVERSE_BYTES           // RefIn = false - CRC_REVERSE_BYTES
#define CRC_REFIN_TRUE      CRC_REVERSE_BITS_BYTES      // RefIn = true - CRC_REVERSE_BITS_BYTES
#define CRC_REFOUT_FALSE    CRC_REVERSE_OFF             // RefOut = false - CRC_REVERSE_OFF
#define CRC_REFOUT_TRUE     CRC_REVERSE_BITS_BYTES      // RefOUT = true - CRC_REVERSE_BITS_BYTES

/* CRC_OutputInversion - Инверсия контрольной суммы */
#define CRC_OUTPUTINVERSION_OFF     0 // Инверсия выключена
#define CRC_OUTPUTINVERSION_ON      1 // Инверсия включена (операция XOR выполняется)

/* Максимальное количество слов/байтов в буфере */
#define CRC_MAX_WORDS   16
#define CRC_MAX_BYTES   64

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


void HAL_CRC_SetInputReverse(CRC_HandleTypeDef *hcrc);
void HAL_CRC_SetOutputInversion(CRC_HandleTypeDef *hcrc);
void HAL_CRC_SetOutputReverse(CRC_HandleTypeDef *hcrc);
void HAL_CRC_SetInit(CRC_HandleTypeDef *hcrc);
void HAL_CRC_Init(CRC_HandleTypeDef *hcrc);
void HAL_CRC_WaitBusy(CRC_HandleTypeDef *hcrc);
void HAL_RTC_WriteData(CRC_HandleTypeDef *hcrc, uint8_t message[], uint32_t message_length);
void HAL_RTC_WriteData32(CRC_HandleTypeDef *hcrc, uint32_t message[], uint32_t message_length);
uint32_t HAL_RTC_ReadCRC(CRC_HandleTypeDef *hcrc);

#endif



/**************************************CRC-32**************************************/
// hcrc.Poly = 0x04C11DB7;                             
// hcrc.Init = 0xFFFFFFFF;                                 
// hcrc.InputReverse = CRC_REFIN_TRUE;                
// hcrc.OutputReverse = CRC_REFOUT_TRUE;                            
// hcrc.OutputInversion = CRC_OUTPUTINVERSION_ON;

/**************************************CRC-32/BZIP2**************************************/
// hcrc.Poly = 0x04C11DB7;                             
// hcrc.Init = 0xFFFFFFFF;                                 
// hcrc.InputReverse = CRC_REFIN_FALSE;              
// hcrc.OutputReverse = CRC_REFOUT_FALSE;                          
// hcrc.OutputInversion = CRC_OUTPUTINVERSION_ON;

/*******************************************CRC-32C*******************************************/
// hcrc.Poly = 0x1EDC6F41;                             
// hcrc.Init = 0xFFFFFFFF;                                 
// hcrc.InputReverse = CRC_REFIN_TRUE;               
// hcrc.OutputReverse = CRC_REFOUT_TRUE;                           
// hcrc.OutputInversion = CRC_OUTPUTINVERSION_ON;

/*******************************************CRC-32D*******************************************/
// hcrc.Poly = 0xA833982B;                             
// hcrc.Init = 0xFFFFFFFF;                                 
// hcrc.InputReverse = CRC_REFIN_TRUE;              
// hcrc.OutputReverse = CRC_REFOUT_TRUE;                         
// hcrc.OutputInversion = CRC_OUTPUTINVERSION_ON;

/****************************************CRC-32/JAMCRC****************************************/
// hcrc.Poly = 0x04C11DB7;                             
// hcrc.Init = 0xFFFFFFFF;                                 
// hcrc.InputReverse = CRC_REFIN_TRUE;               
// hcrc.OutputReverse = CRC_REFOUT_TRUE;                          
// hcrc.OutputInversion = CRC_OUTPUTINVERSION_OFF;

/****************************************CRC-32/MPEG-2****************************************/
// hcrc.Poly = 0x04C11DB7;                             
// hcrc.Init = 0xFFFFFFFF;                                 
// hcrc.InputReverse = CRC_REFIN_FALSE;              
// hcrc.OutputReverse = CRC_REFOUT_FALSE;                          
// hcrc.OutputInversion = CRC_OUTPUTINVERSION_OFF;

/*****************************************CRC-32/POSIX*****************************************/
// hcrc.Poly = 0x04C11DB7;                             
// hcrc.Init = 0x00000000;                                 
// hcrc.InputReverse = CRC_REFIN_FALSE;              
// hcrc.OutputReverse = CRC_REFOUT_FALSE;                       
// hcrc.OutputInversion = CRC_OUTPUTINVERSION_ON;

/*******************************************CRC-32Q*******************************************/
// hcrc.Poly = 0x814141AB;                             
// hcrc.Init = 0x00000000;                                 
// hcrc.InputReverse = CRC_REFIN_FALSE;                 
// hcrc.OutputReverse = CRC_REFOUT_FALSE;                          
// hcrc.OutputInversion = CRC_OUTPUTINVERSION_OFF;

/*****************************************CRC-32/XFER*****************************************/
// hcrc.Poly = 0x000000AF;                             
// hcrc.Init = 0x00000000;                                 
// hcrc.InputReverse = CRC_REFIN_FALSE;               
// hcrc.OutputReverse = CRC_REFOUT_FALSE;                        
// hcrc.OutputInversion = CRC_OUTPUTINVERSION_OFF;

