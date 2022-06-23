#ifndef DMA_LIB_H_INCLUDED
#define DMA_LIB_H_INCLUDED
/*
#define DMA_CHN_DONE_ERROR_NO	0
#define DMA_CHN_DONE_ERROR_SRC	1
#define DMA_CHN_DONE_ERROR_DST	2
#define DMA_CHN_DONE_ERROR_ENABLE	3
#define DMA_CHN_DONE_NOT_DONE	4
*/
#include "dma_config.h"

/*
    Следует пользоваться структурами задач для DMA так, чтобы они не выходили 
    из области видимости и память под ними не использовалась для других целей, 
    пока DMA точно не закончил работать с ними. 
    Например, это относится к созданию переменных этого типа в стеке.
*/
typedef struct
{
    uint32_t SrcAddress;
    uint32_t DestAddress;
    uint32_t LengthBytes;
    uint32_t Control;
} DMA_Task_t;


void DMA_StartNewTask(DMA_CONFIG_TypeDef* dma, uint32_t channel_index, 
                        uint32_t control_value, 
                        uint32_t source_address, uint32_t destination_address, uint32_t length);




void DMA_Wait(DMA_CONFIG_TypeDef* dma, uint32_t channel_index);

/*
void DMA_StartNewLlTask(DMA_CONFIG_TypeDef* dma, uint32_t channel_index, uint32_t task_address);

unsigned int DMA_Wait_Timeout (DMA_CONFIG_TypeDef* dma, uint32_t channel_index);
unsigned int DMA_ChnDone (DMA_CONFIG_TypeDef* dma, uint32_t channel_index);
unsigned int DMA_AllChnDone (DMA_CONFIG_TypeDef* dma);
*/
#endif

