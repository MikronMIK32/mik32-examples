
#include "spi_lib.h"


void SPI_CS_Enable(SPI_TypeDef* SPI, unsigned int CS_M)  {
	SPI->Config = (SPI->Config & ~SPI_CONFIG_CS_M) | CS_M;
}


void SPI_CS_Disable(SPI_TypeDef* SPI, unsigned int CS_M)  {
	SPI->Config = (SPI->Config & ~SPI_CONFIG_CS_M) | SPI_CONFIG_CS_NONE_M;
}


void SPI_Init(SPI_TypeDef* s, uint32_t config)
{
	s->Enable = 0;
    SPI_ClearRxBuffer(s);
    s->Config = config;
	s->Enable = SPI_ENABLE_M;
}


void SPI_WaitTxNotFull(SPI_TypeDef* s)
{
    while (s->IntStatus & SPI_TX_FIFO_not_full_M == 0) ;
}

int SPI_TimeoutWaitTxNotFull(SPI_TypeDef* s, uint32_t timeout)
{
    while ((s->IntStatus & SPI_TX_FIFO_not_full_M) == 0)
    {
        if (timeout-- == 0)
        {
            return -1;
        }
    }
    return 0;
}


void SPI_WaitRxNotEmpty(SPI_TypeDef* s)
{
    while ((s->IntStatus & SPI_RX_FIFO_not_empty_M) == 0) ;
}


int SPI_TimeoutWaitRxNotEmpty(SPI_TypeDef* s, uint32_t timeout)
{
    while ((s->IntStatus & SPI_RX_FIFO_not_empty_M) == 0)
    {
        if (timeout-- == 0)
        {
            return -1;
        }
    }
    return 0;
}


void SPI_ClearRxBuffer(SPI_TypeDef* s)
{
    uint32_t dummy;
    while ((s->IntStatus & SPI_RX_FIFO_not_empty_M) != 0)
    {
        dummy = s->RxData;
    }
}


void SPI_Exchange(SPI_TypeDef* s, uint8_t* input_bytes, uint8_t* output_bytes, uint32_t count)
{
    while (count-- > 0)
    {
        SPI_WaitTxNotFull(s);
        s->TxData = *input_bytes++;
        SPI_WaitRxNotEmpty(s);
        if (output_bytes != 0)
        {
            *output_bytes++ = s->RxData;
        }
        else
        {
            (void)s->RxData;
        }
    }

}
















