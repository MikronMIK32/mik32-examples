#ifndef SPI_LOOP_COMMON_H_INCLUDED
#define SPI_LOOP_COMMON_H_INCLUDED

#include <stdbool.h>
#include "spi_lib.h"
#include "epic.h"


bool TestSpiId(SPI_TypeDef* s);
void TestExchange(SPI_TypeDef* master, SPI_TypeDef* slave, uint32_t slave_epic_index);

void TestSpiIrq(SPI_TypeDef* _s, uint32_t _epic_index);

#endif

