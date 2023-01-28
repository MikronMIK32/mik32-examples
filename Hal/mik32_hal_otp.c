#include "mik32_hal_otp.h"


void HAL_OTP_PowerOff(OTP_HandleTypeDef *hotp, uint8_t PowerOff)
{
    uint32_t OtpadjConfig = hotp->Instance->OTPADJ;

    OtpadjConfig &= ~OTP_OTPADJ_POWER_OFF_I_M;
    OtpadjConfig |= PowerOff << OTP_OTPADJ_POWER_OFF_I_S;

    hotp->Instance->OTPADJ = OtpadjConfig;

    /* После выхода из режима пониженного энергопотребления необходимо выдержать паузу */
    for (volatile int i = 0; i < 1000; i++);  
}

void HAL_OTP_SetUppRead(OTP_HandleTypeDef *hotp, uint8_t UppReadVoltage)
{
    uint32_t OtpadjConfig = hotp->Instance->OTPADJ;

    OtpadjConfig &= ~OTP_OTPADJ_SEL_UPP_READ_I_M;
    OtpadjConfig |= UppReadVoltage << OTP_OTPADJ_SEL_UPP_READ_I_S;

    hotp->Instance->OTPADJ = OtpadjConfig;
}

void HAL_OTP_SetReadCur(OTP_HandleTypeDef *hotp, uint8_t ReadCur)
{
    uint32_t OtpadjConfig = hotp->Instance->OTPADJ;

    OtpadjConfig &= ~OTP_OTPADJ_SEL_READ_CUR_I_M;
    OtpadjConfig |= ReadCur << OTP_OTPADJ_SEL_READ_CUR_I_S;

    hotp->Instance->OTPADJ = OtpadjConfig;
}

void HAL_OPT_TimeInit(OTP_HandleTypeDef *hotp)
{
    uint8_t APBMDivider = PM->DIV_APB_M;
    
    uint32_t OtpadjConfig = hotp->Instance->OTPADJ;
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

    OtpadjConfig &= ~OTP_OTPADJ_N_RSU_M; /* При частоте менее 200МГц рекомендуемое значение 0 */

    OtpadjConfig &= ~OTP_OTPADJ_N_RA_M;
    OtpadjConfig |= recommended_value << OTP_OTPADJ_N_RA_S;

    OtpadjConfig &= ~OTP_OTPADJ_N_RH_M;
    OtpadjConfig |= recommended_value << OTP_OTPADJ_N_RH_S;

    otpwt1_config &= ~OTP_OTPWT1_N_SU_M;
    otpwt1_config |= recommended_value << OTP_OTPWT1_N_SU_S;

    otpwt1_config &= ~OTP_OTPWT1_N_H_M;
    otpwt1_config |= recommended_value << OTP_OTPWT1_N_H_S;

    otpwt2_config &= ~OTP_OTPWT2_N_W_M;
    otpwt2_config |= recommended_N_W << OTP_OTPWT2_N_W_S;

    hotp->Instance->OTPADJ = OtpadjConfig;
    hotp->Instance->OTPWT1 = otpwt1_config;
    hotp->Instance->OTPWT2 = otpwt2_config;

}

void HAL_OPT_SetReadMode(OTP_HandleTypeDef *hotp, uint8_t ReadMode)
{
    uint32_t OtpconConfig = hotp->Instance->OTPCON;

    OtpconConfig &= ~OTP_OTPCON_APBNWS_M;
    OtpconConfig |= ReadMode << OTP_OTPCON_APBNWS_S;

    hotp->Instance->OTPCON = OtpconConfig;
    hotp->ReadMode = ReadMode;
}

void HAL_OTP_Init(OTP_HandleTypeDef *hotp)
{
    /* Настройка временных ограничений */
    HAL_OPT_TimeInit(hotp);

    /* Выбор напряжения на UPP матрицы */
    HAL_OTP_SetUppRead(hotp, OTP_UPP_READ_2_5V);

    /* Режим чтения */
    HAL_OPT_SetReadMode(hotp, hotp->ReadMode);
}

void HAL_OTP_WaitBSY(OTP_HandleTypeDef *hotp)
{
    /* Опрос флага BSY пока он не станет очищен */
    while (hotp->Instance->OTPSTA & OTP_OTPSTA_BSY_M);
}

void HAL_OTP_WriteTestColumn(OTP_HandleTypeDef *hotp, uint8_t Address, uint32_t Data[], uint32_t DataLength)
{
    /* OTPA[4:3] = 10b - тестовый столбец OTP */
    hotp->Instance->OTPA = 0b10000 + (Address & 0b111);

    for (uint32_t i = 0; i < DataLength; i++)
    {
        hotp->Instance->OTPDAT = Data[i];
        HAL_OTP_WaitBSY(hotp);
    }
}

void HAL_OTP_WriteTestRow(OTP_HandleTypeDef *hotp, uint32_t Data)
{
    /* OTPA[4:3] = 01b - тестовая строка OTP */
    hotp->Instance->OTPA = 0b01000;

    hotp->Instance->OTPDAT = Data;
    HAL_OTP_WaitBSY(hotp);
}

