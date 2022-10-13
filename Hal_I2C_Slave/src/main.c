#include "main.h"

I2C_HandleTypeDef hi2c0;

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
        #ifdef MIK32_I2C_DEBUG
        xprintf("\nОжидание\n");
        #endif
 
        int counter = 0;
        
        /*Ожидание запроса мастером адреса слейва*/ 
        while(!(hi2c0.Instance->ISR & I2C_ISR_ADDR_M))
        {
            counter++;
            if(counter==10000000)
            {
                #ifdef MIK32_I2C_DEBUG
                xprintf("\nОжидание\n");
                #endif

                counter = 0;
            }
            
        } 
        

        /*
        * Сброс флага ADDR для подверждения принятия адреса
        * ADDR - Флаг соответствия адреса в режиме ведомого
        */
        hi2c0.Instance->ICR |= I2C_ICR_ADDRCF_M; 
        
        /*
        * I2C_ISR - Регистр прерываний и статуса 
        * DIR - Тип передачи. Обновляется в режиме ведомого при получении адреса:
        *       0 – передача типа запись, ведомый переходит в режим приемника;
        *       1 – передача типа чтения, ведомый переходит в режим передатчика
        */
        if(hi2c0.Instance->ISR & I2C_ISR_DIR_M) // DIR = 1. Режим ведомого - передатчик
        {
            #ifdef MIK32_I2C_DEBUG
            xprintf("\nЗапрос на запись\n");
            #endif

            // Отправка
            HAL_I2C_Slave_Write(&hi2c0, data, sizeof(data));
            HAL_I2C_CheckError(&hi2c0);
        } 
        else // DIR = 0. Режим ведомого - приемник
        {
            #ifdef MIK32_I2C_DEBUG
            xprintf("\nЗапрос на чтение\n");
            #endif
            
            // Чтение двух байт от мастера и запись их в массив data 
            HAL_I2C_Slave_Read(&hi2c0, data, sizeof(data));
            HAL_I2C_CheckError(&hi2c0);
            
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
    hi2c0.Mode = HAL_I2C_MODE_SLAVE;
    hi2c0.ShiftAddress = SHIFT_ADDRESS_DISABLE;

    hi2c0.Init.ClockSpeed = 165;

    hi2c0.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c0.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c0.Init.OwnAddress1 = 0x36; //0x36 0x3FF
    hi2c0.Init.OwnAddress2 = 0b01010111; //0x57
    hi2c0.Init.OwnAddress2Mask = I2C_OWNADDRESS2_MASK_DISABLE;

    hi2c0.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    hi2c0.Init.SBCMode = I2C_SBC_DISABLE;
    hi2c0.Init.AutoEnd = AUTOEND_DISABLE;



    HAL_I2C_Init(&hi2c0);
    /* USER CODE BEGIN I2C1_Init 2 */

    /* USER CODE END I2C1_Init 2 */

}