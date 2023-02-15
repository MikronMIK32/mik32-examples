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


/* Выбор напряжения на UPP матрицы */
#define OTP_UPP_READ_2V         0b000            /* 2 В */
#define OTP_UPP_READ_2_5V       0b001            /* 2,5 В */
#define OTP_UPP_READ_3V         0b011            /* 3 В */
#define OTP_UPP_READ_VDD18      0b010            /* VDD18 */
#define OTP_UPP_READ_VDD5       0b110            /* VDD5 */

/* Выбор тока считывания */
#define OTP_READ_CUR_2         0            /* 2 мкА */
#define OTP_READ_CUR_0_2       1            /* 0,2 мкА */

/* Title: Макросы */

/* Title: Режим пониженного энергопотребления */
/*
 * Define: OTP_POWER_OFF
 * Hard IP введен в режим пониженного энергопотребления, операции записи и чтения запрещены.
 *
 */
#define OTP_POWER_OFF       1            /* Hard IP введен в режим пониженного энергопотребления, операции записи и чтения запрещены */
/*
 * Define: OTP_POWER_ON
 * Hard IP выведен из режима пониженного энергопотребления и может выполнять операции чтения и записи.
 *
 */
#define OTP_POWER_ON        0            /* Hard IP выведен из режима пониженного энергопотребления и может выполнять операции чтения и записи */


/* Title: Режим чтения */
/*
 * Define: OPT_READ_2STAGES
 * Чтение в 2 этапа. Вводятся такты ожидания APB.
 *
 */
#define OPT_READ_2STAGES    0           /* Чтение в 2 этапа. Вводятся такты ожидания APB */
/*
 * Define: OPT_READ_3STAGES
 * Чтение в 3 этапа. Опрос флага BSY. Нет тактов ожидания APB.
 *
 */
#define OPT_READ_3STAGES    1           /* Чтение в 3 этапа. Опрос флага BSY. Нет тактов ожидания APB */

/* Title: Структуры */

/*
 * Struct: OTP_HandleTypeDef
 * Настройки экземпляра OTP.
 * 
 */
typedef struct
{
    /*
    * Variable: Instance
    * Базоый адрес регистров OTP.
    *
    */
    OTP_TypeDef *Instance;       /* Базоый адрес регистров OTP */

    /*
    * Variable: ReadMode
    * Настройка режима чтения.
    *
    */
    uint8_t ReadMode;       /* Режим чтения */
    
} OTP_HandleTypeDef;

/* Title: Функции */

/*
 * Function: HAL_OTP_PowerOff
 * Включить/выключить режим пониженного энергопотребления.
 *
 * Parameters:
 * hotp - Указатель на структуру с настройками OTP.
 * PowerOff - Режим пониженного энергопотребления.
 *
 * Returns:
 * void
 */
void HAL_OTP_PowerOff(OTP_HandleTypeDef *hotp, uint8_t PowerOff);

/*
 * Function: HAL_OTP_SetUppRead
 * Задать напряжение на UPP матрице.
 *
 * Parameters:
 * hotp - Указатель на структуру с настройками OTP.
 * UppReadVoltage - Напряжение на UPP матрице.
 *
 * Returns:
 * void
 */
void HAL_OTP_SetUppRead(OTP_HandleTypeDef *hotp, uint8_t UppReadVoltage);

/*
 * Function: HAL_OTP_SetReadCur
 * Задать ток считывания.
 *
 * Parameters:
 * hotp - Указатель на структуру с настройками OTP.
 * ReadCur - Ток считывания.
 *
 * Returns:
 * void
 */
void HAL_OTP_SetReadCur(OTP_HandleTypeDef *hotp, uint8_t ReadCur);

/*
 * Function: HAL_OPT_TimeInit
 * Инициализация рекомендуемых временных интервалов.
 *
 * Parameters:
 * hotp - Указатель на структуру с настройками OTP.
 *
 * Returns:
 * void
 */
void HAL_OPT_TimeInit(OTP_HandleTypeDef *hotp);

/*
 * Function: HAL_OTP_Init
 * Инициализировать OTP в соответствии с настройками <OTP_HandleTypeDef> *hotp.
 *
 * Parameters:
 * hotp - Указатель на структуру с настройками OTP.
 *
 * Returns:
 * void
 */
void HAL_OTP_Init(OTP_HandleTypeDef *hotp);

