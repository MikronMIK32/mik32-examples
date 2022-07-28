
#include "common.h"
#include "periphery_dma_common.h"


uint8_t test_data[TEST_DATA_SIZE] = {
    0xAF, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 
    0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
};
uint8_t check_data[TEST_DATA_SIZE] = {0};


void WaitDmaChannels(DMA_CONFIG_TypeDef* dma)
{
    int timeout = 1000;
    
    while (((dma->ConfigStatus & DMA_STATUS_READY(WRITING_CHANNEL)) == 0) &&
           (--timeout > 0)) {}
           
    while (((dma->ConfigStatus & DMA_STATUS_READY(READING_CHANNEL)) == 0) &&
           (--timeout > 0)) {}
           
    if (timeout <= 0)
    {
        TEST_ERROR("DMA is not ready so far");
    }
}


uint8_t GetTestData(int i)
{
    if ((i >= 0) && (i < TEST_DATA_SIZE))
    {
        return test_data[i];
    }
    else
    {
        TEST_ERROR("internal error: test_data index %d is out of range", i);
        return 0;
    }
}

void CheckData(int count)
{
    for(int i = 0; i < count; i++)
    {
        if(check_data[i] != test_data[i])
        {
            TEST_ERROR("Data [%d] does not match (0x%02x vs 0x%02x)", 
                       i, check_data[i], test_data[i]);
        }
        check_data[i] = 0;
    }
}


void InitDmaChannels(
    DMA_CONFIG_TypeDef* dma, 
    void* tx_address, void* rx_address, 
    int dma_request_index, uint32_t count,
    uint32_t common_config
)
{
    if(count > TEST_DATA_SIZE)
    {
        TEST_ERROR("internal error: test data size is not enough.");
    }
    
    dma->ConfigStatus = 0xFFFFFFFF;
    common_config |= DMA_CFG_CH_ENABLE_M | 
        DMA_CFG_CH_WRITE_REQ(dma_request_index) |
        DMA_CFG_CH_READ_REQ(dma_request_index);
    
    DMA_CHANNEL_TypeDef* dma_ch = &(dma->CHANNELS[WRITING_CHANNEL]);
    dma_ch->SOURCE = (uint32_t)test_data;
    dma_ch->DESTINATIONS = (uint32_t)(tx_address);
    dma_ch->LEN = count-1;
    dma_ch->CONFIG = common_config | 
        DMA_CFG_CH_READ_MODE_memory_M | DMA_CFG_CH_READ_INCREMENT_M |
        DMA_CFG_CH_WRITE_MODE_periphery_M | DMA_CFG_CH_WRITE_no_INCREMENT_M;
    
    dma_ch = &(dma->CHANNELS[READING_CHANNEL]);
    dma_ch->SOURCE = (uint32_t)(rx_address);
    dma_ch->DESTINATIONS = (uint32_t)check_data;
    dma_ch->LEN = count-1;
    dma_ch->CONFIG = common_config | 
        DMA_CFG_CH_READ_MODE_periphery_M | DMA_CFG_CH_READ_no_INCREMENT_M |
        DMA_CFG_CH_WRITE_MODE_memory_M | DMA_CFG_CH_WRITE_INCREMENT_M;
}
