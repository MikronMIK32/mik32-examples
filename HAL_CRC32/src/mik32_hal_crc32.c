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
    /* Инверсия контрольной суммы */
    HAL_CRC_SetOutputInversion(hcrc);
}

void HAL_CRC_Init(CRC_HandleTypeDef *hcrc)
{
    /* Задается полином */
    HAL_CRC_SetPoly(hcrc);

    /* Найстройка перестановки битов/байтов входных данных */
    HAL_CRC_SetInputReverse(hcrc);

    /* Найстройка перестановки битов/байтов выходных данных */
    HAL_CRC_SetOutputReverse(hcrc);
}

void HAL_CRC_WaitBusy(CRC_HandleTypeDef *hcrc)
{
    /* Ожидание 1 такта для установления флага BUSY в 1 после записи в регистр данных слова */
    for (uint8_t i = 0; i < 1; i++);
    /* Ожидание когда флаг BUSY станет 0 - автомат закончил вычисления */
    while (hcrc->Instance->CTRL & CRC_CTRL_BUSY_M);
}