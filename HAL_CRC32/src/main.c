#include "mik32_hal_rcc.h"
#include "mik32_hal_crc32.h"

#include "uart_lib.h"
#include "xprintf.h"


CRC_HandleTypeDef hcrc;

void SystemClock_Config(void);
static void CRC_Init(void);

int main()
{    

    SystemClock_Config();

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    CRC_Init();

    uint8_t  message[] ={'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    uint32_t data[] = {0xABCDABCD, 0xA1B2C3D4};
    uint32_t CRCValue = 0;
    
    /* Запись по байтам */
    HAL_RTC_WriteData(&hcrc, message, sizeof(message));
    CRCValue = HAL_RTC_ReadCRC(&hcrc);
    xprintf("CRC32 = 0x%08x, ожидалось 0x3010BF7F\n", CRCValue);
    
    /* Запись по словам */
    HAL_RTC_WriteData32(&hcrc, data, sizeof(data)/sizeof(*data));
    CRCValue = HAL_RTC_ReadCRC(&hcrc);
    xprintf("CRC32 = 0x%08x, ожидалось 0x6311BC18\n", CRCValue);
    

    while (1)
    {    

    }
       
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInit = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    RCC_OscInit.OscillatorEnable = RCC_OSCILLATORTYPE_OSC32K | RCC_OSCILLATORTYPE_OSC32M; // Включение 32К и 32М источников
    RCC_OscInit.OscillatorSystem = RCC_OSCILLATORTYPE_OSC32M;                          
    RCC_OscInit.AHBDivider = 0;                             
    RCC_OscInit.APBMDivider = 0;                             
    RCC_OscInit.APBPDivider = 0;                             
    RCC_OscInit.HSI32MCalibrationValue = 0;                  
    RCC_OscInit.LSI32KCalibrationValue = 0;
    RCC_OscInit.RTCClockSelection = RCC_RTCCLKSOURCE_NO_CLK;
    RCC_OscInit.RTCClockCPUSelection = RCC_RTCCLKCPUSOURCE_NO_CLK;
    HAL_RCC_OscConfig(&RCC_OscInit);

    PeriphClkInit.PMClockAHB = PMCLOCKAHB_DEFAULT | PM_CLOCK_CRC32_M;    
    PeriphClkInit.PMClockAPB_M = PMCLOCKAPB_M_DEFAULT | PM_CLOCK_WU_M;     
    PeriphClkInit.PMClockAPB_P = PMCLOCKAPB_P_DEFAULT | PM_CLOCK_UART_0_M;     
    HAL_RCC_ClockConfig(&PeriphClkInit);
}

static void CRC_Init(void)
{

    hcrc.Instance = CRC;
    
    /* Настройки вычисления CRC */                    
    /*******************************************CRC-32Q*******************************************/
    hcrc.Poly = 0x814141AB;                             
    hcrc.Init = 0x00000000;                                 
    hcrc.InputReverse = CRC_REFIN_FALSE;                 
    hcrc.OutputReverse = CRC_REFOUT_FALSE;                          
    hcrc.OutputInversion = CRC_OUTPUTINVERSION_OFF;

    HAL_CRC_Init(&hcrc);

}