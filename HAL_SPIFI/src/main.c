#include "mik32_hal_spifi.h"
#include "mik32_hal_w25.h"

#include "uart_lib.h"
#include "xprintf.h"

int main()
{
    UART_Init(UART_0, 277, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);
    xprintf("start\n");

    SPIFI_HandleTypeDef spifi = {
        .Instance = SPIFI_CONFIG};

    HAL_SPIFI_MspInit(&spifi);

    HAL_SPIFI_Reset(&spifi);

    xprintf("SREG1 = %d\n", HAL_W25_ReadSREG(&spifi, W25_SREG1));
    xprintf("SREG2 = %d\n", HAL_W25_ReadSREG(&spifi, W25_SREG2));

    HAL_W25_WriteSREG_Volatile(&spifi, 0, 0);
    xprintf("WAIT BUSY RESELT %d\n", HAL_W25_WaitBusy(&spifi, 10000));

    xprintf("SREG1 = %d\n", HAL_W25_ReadSREG(&spifi, W25_SREG1));
    xprintf("SREG2 = %d\n", HAL_W25_ReadSREG(&spifi, W25_SREG2));

    SPIFI_CommandTypeDef cmd_read = {
        .Direction = SPIFI_DIRECTION_INPUT,
        .FieldForm = SPIFI_FIELDFORM_ALL_SERIAL,
        .FrameForm = SPIFI_FRAMEFORM_OPCODE_3ADDR,
        .InterimData = 0,
        .InterimLength = 0,
        .OpCode = 0x03,
    };

    SPIFI_CommandTypeDef cmd_read_quad = {
        .Direction = SPIFI_DIRECTION_INPUT,
        .FieldForm = SPIFI_FIELDFORM_DATA_PARALLEL,
        .FrameForm = SPIFI_FRAMEFORM_OPCODE_3ADDR,
        .InterimData = 0,
        .InterimLength = 1,
        .OpCode = 0x6B,
    };

    while (1)
        ;

    xprintf("end\n");

    while (1)
    {
        for (int k = 0; k < 8; k++)
        {
            xprintf("\n\nPage: %d\n", k);

            uint8_t data[16] = {0};

            xprintf("\naddr| ");
            for (int i = 0; i < sizeof(data); i++)
            {
                xprintf("%02x ", i);
            }
            xprintf("\n----+------------------------------------------------\n");

            for (int i = 0; i < 256; i += sizeof(data))
            {
                HAL_SPIFI_SendCommand(&spifi, &cmd_read, i + 256 * k, sizeof(data), data);

                xprintf("%04x| ", i);
                for (int j = 0; j < sizeof(data); j++)
                {
                    xprintf("%02x ", data[j]);
                }
                xprintf("\n");
            }

            for (volatile int i = 0; i < 2000000; i++)
                ;
        }
    }
}
