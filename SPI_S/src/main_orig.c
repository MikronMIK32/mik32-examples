#include "common.h"
#include "wakeup.h"
#include "power_manager.h"
#include <stdbool.h>
#include "spi_lib.h"

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

void CheckReceivedDatus(SPI_TypeDef* s, uint8_t expected_data[], uint32_t expected_count)
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

int main()
{
    
    SystemInit(); 
    PM->CLK_APB_P_SET = PM_CLOCK_SPI_0_M;
    
    //проверка ID 
    if (!TestSpiId(SPI_0))
    {
        return 0;
    }

    SPI_TypeDef* slave = SPI_0;

    uint8_t slave_input[] = {0};
    uint8_t master_data[] = {0xE2};
    uint8_t slave_data[] = {0xE5};

    // уровень при котором регистр TX считается незаполненым и формируется прерывание 
    slave->TxThr = 1;

    SPI_Init(slave, SPI_CONFIG_SLAVE_M);

    while (1)
    {
        SPI_Exchange(slave, slave_data, slave_input, sizeof(slave_data));
        CheckReceivedData(slave, master_data, sizeof(master_data));
    }
}