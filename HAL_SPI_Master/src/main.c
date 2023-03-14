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

    UART_Init(UART_0, 3333, 
        UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);
    
    SPI0_Init();

    volatile uint8_t master_output[] = {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xB0, 0xB1};
    volatile uint8_t maser_input[sizeof(master_output)];

    while (1)
    {    
        /* Начало передачи в ручном режиме управления CS */
        if(hspi0.Init.ManualCS == SPI_MANUALCS_ON)
        {
            //HAL_SPI_Enable(&hspi0);
            HAL_SPI_CS_Enable(&hspi0, SPI_CS_0);
        }

        /* Передача и прием данных */
        HAL_SPI_Exchange(&hspi0, master_output, maser_input, sizeof(master_output));

        /* Конец передачи в ручном режиме управления CS */
        if(hspi0.Init.ManualCS == SPI_MANUALCS_ON)
        {
            HAL_SPI_CS_Disable(&hspi0);
            //HAL_SPI_Disable(&hspi0);
        }

        xprintf("Status = 0x%x\n", (uint8_t)hspi0.Instance->IntStatus);
        /* Вывод принятый данных и обнуление массива maser_input */
        for(uint32_t i = 0; i < sizeof(maser_input); i++)
        {
            xprintf("maser_input[%d] = %02x\n", i, maser_input[i]);
            maser_input[i] = 0;
        }

        for (volatile int i = 0; i < 1000000; i++);
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
    hspi0.Init.SPI_Mode = HAL_SPI_MODE_MASTER;

    /* Настройки */    
    hspi0.Init.BaudRateDiv = SPI_BAUDRATE_DIV64;                    
    hspi0.Init.CLKPhase = SPI_PHASE_OFF;            
    hspi0.Init.CLKPolarity = SPI_POLARITY_LOW;         
    hspi0.Init.Decoder = SPI_DECODER_NONE;
    hspi0.Init.DataSize = SPI_DATASIZE_8BITS;  

    /* Натсройки для ведущего */
    hspi0.Init.ManualCS = SPI_MANUALCS_OFF;     /* Настройки ручного режима управления сигналом CS */
    hspi0.ChipSelect = SPI_CS_0;                /* Выбор ведомого устройства в атоматическом режиме управления CS */

    HAL_SPI_Init(&hspi0);

}
