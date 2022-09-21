#include "main.h"

I2C_HandleTypeDef hi2c0;

void SystemClock_Config(void);
static void MX_I2C0_Init(void);

int main()
{

    SystemClock_Config();

    MX_I2C0_Init();
    
    // Число для оптавки
    uint16_t to_send = 1; 
    // Массив с байтами для отправки / приема
    uint8_t data[2] = {0,0};
    
    while (1)
    {
        xprintf("\nОжидание\n");
        int counter = 0;
        
        // Ожидание запроса мастером адреса слейва
        while(!(hi2c0.Instance->ISR & I2C_ISR_ADDR_M))
        {
            counter++;
            if(counter==10000000)
            {
                xprintf("\nОжидание\n");
                counter = 0;
            }
            
        } 
        
        //I2C_0 -> ISR |=  I2C_ISR_TXE_M;

        // срос флага ADDR для подверждения принятия адреса
        hi2c0.Instance->ICR |= I2C_ICR_ADDRCF_M; 

        /*
        * I2C_ISR - Регистр прерываний и статуса
        *   
        * ADDR - Флаг соответствия адреса в ре-жиме ведомого
        *   
        * DIR - Тип передачи. Обновляется в режиме ведомого при получении адреса:
        *       0 – передача типа запись, ведомый переходит в режим приемника;
        *       1 – передача типа чтения, ведомый переходит в режим передатчика
        * 
        */

        if(hi2c0.Instance->ISR & I2C_ISR_DIR_M) // DIR = 1. Режим ведомого - передатчик
        {
            xprintf("\nЗапрос на запись\n");

            // Отправляется число to_send, состоящее из двух байт
            // Заполняется массив data байтами, которые нужно отправить
            data[0] = to_send >> 8;
            data[1] = to_send & 0b0000000011111111;

            // Отправка
            HAL_I2C_Slave_Write(&hi2c0, data, sizeof(data));
        } 
        else // DIR = 0. Режим ведомого - приемник
        {
            xprintf("\nЗапрос на чтение\n");
            
            // Чтение двух байт от мастера и запись их в массив data 
            HAL_I2C_Slave_Read(&hi2c0, data, sizeof(data));

            // Формирование принятого числа
            to_send = data[0] << 8 | data[1];
            to_send++; 
            if(to_send > 65530)
            {
                to_send = 0;
            }
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
    hi2c0.Init.DualAddressMode = I2C_DUALADDRESS_ENABLE;
    hi2c0.Init.OwnAddress1 = 0x36; //0x36 0x3FF
    hi2c0.Init.OwnAddress2 = 0b01010111; //0x57
    hi2c0.Init.OwnAddress2Mask = I2C_OWNADDRESS2_MASK_1111xxx;

    hi2c0.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    hi2c0.Init.SBCMode = I2C_SBC_DISABLE;
    hi2c0.Init.AutoEnd = SHIFT_AUTOEND_ENABLE;



    HAL_I2C_Init(&hi2c0);
    /* USER CODE BEGIN I2C1_Init 2 */

    /* USER CODE END I2C1_Init 2 */

}