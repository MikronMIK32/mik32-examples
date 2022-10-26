#include "mik32_hal_i2c.h"

void HAL_I2C_Disable(I2C_HandleTypeDef *hi2c)
{
    hi2c->Instance->CR1 &= ~I2C_CR1_PE_M;
}

void HAL_I2C_Enable(I2C_HandleTypeDef *hi2c)
{
    hi2c->Instance->CR1 |= I2C_CR1_PE_M;
}

void HAL_I2C_ReloadDisable(I2C_HandleTypeDef *hi2c)
{
    hi2c->Init.ReloadMode = I2C_RELOAD_DISABLE;
    hi2c->Instance->CR2 &= ~I2C_CR2_RELOAD_M;
}

void HAL_I2C_ReloadEnable(I2C_HandleTypeDef *hi2c)
{
    hi2c->Init.ReloadMode = I2C_RELOAD_ENABLE;
    hi2c->Instance->CR2 |= I2C_CR2_RELOAD_M;
}

void HAL_I2C_SetClockSpeed(I2C_HandleTypeDef *hi2c)
{
    /*
    * Инициализация i2c
    * TIMING - регистр таймингов
    * 
    * SCLDEL - Задержка между изменением SDA и фронтом SCL в режиме ведущего и ведомого при NOSTRETCH = 0
    * 
    * SDADEL - Задержка между спадом SCL и изменением SDA в режиме ведущего и ведомого при NOSTRETCH = 0
    * 
    * SCLL - Время удержания SCL в состоянии логического «0» в режиме веедущего
    * 
    * SCLH - Время удержания SCL в состоянии логической «1» в режиме веедущего
    * 
    * PRESC - Делитель частоты I2CCLK. Используется для вычесления периода сигнала TPRESC для счетчиков предустановки, 
    * удержания, уровня «0» и «1»
    * 
    */
    hi2c->Instance->TIMINGR = I2C_TIMINGR_SCLDEL(1) | I2C_TIMINGR_SDADEL(1) |
        I2C_TIMINGR_SCLL(20) | I2C_TIMINGR_SCLH(20) | I2C_TIMINGR_PRESC(3); //частота 164,7 кГц tsync1 + tsync2 = 10^(-6)
}

void HAL_I2C_MasterInit(I2C_HandleTypeDef *hi2c)
{
    /* Режим 10-битного адреса */
    #ifdef MIK32_I2C_DEBUG
    xprintf("\nМастер. Старт\n");
    #endif
}

void HAL_I2C_SlaveInit(I2C_HandleTypeDef *hi2c)
{
    /* Обнуление регистров OAR1 и OAR2 перед их настройкой*/
    hi2c->Instance->OAR1 = 0;
    hi2c->Instance->OAR2 = 0;
    
    uint32_t slave_address1 = hi2c->Init.OwnAddress1;
    
    // true когда адрес вводится без сдвига
    if(hi2c->ShiftAddress == SHIFT_ADDRESS_DISABLE)
    {
        slave_address1 = slave_address1 << 1;
    }

    /* Режим удержания SCL ведомого */
    if(hi2c->Init.NoStretchMode == I2C_NOSTRETCH_ENABLE)
    {
        hi2c->Instance->CR1 |= I2C_CR1_NOSTRETCH_M; // Без растягивания
    }
    else
    {
        hi2c->Instance->CR1 &= ~I2C_CR1_NOSTRETCH_M; // Растягивание активно
    }

    /* Режим аппаратного контроля байта ведомым - SBC */
    if(hi2c->Init.SBCMode == I2C_SBC_ENABLE)
    {
        hi2c->Instance->CR1 |= I2C_CR1_SBC_M; // Контроль включен
        hi2c->Instance->CR2 |= I2C_CR2_RELOAD_M; 
    }
    else
    {
        hi2c->Instance->CR1 &= ~I2C_CR1_SBC_M; // Контроль выключен
        hi2c->Instance->CR2 &= ~I2C_CR2_RELOAD_M;
    }

    /*Режим адресации 7/10 бит*/
    switch (hi2c->Init.AddressingMode)
    {
    case I2C_ADDRESSINGMODE_7BIT:
        hi2c->Instance->OAR1 &= ~I2C_OAR1_OA1MODE_M;
        break;
    
    case I2C_ADDRESSINGMODE_10BIT:
        hi2c->Instance->OAR1 |= I2C_OAR1_OA1MODE_M;
        break;
    }

    hi2c->Instance->OAR1 |= (slave_address1 << I2C_OAR1_OA1_S) | I2C_OAR1_OA1EN_M; // собственный адрес 1

    /*Режим дополнительного 7 битного адреса. Установка маски для дополнительного адреса*/
    if(hi2c->Init.DualAddressMode == I2C_DUALADDRESS_ENABLE)
    {
        hi2c->Instance->OAR2 |= (hi2c->Init.OwnAddress2 << I2C_OAR2_OA2_S) | 
            (hi2c->Init.OwnAddress2Mask << I2C_OAR2_OA2MSK_S) | I2C_OAR2_OA2EN_M; // собственный адрес 2
    }
    
}

