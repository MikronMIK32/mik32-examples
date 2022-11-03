#include "main.h"

CRC_HandleTypeDef hcrc;

void SystemClock_Config(void);
static void CRC_Init(void);

int main()
{    

    SystemClock_Config();

    CRC_Init();
 
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
    HAL_RCC_OscConfig(&RCC_OscInit);

    PeriphClkInit.PMClockAHB = PMCLOCKAHB_DEFAULT | PM_CLOCK_CRC32_M;    
    PeriphClkInit.PMClockAPB_M = PMCLOCKAPB_M_DEFAULT | PM_CLOCK_WU_M;     
    PeriphClkInit.PMClockAPB_P = PMCLOCKAPB_P_DEFAULT | PM_CLOCK_UART_0_M;     
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_NO_CLK;
    PeriphClkInit.RTCClockCPUSelection = RCC_RTCCLKCPUSOURCE_NO_CLK;
    HAL_RCC_ClockConfig(&PeriphClkInit);
}

static void CRC_Init(void)
{

    hcrc.Instance = CRC;
    hcrc.Poly = 0x00;                             
    hcrc.Init = 0x00;                                 
    hcrc.InputReverse = CRC_REVERSE_OFF;                          
    hcrc.OutputReverse = CRC_REVERSE_OFF;                        
    hcrc.OutputInversion = I2C_OUTPUTINVERSION_OFF;

    HAL_CRC_Init(&hcrc);

}