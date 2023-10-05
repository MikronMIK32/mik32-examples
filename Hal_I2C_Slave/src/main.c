#include "mik32_hal_rcc.h"
#include "mik32_hal_pad_config.h"
#include "mik32_hal_i2c.h"


#include "uart_lib.h"
#include "xprintf.h"


I2C_HandleTypeDef hi2c0;

void SystemClock_Config(void);
static void I2C0_Init(void);

int main()
{    

    SystemClock_Config();

    HAL_PadConfig_PinMode(PORT0_5 | PORT0_6, PIN_MODE_SERIAL);    /* Режим работы выводов UART0 */
    HAL_PadConfig_PinMode(PORT0_9 | PORT0_10, PIN_MODE_SERIAL);   /* Режим работы выводов I2C */

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);
    I2C0_Init();


    uint8_t data[10];

    for(int i = 0; i < sizeof(data); i++)
    {
        data[i] = (uint8_t)i;
    }

    HAL_StatusTypeDef error_code;
    while (1)
    {    

        /* Чтение данных */
        xprintf("\nSlave_read\n");
        error_code = HAL_I2C_Slave_Receive(&hi2c0, data, sizeof(data), 5*I2C_TIMEOUT_DEFAULT);
        if (error_code != HAL_OK)
        {
            xprintf("Slave_Receive error #%d, code %d, ISR %d\n", error_code, hi2c0.ErrorCode, hi2c0.Instance->ISR);
            HAL_I2C_Reset(&hi2c0);
        }
        else
        {
            for(int i = 0; i < sizeof(data); i++)
            {
                xprintf("Data_read[%d] = %d\n", i, data[i]);
                data[i] = 0;
            }
        }



        for(int i = 0; i < sizeof(data); i++)
        {
            data[i] = (uint8_t)i;
        }
        /* Запись данных */
        xprintf("\nSlave_write\n");
        error_code = HAL_I2C_Slave_Transmit(&hi2c0, data, sizeof(data), 5*I2C_TIMEOUT_DEFAULT);
        if (error_code != HAL_OK)
        {
            xprintf("Slave_Transmit error #%d, code %d, ISR %d\n", error_code, hi2c0.ErrorCode, hi2c0.Instance->ISR);
            HAL_I2C_Reset(&hi2c0);
        }

    }
       
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInit = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    RCC_OscInit.OscillatorEnable = RCC_OSCILLATORTYPE_OSC32K | RCC_OSCILLATORTYPE_OSC32M;   
    RCC_OscInit.OscillatorSystem = RCC_OSCILLATORTYPE_OSC32M;                          
    RCC_OscInit.AHBDivider = 0;                             
    RCC_OscInit.APBMDivider = 0;                             
    RCC_OscInit.APBPDivider = 0;                             
    RCC_OscInit.HSI32MCalibrationValue = 0;                  
    RCC_OscInit.LSI32KCalibrationValue = 0;
    RCC_OscInit.RTCClockSelection = RCC_RTCCLKSOURCE_NO_CLK;
    RCC_OscInit.RTCClockCPUSelection = RCC_RTCCLKCPUSOURCE_NO_CLK;
    HAL_RCC_OscConfig(&RCC_OscInit);

    PeriphClkInit.PMClockAHB = PMCLOCKAHB_DEFAULT;    
    PeriphClkInit.PMClockAPB_M = PMCLOCKAPB_M_DEFAULT | PM_CLOCK_APB_M_WU_M;     
    PeriphClkInit.PMClockAPB_P = PMCLOCKAPB_P_DEFAULT | PM_CLOCK_APB_P_UART_0_M | PM_CLOCK_APB_P_I2C_0_M;     
    HAL_RCC_ClockConfig(&PeriphClkInit);
}

static void I2C0_Init(void)
{

    /*Общие настройки*/
    hi2c0.Instance = I2C_0;

    hi2c0.Init.Mode = HAL_I2C_MODE_SLAVE;
    
    hi2c0.Init.DigitalFilter = I2C_DIGITALFILTER_OFF;
    hi2c0.Init.AnalogFilter = I2C_ANALOGFILTER_DISABLE;
    hi2c0.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    hi2c0.Init.OwnAddress1 = 0x36;
    hi2c0.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c0.Init.OwnAddress2Mask = I2C_OWNADDRESS2_MASK_DISABLE;
    hi2c0.Init.OwnAddress2 = 0x55;
    hi2c0.Init.GeneralCall = I2C_GENERALCALL_DISABLE;
    hi2c0.Init.SBCMode = I2C_SBC_DISABLE; 


    /* Настройка частоты */
    hi2c0.Clock.PRESC = 15;
    hi2c0.Clock.SCLDEL = 10;
    hi2c0.Clock.SDADEL = 15;
    hi2c0.Clock.SCLH = 15;
    hi2c0.Clock.SCLL = 15;


    if (HAL_I2C_Init(&hi2c0) != HAL_OK)
    {
        xprintf("I2C_Init error\n");
    }

}

HAL_StatusTypeDef HAL_I2C_Slave_SBC(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t DataSize, uint32_t ByteCount)
{
    if (ByteCount == 5)
    {
        HAL_I2C_Slave_NACK(hi2c);
        return HAL_ERROR;
    }
    
    
    HAL_I2C_Slave_ACK(hi2c);

    return HAL_OK;
}