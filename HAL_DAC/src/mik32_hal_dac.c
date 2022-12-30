#include "mik32_hal_dac.h"

void HAL_DAC_CLBEnable(DAC_HandleTypeDef *hdac)
{
    hdac->Instance->REFV_CONFIG |= 1 << REF_CLB_EN_S;
}

void HAL_DAC_CLBDisable(DAC_HandleTypeDef *hdac)
{
    hdac->Instance->REFV_CONFIG &= ~(1 << REF_CLB_EN_S);
}

void HAL_DAC_VCLBSet(DAC_HandleTypeDef *hdac, uint8_t v_coef)
{
    hdac->Instance->REFV_CONFIG &= ~(0xF << REF_CLB_VCOEF_S);
    hdac->Instance->REFV_CONFIG |= ((v_coef & 0xF) << REF_CLB_VCOEF_S);
}

void HAL_DAC_ICLBSet(DAC_HandleTypeDef *hdac, uint8_t i_coef)
{
    hdac->Instance->REFV_CONFIG &= ~(0xF << REF_CLB_ICOEF_S);
    hdac->Instance->REFV_CONFIG |= ((i_coef & 0xF) << REF_CLB_ICOEF_S);
}

void HAL_DAC_SetDiv(DAC_HandleTypeDef *hdac, uint8_t div)
{
    //div &= ~(1<<7); // div должно быть 7-битным

    hdac->Instance_dac->CFG &= ~DAC_CFG_DIV_M;
    hdac->Instance_dac->CFG |= DAC_CFG_DIV_M;
}

void HAL_DAC_ResetEnable(DAC_HandleTypeDef *hdac)
{
    hdac->Instance_dac->CFG |= DAC_CFG_RESN_M;
}

void HAL_DAC_ResetDisable(DAC_HandleTypeDef *hdac)
{
    hdac->Instance_dac->CFG &= ~DAC_CFG_RESN_M;
}

void HAL_DAC_Disable(DAC_HandleTypeDef *hdac)
{
    hdac->Instance_dac->CFG &= ~DAC_CFG_EN18_M;
    HAL_DAC_ResetDisable(hdac);
}

void HAL_DAC_Enable(DAC_HandleTypeDef *hdac)
{
    hdac->Instance_dac->CFG |= DAC_CFG_EN18_M;
    HAL_DAC_ResetEnable(hdac);
}

void HAL_DAC_Init(DAC_HandleTypeDef *hdac)
{ 
    /* Очищение регистра настроек ЦАП */
    hdac->Instance_dac->CFG = 0;

    HAL_DAC_Enable(hdac);

    /* Перевод вывода DAC_REF в аналоговый режим */
    if((hdac->Init.EXTRef == DAC_EXTREF_ON) || (hdac->Init.EXTClb == DAC_EXTCLB_DACREF))
    {
        PAD_CONFIG->PORT_1_CFG |= (DAC_PORT_AS_FUNC3 << 2 * DAC_REF_PORT_1_11);
    }

    /* Перевод вывода ЦАП в аналоговый режим */
    if(hdac->Instance_dac == HAL_DAC1)
    {
        PAD_CONFIG->PORT_1_CFG |= (DAC_PORT_AS_FUNC3 << 2 * DAC1_PORT_1_12);
        #ifdef MIK32_DAC_DEBUG
        xprintf("PORT_1_12 - функция DAC1\n");
        #endif
    }
    if(hdac->Instance_dac == HAL_DAC2)
    {
        PAD_CONFIG->PORT_1_CFG |= (DAC_PORT_AS_FUNC3 << 2 * DAC2_PORT_1_13);
        #ifdef MIK32_DAC_DEBUG
        xprintf("PORT_1_13 - функция DAC2\n");
        #endif
    }

    HAL_DAC_SetDiv(hdac, hdac->Init.DIV); /* Задание делителя */

    hdac->Instance_dac->CFG |= (hdac->Init.EXTRef << DAC_CFG_EXTEN_S) |    /* Настройка источника опорного напряжения */
                               (hdac->Init.EXTClb << DAC_EXTPAD_EN_S);     /* Выбор внешнего источника опорного напряжения */


}

void HAL_DAC_SetValue(DAC_HandleTypeDef *hdac, uint16_t DAC_Value)
{
    DAC_Value &= ~(0xF << 12);
    hdac->Instance_dac->VALUE = DAC_Value;
}