void HAL_I2C_Init(I2C_HandleTypeDef *hi2c)
{
    HAL_I2C_Disable(hi2c);

    /* Обнуление регистра управления CR1 перед его настройкой */
    hi2c->Instance->CR1 = 0;

    /* Настройка требуемой частоты SCL */
    HAL_I2C_SetClockSpeed(hi2c);

    switch (hi2c->Mode)
    {
    case HAL_I2C_MODE_MASTER:
        HAL_I2C_MasterInit(hi2c);
        break;
    case HAL_I2C_MODE_SLAVE:
        HAL_I2C_SlaveInit(hi2c);
        break;
    }

    hi2c->ErrorCode = I2C_ERROR_NONE;

    HAL_I2C_Enable(hi2c);
}

void HAL_I2C_CheckError(I2C_HandleTypeDef *hi2c)
{
    #ifdef MIK32_I2C_DEBUG
        if(hi2c->ErrorCode != 0)
        {
            xprintf("Ошибка №%d\n", hi2c->ErrorCode);
        }
    #endif

    if(hi2c->ErrorCode == I2C_ERROR_NONE)
    {
        return;
    }

    switch (hi2c->Mode)
    {
    case HAL_I2C_MODE_SLAVE:
        HAL_I2C_Slave_CheckError(hi2c);
        break;
    
    case HAL_I2C_MODE_MASTER:
        HAL_I2C_Master_CheckError(hi2c);
        break;
    }

    hi2c->ErrorCode = I2C_ERROR_NONE;
}

/* Ведущий */
void HAL_I2C_Master_Stop(I2C_HandleTypeDef *hi2c)
{
    hi2c->Instance->CR2 |= I2C_CR2_STOP_M;
    while(hi2c->Instance->ISR & I2C_ISR_BUSY_M);
    HAL_I2C_Slave_CleanFlag(hi2c);
}

void HAL_I2C_Master_Restart(I2C_HandleTypeDef *hi2c)
{
    HAL_I2C_Init(hi2c);
}

void HAL_I2C_Master_CheckError(I2C_HandleTypeDef *hi2c)
{
    // HAL_I2C_ReloadDisable(hi2c);
    // hi2c->Instance->CR2 |= I2C_ICR_STOPCF_M | I2C_ICR_NACKCF_M; // Отправка NACK и STOP

    switch (hi2c->ErrorCode)
    {
    case I2C_ERROR_TIMEOUT:
        #ifdef MIK32_I2C_DEBUG
        xprintf("Ожидание превышено\n");
        #endif

        HAL_I2C_Slave_Restart(hi2c);// Программный сброс модуля i2c и его повторная инициалиизация 
        break;
    case I2C_ERROR_NACK:
        #ifdef MIK32_I2C_DEBUG
        xprintf("Ошибка при передаче\n");
        xprintf("NACKF = %d\n", (hi2c->Instance->ISR & I2C_ISR_NACKF_M) >> I2C_ISR_NACKF_S);
        #endif

        hi2c->Instance->ICR |= I2C_ICR_STOPCF_M | I2C_ICR_NACKCF_M; // сброс флага STOPF и NACKF
        break;
    }
}