/*
 * Function: HAL_OTP_WaitBSY
 * Ожидать сброса флага BSY.
 * 
 * BSY - Блок занят (выполняется запрошенная операция).
 * 
 * Используется при операции записи и чтении в 3 этапа.
 *
 * Parameters:
 * hotp - Указатель на структуру с настройками OTP.
 *
 * Returns:
 * void
 */
void HAL_OTP_WaitBSY(OTP_HandleTypeDef *hotp);

/*
 * Function: HAL_OTP_WriteTestColumn
 * Записать массив данных в тестовый столбец.
 *
 * Если в массиве Data количество бит больше размера столбца, начиная с Address, то запись продолжится с начала столбца.
 * 
 * Parameters:
 * hotp - Указатель на структуру с настройками OTP.
 * Address - Начальная ячейка столбца. Значение может быть в пределах от 0 до 7.
 * Data - Данные для записи в тестовый столбец.
 * DataLength - Размер массива Data.
 *
 * Returns:
 * void
 */
void HAL_OTP_WriteTestColumn(OTP_HandleTypeDef *hotp, uint8_t Address, uint32_t Data[], uint32_t DataLength);

/*
 * Function: HAL_OTP_WriteTestRow
 * Записать данные в тестовую строку.
 * 
 * Parameters:
 * hotp - Указатель на структуру с настройками OTP.
 * Data - Данные для записи в тестовую строку.
 *
 * Returns:
 * void
 */
void HAL_OTP_WriteTestRow(OTP_HandleTypeDef *hotp, uint32_t Data);

/*
 * Function: HAL_OTP_WriteTestBit
 * Записать бит в тестовую ячейку.
 *
 * Parameters:
 * hotp - Указатель на структуру с настройками OTP.
 * Data - Бит для записи в тестовую ячейку.
 *
 * Returns:
 * void
 */
void HAL_OTP_WriteTestBit(OTP_HandleTypeDef *hotp, uint32_t Data);

/*
 * Function: HAL_OTP_Write
 * Записать данные в основной массив OTP.
 * 
 * Parameters:
 * hotp - Указатель на структуру с настройками OTP.
 * Address - Начальная строка. Значение может быть в пределах от 0 до 7.
 * Data - Данные для записи в основной массив OTP.
 * DataLength - Размер массива Data.
 *
 * Returns:
 * void
 */
void HAL_OTP_Write(OTP_HandleTypeDef *hotp, uint8_t Address, uint32_t Data[], uint32_t DataLength);

/*
 * Function: HAL_OTP_ReadTestColumn
 * Прочитать данные из тестового столбца.
 * 
 * Parameters:
 * hotp - Указатель на структуру с настройками OTP.
 * Address - Начальная ячейка столбца. Значение может быть в пределах от 0 до 7.
 * DataRead - Массив для считывания данных тестового столбца.
 * DataLength - Размер массива DataRead.
 *
 * Returns:
 * void
 */
void HAL_OTP_ReadTestColumn(OTP_HandleTypeDef *hotp, uint8_t Address, uint32_t DataRead[], uint32_t DataLength);

/*
 * Function: HAL_OTP_ReadTestRow
 * Прочитать данные из тестовой строки.
 *
 * Parameters:
 * hotp - Указатель на структуру с настройками OTP.
 *
 * Returns:
 * (uint32_t ) - Тестовая строка.
 *
 */
uint32_t HAL_OTP_ReadTestRow(OTP_HandleTypeDef *hotp);

/*
 * Function: HAL_OTP_ReadTestBit
 * Прочитать бит из тестовой ячейки.
 *
 * Parameters:
 * hotp - Указатель на структуру с настройками OTP.
 *
 * Returns:
 * (uint32_t ) - Бит из тестовой ячейки.
 *
 */
uint32_t HAL_OTP_ReadTestBit(OTP_HandleTypeDef *hotp);

/*
 * Function: HAL_OTP_Read
 * Прочитать данные из основного массива OTP.
 * 
 * Parameters:
 * hotp - Указатель на структуру с настройками OTP.
 * Address - Начальная строка основного массива OTP. Значение может быть в пределах от 0 до 7.
 * DataRead - Массив для считывания данных основного массива OTP.
 * DataLength - Размер массива DataRead.
 *
 * Returns:
 * void
 */
void HAL_OTP_Read(OTP_HandleTypeDef *hotp, uint8_t Address, uint32_t DataRead[], uint32_t DataLength);


#endif