#include "mik32_hal_crc32.h"


void HAL_CRC_SetPoly(CRC_HandleTypeDef *hcrc)
{
    /* Задается полином */
    hcrc->Instance->POLY = hcrc->Poly;
}

void HAL_CRC_SetInputReverse(CRC_HandleTypeDef *hcrc)
{
    /* Найстройка перестановки битов/байтов входных данных */
    hcrc->Instance->CTRL &= ~(0b11 << CRC_CTRL_TOT_S);
    hcrc->Instance->CTRL |= hcrc->InputReverse << CRC_CTRL_TOT_S;
}

void HAL_CRC_SetOutputInversion(CRC_HandleTypeDef *hcrc)
{
    hcrc->Instance->CTRL &= ~(1 << CRC_CTRL_FXOR_S);
    hcrc->Instance->CTRL |= hcrc->OutputInversion << CRC_CTRL_FXOR_S;
}

void HAL_CRC_SetOutputReverse(CRC_HandleTypeDef *hcrc)
{
    /* Найстройка перестановки битов/байтов выходных данных */
    hcrc->Instance->CTRL &= ~(0b11 << CRC_CTRL_TOTR_S);
    hcrc->Instance->CTRL |= hcrc->OutputReverse << CRC_CTRL_TOTR_S;
}

void HAL_CRC_SetInit(CRC_HandleTypeDef *hcrc)
{
    /* Назначение регистра данных: 1 – записываем начальное значение */
    hcrc->Instance->CTRL |= CRC_CTRL_WAS_M;

    /* Pапись в регистр данных 32-разрядное начальное значение */
    hcrc->Instance->DATA32 = hcrc->Init;

    /* Назначение регистра данных: 0 – записываем данные */
    hcrc->Instance->CTRL &= ~CRC_CTRL_WAS_M;
}

void HAL_CRC_Init(CRC_HandleTypeDef *hcrc)
{
    /* Найстройка перестановки битов/байтов входных данных */
    HAL_CRC_SetInputReverse(hcrc);

    /* Найстройка перестановки битов/байтов выходных данных */
    HAL_CRC_SetOutputReverse(hcrc);

    /* Инверсия контрольной суммы */
    HAL_CRC_SetOutputInversion(hcrc);

    /* Задается полином */
    HAL_CRC_SetPoly(hcrc);
}

void HAL_CRC_WaitBusy(CRC_HandleTypeDef *hcrc)
{
    /* Ожидание 1 такта для установления флага BUSY в 1 после записи в регистр данных слова */
    for (uint8_t i = 0; i < 100; i++);
    /* Ожидание когда флаг BUSY станет 0 - автомат закончил вычисления */
    while (hcrc->Instance->CTRL & CRC_CTRL_BUSY_M);
}

void HAL_RTC_WriteData(CRC_HandleTypeDef *hcrc, uint8_t message[], uint32_t message_length)
{
    if(message_length > CRC_MAX_BYTES)
    {
        #ifdef MIK32_CRC_DEBUG
        xprintf("Ошибка: переполнение буфера\n");
        #endif

        return;
    }
    
    /* Запись начального значения Init */
    HAL_CRC_SetInit(hcrc);
    
    uint32_t i = 0; 
    while (i < message_length)
    {
        if (i + 3 < message_length)
        {
            hcrc->Instance->DATA32 = (message[i+3] << 0) | 
                                     (message[i+2] << 8) |
                                     (message[i+1] << 16) |
                                     (message[i] << 24);
            i += 4;
        }
        else if (i + 1 < message_length)
        {
            hcrc->Instance->DATA16 = (message[i+1] << 0) | 
                          (message[i] << 8);
            i += 2;
        }
        else
        {
            hcrc->Instance->DATA8 = message[i];
            i += 1;
        }
    }
}

void HAL_RTC_WriteData32(CRC_HandleTypeDef *hcrc, uint32_t message[], uint32_t message_length)
{
    if(message_length > CRC_MAX_WORDS)
    {
        #ifdef MIK32_CRC_DEBUG
        xprintf("Ошибка: переполнение буфера\n");
        #endif

        return;
    }
    
    
    /* Запись начального значения Init */
    HAL_CRC_SetInit(hcrc);
    
    uint32_t data;
    for(uint32_t i = 0; i < message_length; i++)
    {
        data = message[i]; 
        hcrc->Instance->DATA32 = data;
    }
}

uint32_t HAL_RTC_ReadCRC(CRC_HandleTypeDef *hcrc)
{
    uint32_t CRCValue;
    /* Ожидание завершения вычисления CRC */
    HAL_CRC_WaitBusy(hcrc);

    CRCValue = CRC->DATA32;

    return CRCValue;
}