void HAL_I2C_Master_Transfer_Init(I2C_HandleTypeDef *hi2c)
{
    /* Обнуление регистра CR2 перед его настройкой*/
    hi2c->Instance->CR2 = 0;

    /* Изменение режима адресация в зависимости от типа адреса, 
                        при активном режиме двойной адресации */
    if(hi2c->Init.DualAddressMode == I2C_DUALADDRESS_ENABLE)
    {
        if(hi2c->SlaveAddress > (0x7f << 1))
        {
            hi2c->Init.AddressingMode = I2C_ADDRESSINGMODE_10BIT;
        }
        else
        {
            hi2c->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
        }
    }

    /* Выбор режима адресации */
    switch (hi2c->Init.AddressingMode)
    {
    case I2C_ADDRESSINGMODE_7BIT:
        hi2c->Instance->CR2 &= ~I2C_CR2_ADD10_M;
        break;
    
    case I2C_ADDRESSINGMODE_10BIT:
        hi2c->Instance->CR2 |= I2C_CR2_ADD10_M;
        hi2c->Instance->CR2 &= ~I2C_CR2_HEAD10R_M;
        break;
    }
    
    /*
    *
    * CR2 - регистр управления 2
    * 
    * SADD - адрес ведомого
    * 
    * RD_WRN - Направление передачи: 0 – ведущий в режиме записи; 1 – ведущий в режиме чтения
    * 
    * NBYTES - Количество байт для приема / передачи
    * 
    * AUTOEND - Управление режимом автоматического окончания: 0 – автоматическое окончание выкл; 1 – автоматическе окончание вкл
    * 
    */
    hi2c->Instance->CR2 |= I2C_CR2_SADD(hi2c->SlaveAddress) | I2C_CR2_WR_M;

    switch (hi2c->TransferDirection)
    {
    case I2C_TRANSFER_WRITE:
        hi2c->Instance->CR2 |= I2C_CR2_WR_M;
        break;

    case I2C_TRANSFER_READ:
        hi2c->Instance->CR2 |= I2C_CR2_RD_M;
        break;
    }

    /* Если в передаче больше 255 байт, включается режим перезагрузки */
    if(hi2c->TransferSize <= I2C_NBYTE_MAX )
    {
        HAL_I2C_ReloadDisable(hi2c);
        hi2c->Instance->CR2 |= I2C_CR2_NBYTES(hi2c->TransferSize);
    }
    else
    {
        HAL_I2C_ReloadEnable(hi2c);
        hi2c->Instance->CR2 |= I2C_CR2_NBYTES(I2C_NBYTE_MAX);

    }

    

    /* Бит AutoEnd не действует при режиме Reload */
    if(hi2c->Init.AutoEnd == AUTOEND_ENABLE)
    {
        hi2c->Instance->CR2 |= I2C_CR2_AUTOEND_M;
    }
    else
    {
        hi2c->Instance->CR2 &= ~I2C_CR2_AUTOEND_M;
    }

    hi2c->Instance->CR2 |= I2C_CR2_START_M; // старт отправки адреса, а затем данных 
}

