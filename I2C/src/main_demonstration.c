// #include "common.h"
// #include "i2c_loop_common.h"
// #include "stdbool.h"
// #include "wakeup.h"

// void i2c_init(I2C_TypeDef* i2c)
// {
//     //Включаем тактирование необходимых блоков - GPIO_0, GPIO_1, GPIO_2 и модуля выбора режима GPIO 
//     PM->CLK_APB_P_SET |= PM_CLOCK_GPIO_0_M | PM_CLOCK_I2C_0_M;
//     PM->CLK_APB_M_SET |= PM_CLOCK_PAD_CONFIG_M | PM_CLOCK_WU_M | PM_CLOCK_PM_M ;
//     for (volatile int i = 0; i < 10; i++);


//     // обнуление регистра управления
//     i2c->CR1 = 0;

//     /*
//     * Инициализация i2c
//     * TIMING - регистр таймингов
//     * 
//     * SCLDEL - Задержка между изменением SDA и фронтом SCL в режиме ведущего и ведомого при NOSTRETCH = 0
//     * 
//     * SDADEL - Задержка между спадом SCL и изменением SDA в режиме ведущего и ведомого при NOSTRETCH = 0
//     * 
//     * SCLL - Время удержания SCL в состоянии логического «0» в режиме веедущего
//     * 
//     * SCLH - Время удержания SCL в состоянии логической «1» в режиме веедущего
//     * 
//     * PRESC - Делитель частоты I2CCLK. Используется для вычесления периода сигнала TPRESC для счетчиков предустановки, 
//     * удержания, уровня «0»и «1»
//     * 
//     */
//     i2c->TIMINGR = I2C_TIMINGR_SCLDEL(1) | I2C_TIMINGR_SDADEL(1) |
//         I2C_TIMINGR_SCLL(20) | I2C_TIMINGR_SCLH(20) | I2C_TIMINGR_PRESC(3); //частота 164,7 кГц tsync1 + tsync2 = 10^(-6)

//     /*
//     *
//     * CR1 - Регистр управления
//     * 
//     * PE - Управление интерфейсом: 0 – интерфейс выключен; 1 – интерфейс включен
//     * 
//     * STOPIE - Разрешение прерывания детектировании STOP: 0 – прерывание запрещено; 1 – прерывание разрешено
//     *
//     */
//     i2c->CR1 = I2C_CR1_PE_M;
//     xprintf("\nМастер. Старт\n");
// }

// void i2c_master_write(I2C_TypeDef* i2c, uint8_t slave_adr, uint8_t data[], uint8_t byte_count, bool shift)
// {
//     xprintf("\nОтправка\n");
//     uint8_t slave_adr_print = slave_adr;
//     // shift - true когда адрес ведомого должен быть сдвинут на 1 бит
//     if(!shift)
//     {
//         slave_adr = slave_adr << 1;
//     }

//     /*
//     *
//     * CR2 - регистр управления 2
//     * 
//     * SADD - адрес ведомого
//     * 
//     * RD_WRN - Направление передачи: 0 – ведущий в режиме записи; 1 – ведущий в режиме чтения
//     * 
//     * AUTOEND - Управление режимом автоматического окончания: 0 – автоматическое окончание выкл; 1 – автоматическе окончание вкл
//     * 
//     */
//     i2c->CR2 = I2C_CR2_SADD(slave_adr) | I2C_CR2_WR_M | I2C_CR2_NBYTES(byte_count) | I2C_CR2_AUTOEND_M; // sizeof(data)

//     i2c->CR2 |= I2C_CR2_START_M; // старт отправки адреса, а затем данных 

//     for (uint8_t i = 0; i < byte_count; i++)
//     {
//         //xprintf("Отправка\n");
//         while(!(i2c->ISR & I2C_ISR_TXIS_M))// TXIS = 1 - предыдущий байт доставлен
//         {
//             if(i2c->ISR & I2C_ISR_NACKF_M)//при записи байта слейв прислал NACK
//             {
//                 xprintf("Запись. NACKF = %d\n", (i2c->ISR & I2C_ISR_NACKF_M) >> I2C_ISR_NACKF_S);
//                 break;
//             }
//         }
        
