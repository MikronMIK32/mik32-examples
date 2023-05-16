#include "mik32_hal_rcc.h"
#include "mik32_hal_spi.h"

#include "uart_lib.h"
#include "xprintf.h"

#include "pad_config.h"


SPI_HandleTypeDef hspi0;

void SystemClock_Config(void);
static void SPI0_Init(void);

int main()
{    

    SystemClock_Config();

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    /* Подтяжка к питанию вывода Port0.3 - SPI0_CS_IN */
    PAD_CONFIG->PORT_0_PUD |= 01 << (2 * 3);
    
    SPI0_Init();

    uint8_t master_output[] = {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xB0, 0xB1};
    uint8_t master_input[sizeof(master_output)];

    while (1)
    {    
        /* Начало передачи в ручном режиме управления CS */
        if(hspi0.Init.ManualCS == SPI_MANUALCS_ON)
        {
            HAL_SPI_Enable(&hspi0);
            HAL_SPI_CS_Enable(&hspi0, SPI_CS_0);
        }

        /* Передача и прием данных */
        HAL_StatusTypeDef SPI_Status = HAL_SPI_Exchange(&hspi0, master_output, master_input, sizeof(master_output), SPI_TIMEOUT_DEFAULT);
        if (SPI_Status != HAL_OK)
        {
            xprintf("SPI_Error %d, OVR %d, MODF %d\n", SPI_Status, hspi0.Error.RXOVR, hspi0.Error.ModeFail);
            HAL_SPI_ClearError(&hspi0);
        }
         
        /* Конец передачи в ручном режиме управления CS */
        if(hspi0.Init.ManualCS == SPI_MANUALCS_ON)
        {
            HAL_SPI_CS_Disable(&hspi0);
            HAL_SPI_Disable(&hspi0);
        }

        /* Вывод принятый данных и обнуление массива master_input */
        for(uint32_t i = 0; i < sizeof(master_input); i++)
        {
            xprintf("master_input[%d] = 0x%02x\n", i, master_input[i]);
            master_input[i] = 0;
        }
        xprintf("\n");

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
    PeriphClkInit.PMClockAPB_M = PMCLOCKAPB_M_DEFAULT | PM_CLOCK_WU_M | PM_CLOCK_PAD_CONFIG_M;     
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
    hspi0.Init.ThresholdTX = SPI_THRESHOLD_DEFAULT;  

    /* Настройки для ведущего */
    hspi0.Init.ManualCS = SPI_MANUALCS_OFF;     /* Настройки ручного режима управления сигналом CS */
    hspi0.ChipSelect = SPI_CS_0;                /* Выбор ведомого устройства в автоматическом режиме управления CS */

    if ( HAL_SPI_Init(&hspi0) != HAL_OK )
    {
        xprintf("SPI_Init_Error\n");
    }

}