void HAL_I2C_Master_WriteNBYTE(I2C_HandleTypeDef *hi2c, uint16_t slave_adr, uint8_t data[], uint32_t byte_count)
{
    #ifdef MIK32_I2C_DEBUG
    xprintf("\nОтправка\n");
    uint16_t slave_adr_print = slave_adr; // переменная используется для вывода адреса
    #endif

    // true когда адрес вводится без сдвига
    if(hi2c->ShiftAddress == SHIFT_ADDRESS_DISABLE)
    {
        slave_adr = slave_adr << 1;
    } 

    hi2c->SlaveAddress = slave_adr;
    hi2c->pBuff = data;
    hi2c->TransferSize = byte_count;
    hi2c->TransferDirection = I2C_TRANSFER_WRITE;

    /* Настройка CR2 */
    HAL_I2C_Master_Transfer_Init(hi2c);
    uint8_t byte_limit = I2C_NBYTE_MAX;

    for (uint32_t i = 0; i < byte_count; i++)
    {
        int timeout_counter = 0; // Счетчик для ожидания
        while(!(hi2c->Instance->ISR & I2C_ISR_TXIS_M))// TXIS = 1 - предыдущий байт доставлен
        {
            timeout_counter++;
            if(timeout_counter == I2C_TIMEOUT)
            {
                // Ожидание превышено. Возможно механическое повреждение линии связи
                hi2c->ErrorCode = I2C_ERROR_TIMEOUT;
                return;
            }

            // Ошибка. При записи байта ведомый прислал NACK
            if(hi2c->Instance->ISR & I2C_ISR_NACKF_M) 
            {
                hi2c->ErrorCode = I2C_ERROR_NACK;
                return;
            }
        }

        #ifdef MIK32_I2C_DEBUG 
        xprintf("Отправка по адресу 0x%03x байта №%d  0x%02x\n", slave_adr_print, i+1, data[i]);
        #endif
        //for (volatile int i = 0; i < 100000; i++);
        
        hi2c->Instance->TXDR = data[i];

        byte_limit--;

        if(byte_limit == 0)
        {
            if(byte_count - (i + 1) == 0)
            {
                break;
            }

            while(!(hi2c->Instance->ISR & I2C_ISR_TCR_M)); // TCR = 1 - конец передачи 255 байт
            byte_limit = I2C_NBYTE_MAX;

            if(byte_count - (i + 1) > 255)
            {
                hi2c->Instance->CR2 |= I2C_CR2_NBYTES(I2C_NBYTE_MAX);
            }
            else
            {
                HAL_I2C_ReloadDisable(hi2c);
                uint32_t CR2_NBYTES = hi2c->Instance->CR2;
                CR2_NBYTES &= ~I2C_CR2_NBYTES_M; // обнулить NBYTES
                CR2_NBYTES |= I2C_CR2_NBYTES(byte_count - (i + 1));
                hi2c->Instance->CR2 = CR2_NBYTES;
            }
               
        }

    }

    hi2c->ErrorCode = I2C_ERROR_NONE;

    #ifdef MIK32_I2C_DEBUG
    xprintf("Конец передачи\n");
    #endif

    if(hi2c->Init.AutoEnd == AUTOEND_DISABLE)
    {
        /*
        * Флаг окончания передачи в режиме ведущего. 
        * Устанавливается при RELOAD=0, AUTOEND=0 и передачи NBYTES байт
        */
        while(!(hi2c->Instance->ISR & I2C_ISR_TC_M));
        return; 
    }

    while(hi2c->Instance->ISR & I2C_ISR_BUSY_M);

}

void HAL_I2C_Master_ReadNBYTE(I2C_HandleTypeDef *hi2c, uint16_t slave_adr, uint8_t data[], uint32_t byte_count)
{
    
    #ifdef MIK32_I2C_DEBUG
    xprintf("\nЧтение\n");
    uint16_t slave_adr_print = slave_adr; // переменная используется для вывода адреса
    #endif

    // true когда адрес вводится без сдвига
    if(hi2c->ShiftAddress == SHIFT_ADDRESS_DISABLE)
    {
        slave_adr = slave_adr << 1;
    }

    hi2c->SlaveAddress = slave_adr;
    hi2c->pBuff = data;
    hi2c->TransferSize = byte_count;
    hi2c->TransferDirection = I2C_TRANSFER_READ;

    /* Настройка CR2 */
    HAL_I2C_Master_Transfer_Init(hi2c);
    uint8_t byte_limit = I2C_NBYTE_MAX;

    for(uint32_t i = 0; i < byte_count; i++)
    {
        // Счетчик для ожидания
        int timeout_counter = 0;

        while(!(hi2c->Instance->ISR & I2C_ISR_RXNE_M)) // байт принят когда RXNE = 1
        {
            timeout_counter++;
            if(timeout_counter == I2C_TIMEOUT)
            {
                // Ожидание превышено. Возможно механическое повреждение линии связи
                hi2c->ErrorCode = I2C_ERROR_TIMEOUT;
                return;
            }

            // Ошибка. Во время чтения мастер прислал NACK или STOP
            if((hi2c->Instance->ISR & I2C_ISR_STOPF_M) && (hi2c->Instance->ISR & I2C_ISR_NACKF_M))
            {
                #ifdef MIK32_I2C_DEBUG
                xprintf("Чтение. STOPF = %d, NACKF = %d\n", (hi2c->Instance->ISR & I2C_ISR_STOPF_M) >> I2C_ISR_STOPF_S, 
                                                            (hi2c->Instance->ISR & I2C_ISR_NACKF_M) >> I2C_ISR_NACKF_S);
                #endif
                hi2c->ErrorCode = I2C_ERROR_NACK;
                return;
            }
            
        }

        data[i] = hi2c->Instance->RXDR; // чтение байта и сброс RXNE
        #ifdef MIK32_I2C_DEBUG
        xprintf("Чтение по адресу 0x%03x байта №%d 0x%02x\n", slave_adr_print, i+1, data[i]);
        #endif

        byte_limit--;

        if(byte_limit == 0)
        {
            if(byte_count - (i + 1) == 0)
            {
                break;
            }

            while(!(hi2c->Instance->ISR & I2C_ISR_TCR_M)); // TCR = 1 - конец передачи 255 байт
            byte_limit = I2C_NBYTE_MAX;

            if(byte_count - (i + 1) > 255)
            {
                hi2c->Instance->CR2 |= I2C_CR2_NBYTES(I2C_NBYTE_MAX);
            }
            else // Выключение режима Reload и отправка отсавшихся байтов
            {
                HAL_I2C_ReloadDisable(hi2c);
                uint32_t CR2_NBYTES = hi2c->Instance->CR2;
                CR2_NBYTES &= ~I2C_CR2_NBYTES_M; // обнулить NBYTES
                CR2_NBYTES |= I2C_CR2_NBYTES(byte_count - (i + 1));
                hi2c->Instance->CR2 = CR2_NBYTES;
            }
            
        }

    }

    hi2c->ErrorCode = I2C_ERROR_NONE;

    #ifdef MIK32_I2C_DEBUG
    xprintf("Конец передачи\n");
    #endif

    if(hi2c->Init.AutoEnd == AUTOEND_DISABLE)
    {
        /*
        * Флаг окончания передачи в режиме ведущего. 
        * Устанавливается при RELOAD=0, AUTOEND=0 и передачи NBYTES байт
        */
        while(!(hi2c->Instance->ISR & I2C_ISR_TC_M));
        return; 
    }

    while(hi2c->Instance->ISR & I2C_ISR_BUSY_M);

    //hi2c->Instance->ICR |= I2C_ICR_STOPCF_M | I2C_ICR_NACKCF_M; // сброс флага STOPF и NACKF
    

}

