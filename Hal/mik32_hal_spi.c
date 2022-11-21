#include "mik32_hal_spi.h"

void HAL_SPI_Enable(SPI_HandleTypeDef *hspi)
{
    hspi->Instance->Enable = SPI_ENABLE_M;
}

void HAL_SPI_Disable(SPI_HandleTypeDef *hspi)
{
    hspi->Instance->Enable &= ~SPI_ENABLE_M;
}

void HAL_SPI_ClearRxBuffer(SPI_HandleTypeDef *hspi)
{
    volatile uint32_t dummy = 0;
    while ((hspi->Instance->IntStatus & SPI_RX_FIFO_not_empty_M) != 0)
    {
        dummy = hspi->Instance->RxData;
    }
}

void HAL_SPI_IntEnable(SPI_HandleTypeDef *hspi, uint32_t int_en)
{
    hspi->Instance->IntEnable |= int_en;
}

void HAL_SPI_IntDisable(SPI_HandleTypeDef *hspi, uint32_t int_dis)
{
    hspi->Instance->IntDisable |= int_dis;
}

void HAL_SPI_SetDelayBTWN(SPI_HandleTypeDef *hspi, uint8_t btwn)
{
    hspi->Instance->Delay &= ~SPI_DELAY_BTWN_M;
    hspi->Instance->Delay |= SPI_DELAY_BTWN(btwn);
}

void HAL_SPI_SetDelayAFTER(SPI_HandleTypeDef *hspi, uint8_t after)
{
    hspi->Instance->Delay &= ~SPI_DELAY_AFTER_M;
    hspi->Instance->Delay |= SPI_DELAY_AFTER(after);
}

void HAL_SPI_SetDelayINIT(SPI_HandleTypeDef *hspi, uint8_t init)
{
    hspi->Instance->Delay &= ~SPI_DELAY_INIT_M;
    hspi->Instance->Delay |= SPI_DELAY_INIT(init);
}

void HAL_SPI_SetSlaveIdleCounter(SPI_HandleTypeDef *hspi, uint8_t slave_idle_counter)
{
    hspi->Instance->SIC = slave_idle_counter;
}

void HAL_SPI_SetThresholdTX(SPI_HandleTypeDef *hspi, uint32_t threshold)
{
    hspi->Instance->TxThr = threshold;
}

uint32_t HAL_SPI_ReadModuleID(SPI_HandleTypeDef *hspi)
{
    return hspi->Instance->ID;
}

void HAL_SPI_Init(SPI_HandleTypeDef *hspi)
{
    /* Выключение модуля SPI */
    HAL_SPI_Disable(hspi);

    uint32_t SPI_config = 0;

    /* Настройка режима ведомого */
    switch (hspi->Init.SPI_Mode)
    {
    case HAL_SPI_MODE_MASTER:
        SPI_config = SPI_CONFIG_MASTER_M;
        break;
    case HAL_SPI_MODE_SLAVE:
        SPI_config = SPI_CONFIG_SLAVE_M;
        break;
    }
    
    /* Настройки SPI */    
    SPI_config |=  (hspi->Init.BaudRateDiv << SPI_CONFIG_BAUD_RATE_DIV_S) |     /* Настройка делителя частоты */        
                   (hspi->Init.ManualCS << SPI_CONFIG_Manual_CS_S) |            /* Настройка режима управления сигналом CS */
                   (hspi->Init.CLKPhase << SPI_CONFIG_CLK_PH_S) |               /* Настройка фазы тактового сигнала */
                   (hspi->Init.CLKPolarity << SPI_CONFIG_CLK_POL_S) |           /* Настройка полярности тактового сигнала */
                   (hspi->Init.Decoder << SPI_CONFIG_PERI_SEL_S) |              /* Настройка использования внешнего декодера */
                   (hspi->Init.DataSize << SPI_CONFIG_DATA_SZ_S);               /* Длина передаваемой посылки */

    
    /* Выбор ведомого в соответсвии с режимом ManualCS */
    if(hspi->Init.ManualCS == SPI_MANUALCS_ON)
    {
        /* Ведомое устройство не выбрано. Ручное управление сигналом CS */
        SPI_config |= SPI_CS_NONE << SPI_CONFIG_CS_S;             
    }
    else
    {
        /* Выбор ведомого устройства в атоматическом режиме управления CS */
        SPI_config |= hspi->ChipSelect << SPI_CONFIG_CS_S;
    }


    /* Установка выбранных настроек */
    hspi->Instance->Config = SPI_config;

    /* уровень при котором регистр TX считается незаполненым и формируется прерывание */
    HAL_SPI_SetThresholdTX(hspi, SPI_THRESHOLD_DEFAULT);

    HAL_SPI_ClearRxBuffer(hspi);

    xprintf("SPI_Init\n");

    /* Включение модуля SPI */
    HAL_SPI_Enable(hspi);

}

