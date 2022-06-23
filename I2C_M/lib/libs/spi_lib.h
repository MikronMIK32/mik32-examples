#ifndef SPI_LIB_H_INCLUDED
#define SPI_LIB_H_INCLUDED

#include "spi.h"


void SPI_CS_Enable(SPI_TypeDef* SPI, unsigned int CS_M);
void SPI_CS_Disable(SPI_TypeDef* SPI, unsigned int CS_M);

void SPI_Init(SPI_TypeDef* s, uint32_t config);
void SPI_WaitTxNotFull(SPI_TypeDef* s);
int SPI_TimeoutWaitTxNotFull(SPI_TypeDef* s, uint32_t timeout);
void SPI_WaitRxNotEmpty(SPI_TypeDef* s);
int SPI_TimeoutWaitRxNotEmpty(SPI_TypeDef* s, uint32_t timeout);
void SPI_ClearRxBuffer(SPI_TypeDef* s);

void SPI_Exchange(SPI_TypeDef* s, uint8_t* input_bytes, uint8_t* output_bytes, uint32_t count);

#endif

