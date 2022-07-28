
#include "common.h"
#include "spi_loop_common.h"
#include "epic.h"
#include "periphery_dma_common.h"


void CheckReceivedData(SPI_TypeDef* s, uint8_t expected_data[], uint32_t expected_count)
{
    uint8_t received_byte;

    for(int i = 0; i < expected_count; i++)
    {
        if ((s->IntStatus & SPI_RX_FIFO_not_empty_M) == 0)
        {
            TEST_ERROR("FIFO is empty at read number %d", i);
            return;
        }

        received_byte = s->RxData;
        if(received_byte != expected_data[i])
        {
            TEST_ERROR("unexpected data: 0x%02x Expected: 0x%02x", received_byte, expected_data[i]);
        }
    }

}


void TestExchange(SPI_TypeDef* master, SPI_TypeDef* slave, uint32_t slave_epic_index)
{
    uint32_t slave_eic_mask = ((uint32_t)1 << slave_epic_index);

    uint8_t master_data[] = {0xE2, 0xE4};
    uint8_t slave_data[] = {0xE5, 0xE7};
    // уровень при котором регистр TX считается незаполненым и формируется прерывание 
    master->TxThr = 1;
    slave->TxThr = 1;

    SPI_Init(slave, SPI_CONFIG_SLAVE_M);
    for(int i = 0; i < sizeof(slave_data); i++)
    {
        slave->TxData = slave_data[i];
    }

    slave->IntEnable = SPI_RX_FIFO_not_empty_M;

    SPI_Init(master, SPI_CONFIG_MASTER_M | SPI_CONFIG_BAUD_RATE_DIV_16_M );
    for(int i = 0; i < sizeof(master_data); i++)
    {
        master->TxData = master_data[i];
    }

    for(volatile int i = 0; i < 300; ++i) ;

    if ((EPIC->RAW_STATUS & slave_eic_mask) == 0)
    {
        TEST_ERROR("EPIC source [%d] is zero unexpectedly", slave_epic_index);
    } 
    slave->IntStatus = SPI_RX_FIFO_not_empty_M;
    slave->IntDisable = SPI_RX_FIFO_not_empty_M;
    if ((EPIC->RAW_STATUS & slave_eic_mask) != 0)
    {
        TEST_ERROR("EPIC source [%d] has not been cleared", slave_epic_index);
    }

    CheckReceivedData(master, slave_data, sizeof(slave_data));
    CheckReceivedData(slave, master_data, sizeof(master_data));    
}

bool TestSpiId(SPI_TypeDef* s)
{
    uint32_t test_pattern = 0x01090100;
    uint32_t read_value;

    read_value = s->ID;
    if (read_value != test_pattern)
    {
        TEST_ERROR("Unexpected ID register (0x%08x) value: 0x%0x Expected: 0x%0x)", (uint32_t)s, read_value, test_pattern);
        return false;
    }
    return true;
}



SPI_TypeDef* s;
uint32_t epic_index;
uint32_t epic_mask;
uint32_t trap_handler_called = 0;


void trap_handler()
{    
    xprintf("trap_handler()\n");
    
    trap_handler_called++;
    CheckTrapHandlerCalledOnce(trap_handler_called);
 
    if ((EPIC->RAW_STATUS & epic_mask) == 0)
    {
        TEST_ERROR("EPIC source [%d] is zero unexpectedly", epic_index);
    } 

    s->IntDisable = SPI_TX_FIFO_not_full_M;

    if ((EPIC->RAW_STATUS & epic_mask) != 0)
    {
        TEST_ERROR("EPIC source [%d] has not been cleared", epic_index);
    }     
    
    EPIC->CLEAR = epic_mask;    // clear flag in EPIC
}


void TestSpiIrq(SPI_TypeDef* _s, uint32_t _epic_index)
{   
    s = _s;
    epic_index = _epic_index;
    epic_mask = (1 << epic_index);
    EPIC->MASK_SET = 0xFFFFFFFF;
    EnableInterrupts();
    
    xprintf("Activate SPI interrupt\n");

    s->Enable = SPI_ENABLE_M;
    s->IntEnable = SPI_TX_FIFO_not_full_M;
    
    for(volatile int i = 0; i < 10; ++i) ;
    
    CheckTrapHandlerCalledOnce(trap_handler_called);    
}

