#ifndef PERIPHERY_DMA_COMMON_H_INCLUDED
#define PERIPHERY_DMA_COMMON_H_INCLUDED

#include "dma_config.h"


#define TEST_DATA_SIZE      128
#define WRITING_CHANNEL     0
#define READING_CHANNEL     1


extern uint8_t test_data[TEST_DATA_SIZE];
extern uint8_t check_data[TEST_DATA_SIZE];

void WaitDmaChannels(DMA_CONFIG_TypeDef* dma);

uint8_t GetTestData(int i);
void CheckData(int count);

void InitDmaChannels(
    DMA_CONFIG_TypeDef* dma, 
    void* tx_address, void* rx_address, 
    int dma_request_index, uint32_t count,
    uint32_t common_config);

#endif

