#include "main.h"

I2C_HandleTypeDef hi2c0;
uint8_t flag = 0;

void SystemClock_Config(void);
static void MX_I2C0_Init(void);

int main()
{
   
    SystemClock_Config();

    MX_I2C0_Init();
    
    // Массив с байтами для отправки / приема
    uint8_t data[I2C_DATA_BYTES];

    for(int i = 0; i < sizeof(data); i++)
    {
        data[i] = (uint8_t)i; 
    }
    
    while (1)
    {
        /*Ведущий отправляет - ведомый принимает*/
        HAL_I2C_Slave_Read(&hi2c0, data, sizeof(data));

        /*Ведущий принимает - ведомый отправляет*/
        HAL_I2C_Slave_Write(&hi2c0, data, sizeof(data));

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
    PeriphClkInit.PMClockAPB_P = PMCLOCKAPB_P_DEFAULT | PM_CLOCK_UART_0_M | PM_CLOCK_I2C_0_M;     
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_NO_CLK;
    PeriphClkInit.RTCClockCPUSelection = RCC_RTCCLKCPUSOURCE_NO_CLK;
    HAL_RCC_ClockConfig(&PeriphClkInit);
}

static void MX_I2C0_Init(void)
{

    /*Общие настройки*/
    hi2c0.Instance = I2C_0;
    hi2c0.Mode = HAL_I2C_MODE_SLAVE;
    hi2c0.ShiftAddress = SHIFT_ADDRESS_DISABLE;
    hi2c0.Init.ClockSpeed = 165;
    hi2c0.Init.AddressingMode = I2C_ADDRESSINGMODE_10BIT;
    hi2c0.Init.DualAddressMode = I2C_DUALADDRESS_ENABLE; // При ENABLE в режиме ведущего значение AddressingMode не влияет

    /*Настройки ведомого*/
    hi2c0.Init.OwnAddress1 = 0x3FF; //0x36 0x3FF
    hi2c0.Init.OwnAddress2 = 0x36; //0x57
    hi2c0.Init.OwnAddress2Mask = I2C_OWNADDRESS2_MASK_DISABLE;
    hi2c0.Init.SBCMode = I2C_SBC_DISABLE;
    hi2c0.Init.NoStretchMode = I2C_NOSTRETCH_ENABLE; // Не совместим с режимом SBC

    /*Нстройки ведущего*/
    hi2c0.Init.AutoEnd = AUTOEND_ENABLE;

    HAL_I2C_Init(&hi2c0);

}

void HAL_I2C_Slave_SBC(I2C_HandleTypeDef *hi2c, uint32_t byte_count)
{

    if(byte_count == 2)
    {
        HAL_I2C_Slave_NACK(hi2c);
    }
    else
    {
        HAL_I2C_Slave_ACK(hi2c);
    }
    
}