

#include "common.h"
#include "i2c_irq_common.h"


I2C_TypeDef* i2c;
uint32_t epic_index;
uint32_t trap_handler_called = 0;


void trap_handler()
{
    uint32_t epic_mask;
    
    xprintf("trap_handler()\n");
    
    trap_handler_called++;
    CheckTrapHandlerCalledOnce(trap_handler_called);
    
    epic_mask = (1 << epic_index);
    epic_mask = 1<<6;
    if ((EPIC->RAW_STATUS & epic_mask) == 0)
    {
        TEST_ERROR("EIC source [%d] is zero unexpectedly", epic_index);
    } 
    i2c->ICR = 0xFFFFFFFF;
    if ((EPIC->RAW_STATUS & epic_mask) != 0)
    {
        TEST_ERROR("EPIC source [%d] has not been cleared", epic_index);
    } 
    EPIC->CLEAR = epic_mask;    // clear flag in EPIC
}


void Init(I2C_TypeDef* i2c)
{
    i2c->CR1 = 0;
    i2c->TIMINGR = I2C_TIMINGR_SCLDEL(1) | I2C_TIMINGR_SDADEL(1) |
        I2C_TIMINGR_SCLL(2) | I2C_TIMINGR_SCLH(2) | I2C_TIMINGR_PRESC(1);
    i2c->CR1 = I2C_CR1_PE_M | I2C_CR1_STOPIE_M;
}

void Wait(I2C_TypeDef* i2c)
{
    for(volatile int i = 0; i < 100; i++)
    {
        if (i2c->ISR & I2C_ISR_STOPF_M)
        {
            break;
        }
    }
}

void TestI2cIrq(I2C_TypeDef* _i2c, uint32_t _epic_index)
{
    EnableAllClocks();
    uint32_t epic_mask;
    
    i2c = _i2c;
    epic_index = _epic_index;
    EPIC->MASK_SET = 0xFFFFFFFF;
    EnableInterrupts();
    
    xprintf("Activate I2C interrupt\n");    
    
    Init(i2c);
    
    i2c->CR2 = I2C_CR2_SADD(0) | I2C_CR2_WR_M | I2C_CR2_NBYTES(0);
    i2c->CR2 |= I2C_CR2_START_M;
 
    Wait(i2c);
    
    CheckTrapHandlerCalledOnce(trap_handler_called);
}