void HAL_I2C_Master_Write(I2C_HandleTypeDef *hi2c, uint16_t slave_adr, uint8_t data[], uint32_t byte_count)
{
    /*Запись данных по адресу slave_adr без сдвига адреса*/
    HAL_I2C_Master_WriteNBYTE(hi2c, slave_adr, data, byte_count); 
    HAL_I2C_CheckError(hi2c);
}

void HAL_I2C_Master_Read(I2C_HandleTypeDef *hi2c, uint16_t slave_adr, uint8_t data[], uint32_t byte_count)
{
    /*Чтение данных по адресу slave_adr без сдвига адреса*/
    HAL_I2C_Master_ReadNBYTE(hi2c, slave_adr, data, byte_count);  
    HAL_I2C_CheckError(hi2c);
}

/* Ведомый */
void __attribute__((weak)) HAL_I2C_Slave_SBC(I2C_HandleTypeDef *hi2c, uint32_t byte_count)
{
    /*Формирование ACK*/
    HAL_I2C_Slave_ACK(hi2c);
}

void HAL_I2C_Slave_WaitADDR(I2C_HandleTypeDef *hi2c)
{
    #ifdef MIK32_I2C_DEBUG
    xprintf("\nОжидание\n");
    #endif

    int counter = 0;
    
    /*Ожидание запроса мастером адреса слейва*/ 
    while(!(hi2c->Instance->ISR & I2C_ISR_ADDR_M))
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
    hi2c->Instance->ICR |= I2C_ICR_ADDRCF_M;
    while(hi2c->Instance->ISR & I2C_ISR_ADDR_M);
}

void HAL_I2C_Slave_ACK(I2C_HandleTypeDef *hi2c)
{
    /*Формирование сигнала ACK*/
    hi2c->Instance->CR2 &= ~I2C_CR2_NACK_M;
}

void HAL_I2C_Slave_NACK(I2C_HandleTypeDef *hi2c)
{
    /*Формирование сигнала NACK*/
    hi2c->Instance->CR2 |= I2C_CR2_NACK_M;
    hi2c->ErrorCode = I2C_ERROR_NACK;
}

