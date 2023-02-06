#include "main.h"


OTP_HandleTypeDef hotp;

void SystemClock_Config(void);
static void OTP_Init(void);


int main()
{    

    SystemClock_Config();

    OTP_Init();

    uint8_t address = 0;

    uint32_t otp_data_writeTestRow = 0x12345678;
    uint32_t otp_data_writeTestColumn[8] = {0b1, 0b0, 0b1, 0b1, 0b1, 0b1, 0b0, 0b1};
    uint32_t otp_data_writeTestBit = 1;
    uint32_t otp_data_write[8] = {0xAAAAAAAA, 0xBBBBBBBB, 0xCCCCCCCC, 0xDDDDDDDD, 0xEEEEEEEE, 0xFFFFFFFF, 0xAAAABBBB, 0xCCCCDDDD};

    uint32_t otp_data_readTestRow = 0;
    uint32_t otp_data_readTestColumn[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint32_t otp_data_readTestBit = 0;
    uint32_t otp_data_read[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint32_t data_length = sizeof(otp_data_read)/sizeof(*otp_data_read);

    /*********************************** Запись ***********************************/
    #ifdef MIK32_OTP_DEBUG
    xprintf("Write\n");
    #endif
    
    HAL_OTP_Write(&hotp, address, otp_data_write, data_length);
    HAL_OTP_WriteTestRow(&hotp, otp_data_writeTestRow);
    HAL_OTP_WriteTestColumn(&hotp, address, otp_data_writeTestColumn, data_length);
    HAL_OTP_WriteTestBit(&hotp, otp_data_writeTestBit);

    #ifdef MIK32_OTP_DEBUG
    for (uint32_t i = 0; i < data_length; i++)
    {
        xprintf("otp_data_write[%d] = 0x%08x\n", i, otp_data_write[i]);
    }

    xprintf("\notp_data_writeTestRow = 0x%08x\n\n", otp_data_writeTestRow);

    for (uint32_t i = 0; i < data_length; i++)
    {
        xprintf("otp_data_writeTestColumn[%d] = 0x%08x\n", i, otp_data_writeTestColumn[i]);
    }

    xprintf("\notp_data_writeTestBit = 0x%08x\n\n", otp_data_writeTestBit);
    #endif

    /* После окончания операции записи(чтения) до начала следующей за ней операции чтения(записи) должно пройти не менее 1 мкс */
    for (volatile int i = 0; i < 1000000; i++); 

    /*********************************** Чтение ***********************************/
    #ifdef MIK32_OTP_DEBUG
    xprintf("Read\n");
    #endif
    
    HAL_OTP_Read(&hotp, address, otp_data_read, data_length);
    otp_data_readTestRow = HAL_OTP_ReadTestRow(&hotp);
    HAL_OTP_ReadTestColumn(&hotp, address, otp_data_readTestColumn, data_length);
    otp_data_readTestBit = HAL_OTP_ReadTestBit(&hotp);

    #ifdef MIK32_OTP_DEBUG
    for (uint32_t i = 0; i < data_length; i++)
    {
        xprintf("otp_data_read[%d] = 0x%08x\n", i, otp_data_read[i]);
    }

    xprintf("\notp_data_readTestRow = 0x%08x\n\n", otp_data_readTestRow);

    for (uint32_t i = 0; i < data_length; i++)
    {
        xprintf("otp_data_readTestColumn[%d] = 0x%08x\n", i, otp_data_readTestColumn[i]);
    }

    xprintf("\notp_data_readTestBit = 0x%08x\n", otp_data_readTestBit);
    #endif

    while (1)
    {    

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
    PeriphClkInit.PMClockAPB_M = PMCLOCKAPB_M_DEFAULT | PM_CLOCK_WU_M | PM_CLOCK_PAD_CONFIG_M | PM_CLOCK_OTP_CONTROLLER_M;     
    PeriphClkInit.PMClockAPB_P = PMCLOCKAPB_P_DEFAULT | PM_CLOCK_UART_0_M;     
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_NO_CLK;
    PeriphClkInit.RTCClockCPUSelection = RCC_RTCCLKCPUSOURCE_NO_CLK;
    HAL_RCC_ClockConfig(&PeriphClkInit);
}

static void OTP_Init(void)
{
    hotp.Instance = OTP;

    hotp.ReadMode = OPT_READ_3STAGES;

    HAL_OTP_Init(&hotp);
}
