

#include "common.h"
#include "i2c_loop_common.h"


void Init(I2C_TypeDef* i2c)
{
    i2c->CR1 = 0;
    i2c->TIMINGR = I2C_TIMINGR_SCLDEL(1) | I2C_TIMINGR_SDADEL(1) |
        I2C_TIMINGR_SCLL(20) | I2C_TIMINGR_SCLH(20) | I2C_TIMINGR_PRESC(3); //частота 164,7 кГц tsync12 = 10^(-6)
    i2c->CR1 = I2C_CR1_PE_M;
}


void WaitTxe(I2C_TypeDef* i2c)
{
    for(volatile int i = 0; i < 100; i++)
    {
        if (i2c->ISR & I2C_ISR_TXE_M)
        {
            break;
        }
    }
}

void WaitRxne(I2C_TypeDef* i2c)
{
    for(volatile int i = 0; i < 100; i++)
    {
        if (i2c->ISR & I2C_ISR_RXNE_M)
        {
            break;
        }
    }
}


void TestI2cLoop(I2C_TypeDef* i2c)
{
    uint8_t test_address = 0x36;

    uint8_t test_bytes[1];
    test_bytes[0]=1;
    // test_bytes[0]=0xAA;
    // test_bytes[1]=0x55;
    // test_bytes[2]=0xFF;
    // test_bytes[3]=0x00;

    uint8_t received_byte;
    int i;

    Init(i2c);
    
    xprintf("I2C writes some bytes: \n");
    i2c->CR2 = I2C_CR2_SADD(test_address) | I2C_CR2_WR_M |
               I2C_CR2_NBYTES(sizeof(test_bytes));

    i = 0;
    i2c->TXDR = test_bytes[0];
    xprintf("0x%02x ", test_bytes[0]);
    i2c->CR2 |= I2C_CR2_START_M;
    // while(i < sizeof(test_bytes))
    // {
    //     xprintf("0x%02x ", test_bytes[i]);
    //     i2c->TXDR = test_bytes[i++];
    //     WaitTxe(i2c);
    // }
    for (volatile int i = 0; i < 500; i++);
    i2c->CR2 |= I2C_CR2_STOP_M;
    
    for (volatile int i = 0; i < 10000; i++);
    
    xprintf("\nI2C reads bytes back\n");
    i2c->CR2 = I2C_CR2_SADD(test_address) | I2C_CR2_RD_M |
               I2C_CR2_NBYTES(sizeof(test_bytes));
    i2c->CR2 |= I2C_CR2_START_M;

    for (int i = 0; i < sizeof(test_bytes); i++)
    {
        WaitRxne(i2c);
        received_byte = i2c->RXDR;
    }
    for (volatile int i = 0; i < 500; i++);
    i2c->CR2 |= I2C_CR2_STOP_M;
}