void HAL_I2C_Slave_CleanFlag(I2C_HandleTypeDef *hi2c)
{
    hi2c->Instance->ICR |= I2C_ICR_STOPCF_M | I2C_ICR_NACKCF_M | I2C_ICR_OVRCF_M; // сброс флагов
    while(hi2c->Instance->ISR & I2C_ISR_OVR_M); // ожидание сброса флага OVR
    while(hi2c->Instance->ISR & I2C_ISR_NACKF_M); // ожидание сброса флага NACKF
    while(hi2c->Instance->ISR & I2C_ISR_STOPF_M); // ожидание сброса флага STOPF

    #ifdef MIK32_I2C_DEBUG
    xprintf("сброшено. NACKF = %d\n", (hi2c->Instance->ISR & I2C_ISR_NACKF_M) >> I2C_ISR_NACKF_S);
    #endif
}

void HAL_I2C_Slave_Restart(I2C_HandleTypeDef *hi2c)
{
    HAL_I2C_Init(hi2c);
}

void HAL_I2C_Slave_CheckError(I2C_HandleTypeDef *hi2c)
{ 

    switch (hi2c->ErrorCode)
    {
    case I2C_ERROR_TIMEOUT:
        #ifdef MIK32_I2C_DEBUG
        xprintf("Ожидание превышено\n");
        #endif

        HAL_I2C_Slave_Restart(hi2c);// Программный сброс модуля i2c и его повторная инициалиизация 
        break;
    case I2C_ERROR_NACK:
        #ifdef MIK32_I2C_DEBUG
        xprintf("Ошибка. NACKF = %d\n", (hi2c->Instance->ISR & I2C_ISR_NACKF_M) >> I2C_ISR_NACKF_S);
        #endif

        HAL_I2C_Slave_CleanFlag(hi2c);
        break;
    case I2C_ERROR_OVR:
        #ifdef MIK32_I2C_DEBUG
        xprintf("Ошибка перебор/недобор. OVR = %d\n", (hi2c->Instance->ISR & I2C_ISR_OVR_M) >> I2C_ISR_OVR_S);
        #endif

        HAL_I2C_Slave_CleanFlag(hi2c);
        break;
    }
}

void HAL_I2C_Slave_WriteNBYTE(I2C_HandleTypeDef *hi2c, uint8_t data[], uint32_t byte_count)
{
    #ifdef MIK32_I2C_DEBUG
    xprintf("\nОтправка\n");
    #endif
       
    HAL_I2C_Slave_CleanFlag(hi2c);
    
    hi2c->pBuff = data;
    hi2c->TransferSize = byte_count;
    hi2c->TransferDirection = I2C_TRANSFER_WRITE;
    for (uint32_t i = 0; i < byte_count; i++)
    {
        
        int timeout_counter = 0; // Счетчик для ожидания
        while(!(hi2c->Instance->ISR & I2C_ISR_TXE_M)) // TXE = 1 - Данные заиисываются в сдвиговый регистр
        {
            timeout_counter++;
            if(timeout_counter == I2C_TIMEOUT)
            {
                /*Ожидание превышено. Возможно механическое повреждение линии связи*/
                hi2c->ErrorCode = I2C_ERROR_TIMEOUT;
                return;
            }

            /*При записи байта мастер прислал NACK. Возникла ошибка при передаче*/
            if(hi2c->Instance->ISR & I2C_ISR_NACKF_M)
            {
                hi2c->ErrorCode = I2C_ERROR_NACK;
                return;
            }

        }

        if(hi2c->Init.NoStretchMode == I2C_NOSTRETCH_ENABLE)
        {
            /*В TXDR долго не записываются данные. Перед передачей не был сброшен STOPF*/
            if(hi2c->Instance->ISR & I2C_ISR_OVR_M)
            {
                hi2c->ErrorCode = I2C_ERROR_OVR;
                return;
            }
        }

        hi2c->Instance->TXDR = data[i]; // Загрузка передаваемого байта в регистр TXDR
        
        #ifdef MIK32_I2C_DEBUG
        xprintf("Отправлен байт №%d 0x%02x [%d]\n", i+1, data[i], data[i]);
        #endif

        
    }
    //HAL_I2C_Slave_CleanFlag(hi2c);
    #ifdef MIK32_I2C_DEBUG
    xprintf("Конец передачи\n");
    #endif 
}