void HAL_OTP_WriteTestBit(OTP_HandleTypeDef *hotp, uint32_t Data)
{
    /* OTPA[4:3] = 11b - последняя тестовая ячейка в тестовой строке */
    hotp->Instance->OTPA = 0b11000;

    hotp->Instance->OTPDAT = Data;
    HAL_OTP_WaitBSY(hotp);
}

void HAL_OTP_Write(OTP_HandleTypeDef *hotp, uint8_t Address, uint32_t Data[], uint32_t DataLength)
{
    /* OTPA[4:3] = 00b - основной массив OTP */
    hotp->Instance->OTPA = 0b00000 + (Address & 0b111);

    for (uint32_t i = 0; i < DataLength; i++)
    {
        hotp->Instance->OTPDAT = Data[i];
        HAL_OTP_WaitBSY(hotp);
    }
}

void HAL_OTP_ReadTestColumn(OTP_HandleTypeDef *hotp, uint8_t Address, uint32_t DataRead[], uint32_t DataLength)
{
    if (hotp->ReadMode == OPT_READ_3STAGES)     /* Чтение в 3 этапа. Без вставки тактов ожидания. С опросом BSY. Без автоинкрементирования адреса OTPA */
    {
        /* OTPA[4:3] = 10b - тестовый столбец OTP */
        uint8_t address_mask = 0b00111;
        uint8_t address_column_mask = (1 << 4);

        for (uint32_t i = 0; i < DataLength; i++)
        {
            hotp->Instance->OTPA = ((Address + i) & address_mask) | address_column_mask;
            HAL_OTP_WaitBSY(hotp);
            DataRead[i] = hotp->Instance->OTPDAT;
        }
    }
    else    /* Чтение в 2 этапа. Со вставкой тактов ожидания. Без опроса BSY. С автоинкрементированием адреса OTPA. */
    {
        /* OTPA[4:3] = 10b - тестовый столбец OTP */
        uint8_t address_column = 0b10000 + (Address & 0b111); 
        hotp->Instance->OTPA = address_column;
        volatile uint32_t dummy = hotp->Instance->OTPDAT;
        
        for (uint32_t i = 0; i < DataLength; i++)
        {
            DataRead[i] = hotp->Instance->OTPDAT;
        }
    }
    
    
}

uint32_t HAL_OTP_ReadTestRow(OTP_HandleTypeDef *hotp)
{    
    uint32_t DataRead = 0;
    
    /* OTPA[4:3] = 01b - тестовая строка OTP */
    hotp->Instance->OTPA = 0b01000;

    if (hotp->ReadMode == OPT_READ_3STAGES)     /* Чтение в 3 этапа. Без вставки тактов ожидания. С опросом BSY. Без автоинкрементирования адреса OTPA */
    {
        HAL_OTP_WaitBSY(hotp);
    }
    else
    {
        volatile uint32_t dummy = hotp->Instance->OTPDAT;
    }
    
    DataRead = hotp->Instance->OTPDAT;

    return DataRead;
}

uint32_t HAL_OTP_ReadTestBit(OTP_HandleTypeDef *hotp)
{
    uint32_t DataRead = 0;
    
    /* OTPA[4:3] = 11b - последняя тестовая ячейка в тестовой строке */
    hotp->Instance->OTPA = 0b11000;

    if (hotp->ReadMode == OPT_READ_3STAGES)     /* Чтение в 3 этапа. Без вставки тактов ожидания. С опросом BSY. Без автоинкрементирования адреса OTPA */
    {
        HAL_OTP_WaitBSY(hotp);
    }
    else
    {
        volatile uint32_t dummy = hotp->Instance->OTPDAT;
    }

    DataRead = hotp->Instance->OTPDAT;

    return DataRead;
}

void HAL_OTP_Read(OTP_HandleTypeDef *hotp, uint8_t Address, uint32_t DataRead[], uint32_t DataLength)
{
    if (hotp->ReadMode == OPT_READ_3STAGES)     /* Чтение в 3 этапа. Без вставки тактов ожидания. С опросом BSY. Без автоинкрементирования адреса OTPA */
    {
        /* OTPA[4:3] = 00b - основной массив OTP */
        uint8_t address_mask = 0b00111;

        for (uint32_t i = 0; i < DataLength; i++)
        {
            hotp->Instance->OTPA = (Address + i) & address_mask;
            HAL_OTP_WaitBSY(hotp);
            DataRead[i] = hotp->Instance->OTPDAT;
        }
    }
    else    /* Чтение в 2 этапа. Со вставкой тактов ожидания. Без опроса BSY. С автоинкрементированием адреса OTPA. */
    {
        /* OTPA[4:3] = 00b - основной массив OTP */
        hotp->Instance->OTPA = 0b00000 + (Address & 0b111); 
        volatile uint32_t dummy = hotp->Instance->OTPDAT;
        
        for (uint32_t i = 0; i < DataLength; i++)
        {
            DataRead[i] = hotp->Instance->OTPDAT;
        }
    }

}
