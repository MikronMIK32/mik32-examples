#include "main.h"

I2C_HandleTypeDef hi2c0;

void SystemClock_Config(void);
static void MX_I2C0_Init(void);

int main()
{    

    SystemClock_Config();
    
    MX_I2C0_Init();

    // Адрес ведомого
    uint16_t slave_address = 0x36; //0x36 0x3FF 0x7F

    uint8_t data[I2C_DATA_BYTES];

    for(int i = 0; i < sizeof(data); i++)
    {
        data[i] = (uint8_t)i;

        #ifdef MIK32_I2C_DEBUG
        xprintf("data[%d] = %d\n", i, data[i]);
        #endif
    }
     
    while (1)
    {    

        // // Запись данных по адресу slave_adr = 0x36 без сдвига адреса
        HAL_I2C_Master_Write(&hi2c0, slave_address, data, sizeof(data)); 
        HAL_I2C_CheckError(&hi2c0);
        for (volatile int i = 0; i < 1000000; i++); 

        // Чтение данных по адресу slave_adr = 0x36 без сдвига адреса
        HAL_I2C_Master_Read(&hi2c0, slave_address, data, sizeof(data)); 
        HAL_I2C_CheckError(&hi2c0);
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
    PeriphClkInit.PMClockAPB_P = PMCLOCKAPB_P_DEFAULT | PM_CLOCK_UART_0_M | PM_CLOCK_I2C_0_M;     
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_NO_CLK;
    PeriphClkInit.RTCClockCPUSelection = RCC_RTCCLKCPUSOURCE_NO_CLK;
    HAL_RCC_ClockConfig(&PeriphClkInit);
}

static void MX_I2C0_Init(void)
{

    /* USER CODE BEGIN I2C1_Init 0 */

    /* USER CODE END I2C1_Init 0 */

    /* USER CODE BEGIN I2C1_Init 1 */

    /* USER CODE END I2C1_Init 1 */
    hi2c0.Instance = I2C_0;
    hi2c0.Mode = HAL_I2C_MODE_MASTER;
    hi2c0.ShiftAddress = SHIFT_ADDRESS_DISABLE;

    hi2c0.Init.ClockSpeed = 165;

    hi2c0.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c0.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE; // При ENABLE значение AddressingMode не влияет на тип адресации (Только в режиме мастера)
    hi2c0.Init.OwnAddress1 = 0;
    hi2c0.Init.OwnAddress2 = 0;
    hi2c0.Init.OwnAddress2Mask = I2C_OWNADDRESS2_MASK_DISABLE;

    hi2c0.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    hi2c0.Init.SBCMode = I2C_SBC_DISABLE;
    hi2c0.Init.AutoEnd = SHIFT_AUTOEND_ENABLE;



    HAL_I2C_Init(&hi2c0);
    /* USER CODE BEGIN I2C1_Init 2 */

    /* USER CODE END I2C1_Init 2 */

}