void HAL_I2C_Slave_ReadNBYTE(I2C_HandleTypeDef *hi2c, uint8_t data[], uint32_t byte_count)
{
    #ifdef MIK32_I2C_DEBUG
    xprintf("\nЧтение\n");
    #endif

    HAL_I2C_Slave_CleanFlag(hi2c);

    hi2c->pBuff = data;
    hi2c->TransferSize = byte_count;
    hi2c->TransferDirection = I2C_TRANSFER_READ;

    for (uint32_t i = 0; i < byte_count; i++)
    {
       
        /*Счетчик для ожидания*/
        int timeout_counter = 0;

        /*Байт нужно прочитать когда RXNE = 1*/
        while(!(hi2c->Instance->ISR & I2C_ISR_RXNE_M)) 
        {
            
            timeout_counter++;

            if(timeout_counter == I2C_TIMEOUT)
            {
                /*Ожидание превышено. Возможно механическое повреждение линии связи*/
                hi2c->ErrorCode = I2C_ERROR_TIMEOUT;
                return;
            }

            /*Данные не вычитаны из RXDR до ACK следующего байта*/
            if(hi2c->Instance->ISR & I2C_ISR_OVR_M)
            {
                hi2c->ErrorCode = I2C_ERROR_OVR;
                return;
            }

            if((hi2c->Instance->ISR & I2C_ISR_NACKF_M))
            {
                /*Ведомый отправил NACK. Ошибка чтения*/
                hi2c->ErrorCode = I2C_ERROR_NACK;
                return;
            }

        } 

        if(!(hi2c->Instance->ISR & I2C_ISR_DIR_M))
        {
            data[i] = hi2c->Instance->RXDR; // Чтение байта и сброс флага RXNE

            #ifdef MIK32_I2C_DEBUG
            xprintf("Чтение байта №%d  0x%02x [%d]\n", i+1, data[i], data[i]);
            #endif

            if(hi2c->Init.SBCMode == I2C_SBC_ENABLE)
            {
                HAL_I2C_Slave_SBC(hi2c, i);
                hi2c->Instance->CR2 |= I2C_CR2_NBYTES(0x1);
                
                if(hi2c->ErrorCode == I2C_ERROR_NACK)
                {
                    /*Ведомый отправил NACK. Ошибка чтения*/
                    hi2c->ErrorCode = I2C_ERROR_NACK;
                    return;
                }
            }
        }
        
    }

    if(!(hi2c->Instance->ISR & I2C_ISR_DIR_M))
    {
        #ifdef MIK32_I2C_DEBUG
        xprintf("Прочитано\n");
        #endif
    }
    
}

void HAL_I2C_Slave_Write(I2C_HandleTypeDef *hi2c, uint8_t data[], uint32_t byte_count)
{
    HAL_I2C_Slave_WaitADDR(hi2c);
    if((hi2c->Init.AddressingMode == I2C_ADDRESSINGMODE_10BIT) && (hi2c->Init.NoStretchMode == I2C_NOSTRETCH_DISABLE))
    {
        HAL_I2C_Slave_WaitADDR(hi2c); 
    }
    
    /*Ожидание DIR = 1*/
    while(!(hi2c->Instance->ISR & I2C_ISR_DIR_M));
    /*
    * I2C_ISR - Регистр прерываний и статуса 
    * DIR = 1 – передача типа чтения, ведомый переходит в режим передатчика
    */
    if(hi2c->Instance->ISR & I2C_ISR_DIR_M) 
    {
        #ifdef MIK32_I2C_DEBUG
        xprintf("\nЗапрос на запись\n");
        #endif
        // Отправка
        HAL_I2C_Slave_WriteNBYTE(hi2c, data, byte_count);
        HAL_I2C_CheckError(hi2c);
    } 
}

void HAL_I2C_Slave_Read(I2C_HandleTypeDef *hi2c, uint8_t data[], uint32_t byte_count)
{
    HAL_I2C_Slave_WaitADDR(hi2c);
    
    /*Ожидание DIR = 0*/
    while(hi2c->Instance->ISR & I2C_ISR_DIR_M);
    /*
    * I2C_ISR - Регистр прерываний и статуса 
    * DIR = 0 – передача типа запись, ведомый переходит в режим приемника;
    */
    if(!(hi2c->Instance->ISR & I2C_ISR_DIR_M))
    {
        #ifdef MIK32_I2C_DEBUG
        xprintf("\nЗапрос на чтение\n");
        #endif

        // Чтение двух байт от мастера и запись их в массив data 
        HAL_I2C_Slave_ReadNBYTE(hi2c, data, byte_count);
        HAL_I2C_CheckError(hi2c);
        
    } 
}