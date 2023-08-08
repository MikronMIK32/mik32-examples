#include "mik32_hal_rcc.h"
#include "mik32_hal_spi.h"
#include "mik32_hal_irq.h"

#include "uart_lib.h"
#include "xprintf.h"

#include "pad_config.h"


SPI_HandleTypeDef hspi0;

void SystemClock_Config(void);
static void SPI0_Init(void);

int main()
{    

    SystemClock_Config();

    HAL_EPIC_MaskLevelSet(HAL_EPIC_SPI_0_MASK); 
    HAL_IRQ_EnableInterrupts();

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    SPI0_Init();
    // HAL_SPI_SetDelayBTWN(&hspi0, 1);
    // HAL_SPI_SetDelayAFTER(&hspi0, 0);
    // HAL_SPI_SetDelayINIT(&hspi0, 100);

    uint8_t slave_output[] = {0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xC0, 0xC1};
    uint8_t slave_input[sizeof(slave_output)]; 
    for (uint32_t i = 0; i < sizeof(slave_input); i++)
    {
        slave_input[i] = 0;
    }

    while (1)
    {    
        if (hspi0.State == HAL_SPI_STATE_READY) 
        {
            /* Передача и прием данных */
            HAL_StatusTypeDef SPI_Status = HAL_SPI_Exchange_IT(&hspi0, slave_output, slave_input, sizeof(slave_output));
            if (SPI_Status != HAL_OK)
            {
                HAL_SPI_ClearError(&hspi0);
                hspi0.State = HAL_SPI_STATE_READY;
            } 
        }

        if (hspi0.State == HAL_SPI_STATE_END)
        {
            /* Вывод принятый данных и обнуление массива slave_input */
            for(uint32_t i = 0; i < sizeof(slave_input); i++)
            {
                xprintf("slave_input[%d] = 0x%02x\n", i, slave_input[i]);
                slave_input[i] = 0;
            }
            xprintf("\n");

            hspi0.State = HAL_SPI_STATE_READY;
        }

        if (hspi0.State == HAL_SPI_STATE_ERROR)
        {
            xprintf("SPI_Error: OVR %d, MODF %d\n", hspi0.Error.RXOVR, hspi0.Error.ModeFail);
            HAL_SPI_Disable(&hspi0);
            hspi0.State = HAL_SPI_STATE_READY;
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
    PeriphClkInit.PMClockAPB_M = PMCLOCKAPB_M_DEFAULT | PM_CLOCK_WU_M | PM_CLOCK_PAD_CONFIG_M | PM_CLOCK_EPIC_M;     
    PeriphClkInit.PMClockAPB_P = PMCLOCKAPB_P_DEFAULT | PM_CLOCK_UART_0_M | PM_CLOCK_SPI_0_M;
    HAL_RCC_ClockConfig(&PeriphClkInit);
}

static void SPI0_Init(void)
{
    hspi0.Instance = SPI_0;

    /* Режим SPI */
    hspi0.Init.SPI_Mode = HAL_SPI_MODE_SLAVE;

    /* Настройки */                       
    hspi0.Init.CLKPhase = SPI_PHASE_ON;            
    hspi0.Init.CLKPolarity = SPI_POLARITY_LOW;      
    hspi0.Init.ThresholdTX = 1;

    if ( HAL_SPI_Init(&hspi0) != HAL_OK )
    {
        xprintf("SPI_Init_Error\n");
    }

}

void trap_handler()
{
    #ifdef MIK32_IRQ_DEBUG
    xprintf("\nTrap\n");
    xprintf("EPIC->RAW_STATUS = %d\n", EPIC->RAW_STATUS);
    xprintf("EPIC->STATUS = %d\n", EPIC->STATUS);
    #endif

    if (EPIC_CHECK_SPI_0())
    {
        HAL_SPI_IRQHandler(&hspi0);
    }

    /* Сброс прерываний */
    HAL_EPIC_Clear(0xFFFFFFFF);


    #ifdef MIK32_IRQ_DEBUG
    xprintf("Clear\n");
    xprintf("EPIC->RAW_STATUS = %d\n", EPIC->RAW_STATUS);
    xprintf("EPIC->STATUS = %d\n", EPIC->STATUS);
    #endif
}