#include "mik32_hal_adc.h"

void HAL_ADC_CLBEnable(ADC_HandleTypeDef *hadc)
{
    hadc->Instance->REFV_CONFIG |= 1 << REF_CLB_EN_S;
}

void HAL_ADC_CLBDisable(ADC_HandleTypeDef *hadc)
{
    hadc->Instance->REFV_CONFIG &= ~(1 << REF_CLB_EN_S);
}

void HAL_ADC_VCLBSet(ADC_HandleTypeDef *hadc, uint8_t v_coef)
{
    hadc->Instance->REFV_CONFIG &= ~(0xF << REF_CLB_VCOEF_S);
    hadc->Instance->REFV_CONFIG |= ((v_coef & 0xF) << REF_CLB_VCOEF_S);
}

void HAL_ADC_ICLBSet(ADC_HandleTypeDef *hadc, uint8_t i_coef)
{
    hadc->Instance->REFV_CONFIG &= ~(0xF << REF_CLB_ICOEF_S);
    hadc->Instance->REFV_CONFIG |= ((i_coef & 0xF) << REF_CLB_ICOEF_S);
}

void HAL_ADC_ResetEnable(ADC_HandleTypeDef *hadc)
{
    hadc->Instance->ADC_CONFIG |= 1 << ADC_RESETn_S;
}

void HAL_ADC_ResetDisable(ADC_HandleTypeDef *hadc)
{
    hadc->Instance->ADC_CONFIG &= ~(1 << ADC_RESETn_S);
}

void HAL_ADC_Disable(ADC_HandleTypeDef *hadc)
{
    hadc->Instance->ADC_CONFIG &= ~(1 << ADC_EN_S);
    HAL_ADC_ResetDisable(hadc);
}

void HAL_ADC_Enable(ADC_HandleTypeDef *hadc)
{
    hadc->Instance->ADC_CONFIG |= 1 << ADC_EN_S;
    HAL_ADC_ResetEnable(hadc);
}

void HAL_ADC_ChannelSet(ADC_HandleTypeDef *hadc)
{
    switch (hadc->Init.Sel)
    {
    case ADC_CHANNEL0:
        PAD_CONFIG->PORT_1_CFG |= (ADC_PORT_AS_FUNC3 << 2 * ADC_CHANNEL0_PORT_1_5);
        break;
    case ADC_CHANNEL1:
        PAD_CONFIG->PORT_1_CFG |= (ADC_PORT_AS_FUNC3 << 2 * ADC_CHANNEL1_PORT_1_7);
        break;
    case ADC_CHANNEL2:
        PAD_CONFIG->PORT_0_CFG |= (ADC_PORT_AS_FUNC3 << 2 * ADC_CHANNEL2_PORT_0_2);
         break;
    case ADC_CHANNEL3:
        PAD_CONFIG->PORT_0_CFG |= (ADC_PORT_AS_FUNC3 << 2 * ADC_CHANNEL3_PORT_0_4);
        break;
    case ADC_CHANNEL4:
        PAD_CONFIG->PORT_0_CFG |= (ADC_PORT_AS_FUNC3 << 2 * ADC_CHANNEL4_PORT_0_7);
        break;
    case ADC_CHANNEL5:
        PAD_CONFIG->PORT_0_CFG |= (ADC_PORT_AS_FUNC3 << 2 * ADC_CHANNEL5_PORT_0_9);
        break;
    case ADC_CHANNEL6:
        PAD_CONFIG->PORT_0_CFG |= (ADC_PORT_AS_FUNC3 << 2 * ADC_CHANNEL6_PORT_0_11);
        break;
    case ADC_CHANNEL7:
        PAD_CONFIG->PORT_0_CFG |= (ADC_PORT_AS_FUNC3 << 2 * ADC_CHANNEL7_PORT_0_13);
        break;
    }

    hadc->Instance->ADC_CONFIG &= ~(0b111 << ADC_SEL_S);            /* Сброс канала АЦП */
    hadc->Instance->ADC_CONFIG |= (hadc->Init.Sel << ADC_SEL_S);            /* Настройка канала АЦП */
}

void HAL_ADC_Init(ADC_HandleTypeDef *hadc)
{ 
    hadc->Instance->ADC_CONFIG = 0;
    HAL_ADC_Enable(hadc);

    HAL_ADC_ChannelSet(hadc); /* Настройка канала АЦП. Перевод используемого вывода в аналоговый режим */
    if((hadc->Init.EXTRef == ADC_EXTREF_ON) || (hadc->Init.EXTClb == ADC_EXTCLB_ADCREF))
    {
        PAD_CONFIG->PORT_1_CFG |= (ADC_PORT_AS_FUNC3 << 2 * ADC_REF_PORT_1_10);
    }

    hadc->Instance->ADC_CONFIG |=   (hadc->Init.EXTRef << ADC_EXTREF_S) |       /* Настройка источника опорного напряжения */
                                    (hadc->Init.EXTClb << ADC_EXTPAD_EN_S);     /* Выбор внешнего источника опорного напряжения */


}

void HAL_ADC_Single(ADC_HandleTypeDef *hadc)
{
    hadc->Instance->ADC_SINGLE = 1;
}

void HAL_ADC_ContiniusDisabled(ADC_HandleTypeDef *hadc)
{
    hadc->Instance->ADC_CONTINIUS = 0;
}

void HAL_ADC_ContiniusEnable(ADC_HandleTypeDef *hadc)
{
    hadc->Instance->ADC_CONTINIUS = 1;
}

void HAL_ADC_WaitValid(ADC_HandleTypeDef *hadc)
{
    /* Ожидание актуальных данных - ADC_VALID = 1 */
    while (!(hadc->Instance->ADC_VALID));
}

uint16_t HAL_ADC_GetValue(ADC_HandleTypeDef *hadc)
{
    uint16_t value = hadc->Instance->ADC_VALUE;

    return value;
}

uint16_t HAL_ADC_WaitAndGetValue(ADC_HandleTypeDef *hadc)
{
    HAL_ADC_WaitValid(hadc);

    uint16_t value = HAL_ADC_GetValue(hadc);
    
    return value;
}

