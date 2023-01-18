#include "mik32_hal_otp.h"


void HAL_OPT_TimeInit(OTP_HandleTypeDef *hotp)
{
    uint8_t APBMDivider = PM->DIV_APB_M;
    
    uint32_t otpadj_config = hotp->Instance->OTPADJ;
    uint32_t otpwt1_config = hotp->Instance->OTPWT1;
    uint32_t otpwt2_config = hotp->Instance->OTPWT2;

    /* Рекомендуемое значение для N_RA, N_RH, N_SU, N_H. ceil(40/Period), где Period в нс */
    uint32_t recommended_value = 0; 
    if((40 * MIK32_FREQ_MHZ) % ((APBMDivider + 1) * 1000) == 0)
    {
        recommended_value = (40 * MIK32_FREQ_MHZ) / ((APBMDivider + 1) * 1000);
    }
    else
    {
        recommended_value = (40 * MIK32_FREQ_MHZ) / ((APBMDivider + 1) * 1000) + 1;
    }


    /* Рекомендуемое значение N_W = 50 000 000 нс / Pclk, где Pclk – период тактового сигнала в нс */
    uint32_t recommended_N_W = (MIK32_FREQ / 20) / (APBMDivider + 1); 

    otpadj_config &= ~OTP_OTPADJ_N_RSU_M; /* При частоте менее 200МГц рекомендуемое значение 0 */

    otpadj_config &= ~OTP_OTPADJ_N_RA_M;
    otpadj_config |= recommended_value << OTP_OTPADJ_N_RA_S;

    otpadj_config &= ~OTP_OTPADJ_N_RH_M;
    otpadj_config |= recommended_value << OTP_OTPADJ_N_RH_S;

    otpwt1_config &= ~OTP_OTPWT1_N_SU_M;
    otpwt1_config |= recommended_value << OTP_OTPWT1_N_SU_S;

    otpwt1_config &= ~OTP_OTPWT1_N_H_M;
    otpwt1_config |= recommended_value << OTP_OTPWT1_N_H_S;

    otpwt2_config &= ~OTP_OTPWT2_N_W_M;
    otpwt2_config |= recommended_N_W << OTP_OTPWT2_N_W_S;

    hotp->Instance->OTPADJ = otpadj_config;
    hotp->Instance->OTPWT1 = otpwt1_config;
    hotp->Instance->OTPWT2 = otpwt2_config;

}

void HAL_OTP_Init(OTP_HandleTypeDef *hotp)
{
    /* Настройка временных ограничений */
    HAL_OPT_TimeInit(hotp);

    /* Такты ожидания отключены */
    hotp->Instance->OTPCON |= OTP_OTPCON_APBNWS_M; 
}

void HAL_OTP_WaitBSY(OTP_HandleTypeDef *hotp)
{
    /* Опрос флага BSY пока он не станет очищен */
    while (hotp->Instance->OTPSTA & OTP_OTPSTA_BSY_M);
}

void HAL_OTP_WriteTestColumn(OTP_HandleTypeDef *hotp, uint8_t address, uint32_t data[], uint32_t data_length)
{
    /* OTPA[4:3] = 10b - тестовый столбец OTP */
    hotp->Instance->OTPA = 0b10000;
    hotp->Instance->OTPA += address & 0b111;

    for (uint32_t i = 0; i < data_length; i++)
    {
        hotp->Instance->OTPDAT = data[i];
        HAL_OTP_WaitBSY(hotp);
    }
}

void HAL_OTP_WriteTestRow(OTP_HandleTypeDef *hotp, uint32_t data)
{
    /* OTPA[4:3] = 01b - тестовая строка OTP */
    hotp->Instance->OTPA = 0b01000;

    hotp->Instance->OTPDAT = data;
    HAL_OTP_WaitBSY(hotp);
}

void HAL_OTP_WriteTestBit(OTP_HandleTypeDef *hotp, uint32_t data)
{
    /* OTPA[4:3] = 11b - последняя тестовая ячейка в тестовой строке */
    hotp->Instance->OTPA = 0b11000;

    hotp->Instance->OTPDAT = data;
    HAL_OTP_WaitBSY(hotp);
}

void HAL_OTP_Write(OTP_HandleTypeDef *hotp, uint8_t address, uint32_t data[], uint32_t data_length)
{
    /* OTPA[4:3] = 00b - основной массив OTP */
    hotp->Instance->OTPA = 0b00000;
    hotp->Instance->OTPA += address & 0b111;

    for (uint32_t i = 0; i < data_length; i++)
    {
        hotp->Instance->OTPDAT = data[i];
        HAL_OTP_WaitBSY(hotp);
    }
}

void HAL_OTP_ReadTestColumn(OTP_HandleTypeDef *hotp, uint8_t address, uint32_t data_read[], uint32_t data_length)
{
    /* OTPA[4:3] = 10b - тестовый столбец OTP */
    uint8_t address_mask = 0b00111;
    uint8_t address_column_mask = (1 << 4);

    for (uint32_t i = 0; i < data_length; i++)
    {
        hotp->Instance->OTPA = (((address + i) % 8) & address_mask) | address_column_mask;
        HAL_OTP_WaitBSY(hotp);
        data_read[i] = hotp->Instance->OTPDAT;
    }
}

uint32_t HAL_OTP_ReadTestRow(OTP_HandleTypeDef *hotp)
{
    uint32_t data_read = 0;
    
    /* OTPA[4:3] = 01b - тестовая строка OTP */
    hotp->Instance->OTPA = 0b01000;

    HAL_OTP_WaitBSY(hotp);
    data_read = hotp->Instance->OTPDAT;

    return data_read;
}

uint32_t HAL_OTP_ReadTestBit(OTP_HandleTypeDef *hotp)
{
    uint32_t data_read = 0;
    
    /* OTPA[4:3] = 11b - последняя тестовая ячейка в тестовой строке */
    hotp->Instance->OTPA = 0b11000;

    HAL_OTP_WaitBSY(hotp);
    data_read = hotp->Instance->OTPDAT;

    return data_read;
}

void HAL_OTP_Read(OTP_HandleTypeDef *hotp, uint8_t address, uint32_t data_read[], uint32_t data_length)
{
    /* OTPA[4:3] = 00b - основной массив OTP */
    uint8_t address_mask = 0b00111;

    for (uint32_t i = 0; i < data_length; i++)
    {
        hotp->Instance->OTPA = ((address + i) % 8) & address_mask;
        HAL_OTP_WaitBSY(hotp);
        data_read[i] = hotp->Instance->OTPDAT;
    }
}
