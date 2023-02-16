#ifndef MIK32_HAL_CRC32
#define MIK32_HAL_CRC32

#include "def_list.h"
#include "mcu32_memory_map.h"
#include "crc.h"

#ifdef MIK32_CRC_DEBUG
#include "common.h"
#endif
 
/* Title: Макросы */

/*
 * Defines: Перестановка битов/байтов данных
 *
 * CRC_REVERSE_OFF - Перестановка выключена
 * CRC_REVERSE_BITS - Биты в байтах перестанавливаются, байты не перестанавливаются
 * CRC_REVERSE_BITS_BYTES - Перестанавливаются и биты, и байты
 * CRC_REVERSE_BYTES - Биты в байтах не перестанавливаются, байты перестанавливаются          
 *
 */
#define CRC_REVERSE_OFF             0 // Перестановка выключена
#define CRC_REVERSE_BITS            1 // Биты в байтах перестанавливаются, байты не перестанавливаются
#define CRC_REVERSE_BITS_BYTES      2 // Перестанавливаются и биты, и байты
#define CRC_REVERSE_BYTES           3 // Биты в байтах не перестанавливаются, байты перестанавливаются 


/* Модификаторы */ 
/*
 * Defines: Флаг RefIn
 * 
 * Флаг RefIn, указывающий на начало и направление вычислений. 
 * 
 * Перестановка битов/байтов входных данных.
 * 
 * Существует два варианта: 
 * 
 * - CRC_REFIN_FALSE начиная со старшего значащего бита (MSB-first);
 * - CRC_REFIN_TRUE с младшего (LSB-first).
 *
 * CRC_REFIN_FALSE - Биты в байтах не перестанавливаются, байты перестанавливаются 
 * CRC_REFIN_TRUE - Перестанавливаются и биты, и байты      
 *
 */
#define CRC_REFIN_FALSE     CRC_REVERSE_BYTES           // RefIn = false - CRC_REVERSE_BYTES
#define CRC_REFIN_TRUE      CRC_REVERSE_BITS_BYTES      // RefIn = true - CRC_REVERSE_BITS_BYTES

/*
 * Defines: Флаг RefOut
 * 
 * Флаг RefOut, определяющий, инвертируется ли порядок битов регистра при входе на элемент XOR.
 * 
 * Перестановки битов/байтов выходных данных.
 *
 * CRC_REFOUT_FALSE - Биты в байтах не перестанавливаются, байты перестанавливаются 
 * CRC_REFOUT_TRUE - Перестанавливаются и биты, и байты      
 *
 */
#define CRC_REFOUT_FALSE    CRC_REVERSE_OFF             // RefOut = false - CRC_REVERSE_OFF
#define CRC_REFOUT_TRUE     CRC_REVERSE_BITS_BYTES      // RefOUT = true - CRC_REVERSE_BITS_BYTES

/*
 * Defines: Флаг XorOut
 *
 * Инверсия контрольной суммы.
 * Число XorOut = 0xFFFFFFFF, с которым складывается по модулю 2 полученный результат. Побитовая инверсия результата.
 * 
 * CRC_OUTPUTINVERSION_OFF - Инверсия выключена.
 * CRC_OUTPUTINVERSION_ON - Инверсия включена (операция XOR выполняется)  
 *
 */
#define CRC_OUTPUTINVERSION_OFF     0 // Инверсия выключена
#define CRC_OUTPUTINVERSION_ON      1 // Инверсия включена (операция XOR выполняется)

/*
 * Defines: Размер буфера
 * 
 * CRC_MAX_WORDS - Максимальное количество слов в буфере.
 * CRC_MAX_BYTES - Максимальное количество байтов в буфере.
 *
 */
#define CRC_MAX_WORDS   16
#define CRC_MAX_BYTES   64

/* Title: Структуры */

/*
 * Struct: CRC_HandleTypeDef
 * Настройки CRC
 * 
 */