void HAL_SPI_WaitTxNotFull(SPI_HandleTypeDef *hspi)
{
    while ((hspi->Instance->IntStatus & SPI_TX_FIFO_not_full_M) == 0) ;
}

void HAL_SPI_WaitRxNotEmpty(SPI_HandleTypeDef *hspi)
{

    while ((hspi->Instance->IntStatus & SPI_RX_FIFO_not_empty_M) == 0)
    {

    }
}

bool HAL_SPI_CheckOverflow(SPI_HandleTypeDef *hspi)
{
    if(hspi->Instance->IntStatus & SPI_RX_OVERFLOW_M)
    {
        HAL_SPI_Disable(hspi);
        xprintf("OVERFLOW\n");
        HAL_SPI_ClearRxBuffer(hspi);
        /* Сброс флага RX_OVERFLOW */
        hspi->Instance->IntStatus |= SPI_RX_OVERFLOW_M;
        xprintf("OVR_STATUS = 0x%x\n", (uint8_t)hspi->Instance->IntStatus);
        return true;
    }
    return false;
}

bool HAL_SPI_CheckModeFail(SPI_HandleTypeDef *hspi)
{
    if(hspi->Instance->IntStatus & SPI_MODE_FAIL_M)
    {
        //xprintf("MODE_FAIL\n");
        HAL_SPI_Disable(hspi);
        HAL_SPI_ClearRxBuffer(hspi);
        return true;
    }
    return false;
}

void HAL_SPI_CS_Enable(SPI_HandleTypeDef *hspi, uint32_t CS_M)  
{
    CS_M = CS_M << SPI_CONFIG_CS_S;
    hspi->Instance->Config = (hspi->Instance->Config & ~SPI_CONFIG_CS_M) | CS_M;
}

void HAL_SPI_CS_Disable(SPI_HandleTypeDef *hspi)  
{
	hspi->Instance->Config = (hspi->Instance->Config & ~SPI_CONFIG_CS_M) | SPI_CONFIG_CS_NONE_M;
}

void HAL_SPI_Exchange_Fail(SPI_HandleTypeDef *hspi, uint8_t transmit_bytes[], uint8_t receive_bytes[], uint32_t count)
{

    HAL_SPI_Enable(hspi);
    HAL_SPI_CS_Enable(hspi, SPI_CS_0);

    for(uint32_t i = 0; i < count; i++)
    {    
        /* Ожидание когда TX_FIFO будет не заполнен - регистр заполнен меньше чем THRESHOLD */
        HAL_SPI_WaitTxNotFull(hspi);
        for (uint8_t offset = 0; offset < hspi->Init.DataSize + 1; offset++)
        {
            /* Загрузка байта */
            hspi->Instance->TxData = transmit_bytes[i + offset];
        }
        
  
        for (uint8_t offset = 0; offset < hspi->Init.DataSize + 1; offset++)
        {
            /* Ожидание когда в RX_FIFO появится хотябы один байт */
            HAL_SPI_WaitRxNotEmpty(hspi);
            /* Чтение байта */
            receive_bytes[i + offset] = hspi->Instance->RxData;
        }
        
        i = i + hspi->Init.DataSize;
    }

    HAL_SPI_CS_Disable(hspi);
    HAL_SPI_Disable(hspi);
}

void HAL_SPI_Exchange(SPI_HandleTypeDef *hspi, uint8_t transmit_bytes[], uint8_t receive_bytes[], uint32_t count)
{
    HAL_SPI_Enable(hspi);

    for(uint32_t i = 0; i < count; i++)
    {    
        /* Ожидание когда TX_FIFO будет не заполнен - регистр заполнен меньше чем THRESHOLD */
        HAL_SPI_WaitTxNotFull(hspi);
        for (uint8_t offset = 0; offset < hspi->Init.DataSize + 1; offset++)
        {
            /* Загрузка байта */
            hspi->Instance->TxData = transmit_bytes[i + offset];
        }
        
  
        for (uint8_t offset = 0; offset < hspi->Init.DataSize + 1; offset++)
        {
            /* Ожидание когда в RX_FIFO появится хотябы один байт */
            HAL_SPI_WaitRxNotEmpty(hspi);
            /* Чтение байта */
            receive_bytes[i + offset] = hspi->Instance->RxData;
        }

        i = i + hspi->Init.DataSize;
    }

    HAL_SPI_Disable(hspi);
}