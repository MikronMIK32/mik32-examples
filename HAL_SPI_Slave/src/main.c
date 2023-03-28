#include "mik32_hal_rcc.h"
#include "mik32_hal_spi.h"

#include "uart_lib.h"
#include "xprintf.h"

SPI_HandleTypeDef hspi0;

void SystemClock_Config(void);
static void SPI0_Init(void);

int main()
{    

    SystemClock_Config();

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);
    
    SPI0_Init();

    uint8_t slave_output[] = {0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xC0, 0xC1};
    uint8_t slave_input[sizeof(slave_output)]; 
    for (uint32_t i = 0; i < sizeof(slave_input); i++)
    {
        slave_input[i] = 0;
    }

    while (1)
    {    
        /* Передача и прием данных */
        HAL_SPI_Exchange(&hspi0, slave_output, slave_input, sizeof(slave_output));
              
        xprintf("Status = 0x%x\n", (uint8_t)hspi0.Instance->IntStatus); 
        /* Вывод принятый данных и обнуление массива slave_input */
        for(uint32_t i = 0; i < sizeof(slave_input); i++)
        {
            xprintf("slave_input[%d] = %02x\n", i, slave_input[i]);
            slave_input[i] = 0;
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
    HAL_RCC_OscConfig(&RCC_OscInit);

    PeriphClkInit.PMClockAHB = PMCLOCKAHB_DEFAULT;    
    PeriphClkInit.PMClockAPB_M = PMCLOCKAPB_M_DEFAULT | PM_CLOCK_WU_M;     
    PeriphClkInit.PMClockAPB_P = PMCLOCKAPB_P_DEFAULT | PM_CLOCK_UART_0_M | PM_CLOCK_SPI_0_M;
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_NO_CLK;
    PeriphClkInit.RTCClockCPUSelection = RCC_RTCCLKCPUSOURCE_NO_CLK;
    HAL_RCC_ClockConfig(&PeriphClkInit);
}

static void SPI0_Init(void)
{
    hspi0.Instance = SPI_0;

    /* Режим SPI */
    hspi0.Init.SPI_Mode = HAL_SPI_MODE_SLAVE;

    /* Настройки */                       
    hspi0.Init.CLKPhase = SPI_PHASE_OFF;            
    hspi0.Init.CLKPolarity = SPI_POLARITY_LOW;         
    hspi0.Init.DataSize = SPI_DATASIZE_8BITS;  

    HAL_SPI_Init(&hspi0);

}