typedef struct
{
    /*
    * Variable: Instance
    * Базоый адрес регистров CRC
    *
    */
    CRC_TypeDef *Instance;

    /*
    * Variable: Poly
    * Значение порождающего многочлена.
    * 
    * Этот параметр может быть 32-битным значением.
    *
    */
    uint32_t Poly;

    /*
    * Variable: Init
    * Стартовое значение данных.
    * 
    * Этот параметр может быть 32-битным значением.
    *
    */
    uint32_t Init;

    /*
    * Variable: InputReverse
    * Перестановка битов/байтов входных данных.
    * 
    * Этот параметр должен быть одним из значений:
    * 
    * - <CRC_REFIN_FALSE>;
    * - <CRC_REFIN_TRUE>.
    *
    */
    uint8_t InputReverse;

/*
 * Variable: OutputReverse
 * Перестановка битов/байтов выходных данных.
 * 
 * Этот параметр должен быть одним из значений:
 * 
 * - <CRC_REFOUT_FALSE>;
 * - <CRC_REFOUT_TRUE>.
 *
 */
    uint8_t OutputReverse;

/*
 * Variable: OutputInversion
 * Инверсия контрольной суммы.
 * 
 * Этот параметр должен быть одним из значений:
 * 
 * - <CRC_OUTPUTINVERSION_OFF>;
 * - <CRC_OUTPUTINVERSION_ON>.
 *
 */
    uint8_t OutputInversion;

} CRC_HandleTypeDef;

/* Title: Функции */

/*
 * Function: HAL_CRC_SetInputReverse
 * Задать перестановка битов/байтов входных данных в соответствии со значением <CRC_HandleTypeDef.InputReverse> (RefIn).
 *
 * Parameters:
 * hcrc - Указатель на структуру с настройками CRC.
 *
 * Returns:
 * void
 */
void HAL_CRC_SetInputReverse(CRC_HandleTypeDef *hcrc);

/*
 * Function: HAL_CRC_SetOutputInversion
 * Инверсия контрольной суммы в соответсвии со значением <CRC_HandleTypeDef.OutputInversion> (XorOut).
 *
 * Parameters:
 * hcrc - Указатель на структуру с настройками CRC.
 *
 * Returns:
 * void
 */
void HAL_CRC_SetOutputInversion(CRC_HandleTypeDef *hcrc);

/*
 * Function: HAL_CRC_SetOutputReverse
 * Перестановки битов/байтов выходных данных в соответствии со значением <CRC_HandleTypeDef.OutputReverse> (RefOut).
 *
 * Parameters:
 * hcrc - Указатель на структуру с настройками CRC.
 *
 * Returns:
 * void
 */
void HAL_CRC_SetOutputReverse(CRC_HandleTypeDef *hcrc);

/*
 * Function: HAL_CRC_SetInit
 * Задать начальное значение данных в соответсвии с <CRC_HandleTypeDef.Init>
 *
 * Parameters:
 * hcrc - Указатель на структуру с настройками CRC.
 *
 * Returns:
 * void
 */
void HAL_CRC_SetInit(CRC_HandleTypeDef *hcrc);

/*
 * Function: HAL_CRC_Init
 * Инициализировать CRC в соответствии с настройками <CRC_HandleTypeDef> *hcrc.
 *
 * Parameters:
 * hcrc - Указатель на структуру с настройками CRC.
 *
 * Returns:
 * void
 */
void HAL_CRC_Init(CRC_HandleTypeDef *hcrc);

/*
 * Function: HAL_CRC_WaitBusy
 * Ожидать сброса флага BUSY. Если флаг равен 0, то автомат закончил вычисления.
 *
 * Parameters:
 * hcrc - Указатель на структуру с настройками CRC.
 *
 * Returns:
 * void
 */
void HAL_CRC_WaitBusy(CRC_HandleTypeDef *hcrc);

/*
 * Function: HAL_CRC_WaitBusy
 * Записать данные по байтам в буфер CRC.
 *
 * Parameters:
 * hcrc - Указатель на структуру с настройками CRC.
 * message -  Массив с данными.
 * message_length - Количество передаваемых байтов.
 *
 * Returns:
 * void
 */
void HAL_RTC_WriteData(CRC_HandleTypeDef *hcrc, uint8_t message[], uint32_t message_length);

/*
 * Function: HAL_RTC_WriteData32
 * Записать данные по словам в буфер CRC.
 *
 * Parameters:
 * hcrc - Указатель на структуру с настройками CRC.
 * message -  Массив с данными.
 * message_length - Количество передаваемых байтов.
 *
 * Returns:
 * void
 */
void HAL_RTC_WriteData32(CRC_HandleTypeDef *hcrc, uint32_t message[], uint32_t message_length);

/*
 * Function: HAL_RTC_ReadCRC
 * Получить значение CRC.
 *
 * Parameters:
 * hcrc - Указатель на структуру с настройками CRC.
 *
 * Returns:
 * (uint32_t ) - Значение CRC.
 *
 */
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

