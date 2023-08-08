#include "mik32_hal_rcc.h"
#include "mik32_hal_pad_config.h"
#include "mik32_hal_i2c.h"
#include "mik32_hal_irq.h"

#include "csr.h"
#include "scr1_csr_encoding.h"

#include "uart_lib.h"
#include "xprintf.h"


I2C_HandleTypeDef hi2c0;

void SystemClock_Config(void);
static void I2C0_Init(void);


int main()
{    

    SystemClock_Config();
    HAL_PadConfig_PinMode(PORT0_5 | PORT0_6, PIN_MODE0);    /* Режим работы выводов UART0 */
    HAL_PadConfig_PinMode(PORT0_9 | PORT0_10, PIN_MODE0);   /* Режим работы выводов I2C */

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);
    I2C0_Init();

    HAL_EPIC_MaskLevelSet(HAL_EPIC_I2C_0_MASK); 
    HAL_IRQ_EnableInterrupts();

    uint8_t data[10];

    for(int i = 0; i < sizeof(data); i++)
    {
        data[i] = (uint8_t)i;
    }

    HAL_StatusTypeDef error_code;
    while (1)
    {    
        for(int i = 0; i < sizeof(data); i++)
        {
            data[i] = (uint8_t)i;
        }
        /* Передача данных по адресу 0x36 */
        xprintf("\nMaster_Transmit\n");
        error_code = HAL_I2C_Master_Transmit_IT(&hi2c0, 0x36, data, sizeof(data));
        if (error_code != HAL_OK)
        {
            xprintf("Master_Transmit error #%d, code %d, ISR %d\n", error_code, hi2c0.ErrorCode, hi2c0.Instance->ISR);
            HAL_I2C_Reset(&hi2c0);
        }
        
        while (hi2c0.State == HAL_I2C_STATE_BUSY);
        if (hi2c0.Init.AutoEnd == I2C_AUTOEND_DISABLE)
        {
            hi2c0.Instance->CR2 |= I2C_CR2_STOP_M;
        }
        for (volatile int i = 0; i < 1000000; i++); 


        /* Прием данных по адресу 0x36 */
        xprintf("\nMaster_Receive\n");
        error_code = HAL_I2C_Master_Receive_IT(&hi2c0, 0x36, data, sizeof(data));
        if (error_code != HAL_OK)
        {
            xprintf("Master_Receive error #%d, code %d, ISR %d\n", error_code, hi2c0.ErrorCode, hi2c0.Instance->ISR);
            HAL_I2C_Reset(&hi2c0);
        }
        
        while (hi2c0.State != HAL_I2C_STATE_END);
        if (hi2c0.Init.AutoEnd == I2C_AUTOEND_DISABLE)
        {
            hi2c0.Instance->CR2 |= I2C_CR2_STOP_M;
        }
        for(int i = 0; i < sizeof(data); i++)
        {
            xprintf("Data_read[%d] = %d\n", i, data[i]);
            data[i] = 0;
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
    RCC_OscInit.RTCClockSelection = RCC_RTCCLKSOURCE_NO_CLK;
    RCC_OscInit.RTCClockCPUSelection = RCC_RTCCLKCPUSOURCE_NO_CLK;
    HAL_RCC_OscConfig(&RCC_OscInit);

    PeriphClkInit.PMClockAHB = PMCLOCKAHB_DEFAULT;    
    PeriphClkInit.PMClockAPB_M = PMCLOCKAPB_M_DEFAULT | PM_CLOCK_WU_M | PM_CLOCK_EPIC_M;     
    PeriphClkInit.PMClockAPB_P = PMCLOCKAPB_P_DEFAULT | PM_CLOCK_UART_0_M | PM_CLOCK_I2C_0_M;     
    HAL_RCC_ClockConfig(&PeriphClkInit);
}

static void I2C0_Init(void)
{

    /* Общие настройки */
    hi2c0.Instance = I2C_0;

    hi2c0.Init.Mode = HAL_I2C_MODE_MASTER;

    hi2c0.Init.DigitalFilter = I2C_DIGITALFILTER_OFF;
    hi2c0.Init.AnalogFilter = I2C_ANALOGFILTER_DISABLE;
    hi2c0.Init.AutoEnd = I2C_AUTOEND_ENABLE;

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


void trap_handler()
{
    #ifdef MIK32_IRQ_DEBUG
    xprintf("\nTrap\n");
    xprintf("EPIC->RAW_STATUS = %d\n", EPIC->RAW_STATUS);
    xprintf("EPIC->STATUS = %d\n", EPIC->STATUS);
    #endif

    if (EPIC_CHECK_I2C_0()) 
    {
        HAL_I2C_IRQHandler(&hi2c0);
    }

    /* Сброс прерываний */
    HAL_EPIC_Clear(0xFFFFFFFF);


    #ifdef MIK32_IRQ_DEBUG
    xprintf("Clear\n");
    xprintf("EPIC->RAW_STATUS = %d\n", EPIC->RAW_STATUS);
    xprintf("EPIC->STATUS = %d\n", EPIC->STATUS);
    #endif
}