//         if(i2c->ISR & I2C_ISR_NACKF_M)//при записи байта слейв прислал NACK
//         {
//             i2c->ICR |=  I2C_ICR_NACKCF_M; // сброс флага STOPF и сброс флага NACKF 
//             break;
//         }
//         xprintf("Отправка по адресу 0x%02x байта  0x%02x\n", slave_adr_print, data[i]);
//         i2c->TXDR = data[i];
//     }

// }



// void i2c_master_read(I2C_TypeDef* i2c, uint8_t slave_adr, uint8_t data[], uint8_t byte_count, bool shift)
// {
//     uint8_t slave_adr_print = slave_adr;
//     // shift - true когда адрес ведомого должен быть сдвинут на 1 бит
//     if(!shift)
//     {
//         slave_adr = slave_adr << 1;
//     }
//     xprintf("\nЧтение\n");
//     i2c->CR2 = I2C_CR2_SADD(slave_adr) | I2C_CR2_RD_M | I2C_CR2_NBYTES(byte_count) | I2C_CR2_AUTOEND_M; 
//     i2c->CR2 |= I2C_CR2_START_M;

//     for(uint8_t i = 0; i < byte_count; i++)
//     {
        
//         while(!(i2c->ISR & I2C_ISR_RXNE_M)) // байт принят когда RXNE = 1
//         {
//             // if(i2c->ISR & I2C_ISR_NACKF_M)
//             // xprintf("Чтение. NACF = %d\n", (i2c->ISR & I2C_ISR_NACKF_M) >> I2C_ISR_NACKF_S);

//             if((i2c->ISR & I2C_ISR_STOPF_M) && (i2c->ISR & I2C_ISR_NACKF_M))
//             {
//                 xprintf("Чтение. STOPF = %d, NACKF = %d\n", (i2c->ISR & I2C_ISR_STOPF_M) >> I2C_ISR_STOPF_S, (i2c->ISR & I2C_ISR_NACKF_M) >> I2C_ISR_NACKF_S);
//                 break;
//             }
            
//         }

//         if((i2c->ISR & I2C_ISR_STOPF_M) && (i2c->ISR & I2C_ISR_NACKF_M))
//         {
//             i2c->ICR |= I2C_ICR_STOPCF_M | I2C_ICR_NACKCF_M; // сброс флага STOPF и сброс флага NACKF
//             break;
//         }
//         data[i] = i2c->RXDR; // чтение байта и сброс RXNE
//         xprintf("Чтение по адресу 0x%02x байта  0x%02x\n", slave_adr_print, data[i]);
//     }

// }




// int main()
// {
//     PM->AHB_CLK_MUX = 0; // выбор источника тактирования. 0 - внешний кварц
//     PM->CPU_RTC_CLK_MUX = 0;// выбор часового кварца как внешний
//     for (volatile int i = 0; i < 100; i++); 
//     WU->CLOCKS_SYS = 1 << 1; // выключение внутреннего системного кварца
//     WU->CLOCKS_BU = 1 << 1; // выключение внутреннего часовго кварца
//     for (volatile int i = 0; i < 100; i++) ; 
    

//     uint8_t slave_adr = 0x36; // адрес ведомого
//     uint16_t to_send = 300; // данные для оптавки
//     uint8_t to_send_byte = 1; // данные для оптавки
    
//     //uint8_t data[2] = {to_send >> 8, to_send & 0b0000000011111111}; // массив байтов на отправку

//     uint8_t data[1] = {to_send_byte}; // массив байтов на отправку
//     i2c_init(I2C_0); // инициализация блока i2c 
    

//     while (1)
//     {
        
//         //i2c_master_write(I2C_0, slave_adr, data, 2, false); // запись данных по адресу slave_adr = 0x36 без сдвига адреса
//         //i2c_master_write(I2C_0, slave_adr, data, 1, false); // запись данных по адресу slave_adr = 0x36 без сдвига адреса

//         //i2c_master_read(I2C_0, slave_adr, data, 2, false); // чтение данных по адресу slave_adr = 0x36 без сдвига адреса
//         i2c_master_read(I2C_0, slave_adr, data, 1, false); // чтение данных по адресу slave_adr = 0x36 без сдвига адреса

//         for (volatile int i = 0; i < 1000000; i++); 
//     }
    
    
// }