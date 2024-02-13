#include "power_manager.h"
#include "spifi.h"
#include "mcu32_memory_map.h"
#include "array.h"

#include "uart_lib.h"
#include "xprintf.h"

/*
* Данный пример демонстрирует работу с SPIFI.
* Прошивка из пример Blink для платы lite, на которой пользовательский светодиод подключен к выводу PORT2_7 преобразован в
* массив байт в файле array.h. Эти данные загружаются во внешнюю флеш по SPIFI.
* Процесс выполнения загрузки выводится по UART0. После успешной записи внешней флеш можно сменить режим загрузки перемычками BOOT
* на загрузку из внешней флеш. В этом случае светодиод начнет мигать.
*
* Работа с внешней флеш памятью идет в режиме Single spifi. 
*/

#define SREG1_BUSY                1

#define READ_SREG                 1
#define READ_LEN                  256
#define TIMEOUT                 100000

#define CHIP_ERASE_COMMAND    0xC7

#define WRITE_ENABLE_COMMAND    0x06
#define WRITE_DISABLE_COMMAND    0x04

#define MEM_CONFIG_COMMAND      0x61
#define MEM_CONFIG_VALUE        0x7F

#define READ_DATA_COMMAND         0x03
#define READ_SREG_COMMAND         0x05

#define PAGE_PROGRAM_COMMAND         0x02


void spifi_init()
{
    __HAL_PCC_SPIFI_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_PCC_GPIO_2_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_SERIAL;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    HAL_GPIO_Init(GPIO_2, &GPIO_InitStruct);

    /*
    *
    * STAT - регистр статуса
    * INTRQ - Запись «1» в бит сбрасывает запрос на прерывание от контроллера SPIFI
    * RESET - Бит предназначен для того, чтобы прервать текущую команду периферийного режима или режима памяти
    * 
    * ADDR - Исполнительный адрес команды
    * 
    * IDATA - регистр промежуточных данных
    * 
    * CLIMIT - Верхний предел кэшируемой памяти
    * 
    * CTRL - регистр управления
    * INTEN - Бит разрешения прерывания при завершении выполнения команды (если этот бит равен «1», то прерывание разрешено)
    * 
    */
    SPIFI_CONFIG->STAT |= SPIFI_CONFIG_STAT_INTRQ_M |
                          SPIFI_CONFIG_STAT_RESET_M;
    SPIFI_CONFIG->ADDR = 0x00;
    SPIFI_CONFIG->IDATA = 0x00;
    SPIFI_CONFIG->CLIMIT = 0x00000000;  
    //SPIFI_CONFIG -> CTRL |= SPIFI_CONFIG_CTRL_SCK_DIV(3); // должно быть 2Мгц

    // SPIFI_CONFIG->CTRL |= SPIFI_CONFIG_CTRL_INTEN_M;

    for (int i = 0; i < 10000000; i++);
    xprintf("Finish init SPIFI\n");
 
}


int SPIFI_WaitIntrqTimeout(SPIFI_CONFIG_TypeDef* spifi, uint32_t timeout)  
{
    while (timeout-- > 0)
    {
        if((spifi->STAT & SPIFI_CONFIG_STAT_INTRQ_M) != 0)
        {
            return 1;
        }
    }
    return 0;
}

void write_enable()
{
    /*
    *
    * CMD  код операции
    * OPCODE - код операции
    * FRAMEFORM - Бит управления полями кода операции и адреса команды:
    * «0» – резерв;
    * «1» – выдается только код операции, адреса нет; (SPIFI_CONFIG_CMD_FRAMEFORM_OPCODE_NOADDR)
    * «2» – код операции и младший байт адреса;
    * «3» – код операции и два младших байта адреса;
    * «4» – код операции и три младших байта адреса;
    * «5» – код операции и 4 байта адреса;
    * «6» – нет кода операции, три младших байта адре-са;
    * «7» – нет кода операции, 4 байта адреса
    * 
    * FIELDFORM - Формат вывода полей команды:
    * «0» – все поля выводятся в последовательном режиме; (SPIFI_CONFIG_CMD_FIELDFORM_ALL_SERIAL)
    * «1» – данные выводятся в четырех или двух битовом режиме, а остальные поля в последовательном режиме;
    * «2» – код операции выводится в последовательном режиме, а остальные в четырех или двух битовом;
    * «3» – все поля в четырех или двух битовом режиме
    * 
    */
    xprintf("Start write en\n");
    SPIFI_CONFIG->STAT |= SPIFI_CONFIG_STAT_INTRQ_M;
    SPIFI_CONFIG->CMD = (WRITE_ENABLE_COMMAND << SPIFI_CONFIG_CMD_OPCODE_S) |
                        (SPIFI_CONFIG_CMD_FRAMEFORM_OPCODE_NOADDR << SPIFI_CONFIG_CMD_FRAMEFORM_S)      |
                        (SPIFI_CONFIG_CMD_FIELDFORM_ALL_SERIAL << SPIFI_CONFIG_CMD_FIELDFORM_S);
    if(SPIFI_WaitIntrqTimeout(SPIFI_CONFIG, TIMEOUT) == 0)
    {
        xprintf("Timeout executing write enable command\n");
        return;
    }
}

uint8_t read_sreg_1()
{
    uint8_t read_sreg = 0;
    /*
    *
    * CMD  код операции
    * OPCODE - код операции
    * FRAMEFORM - Бит управления полями кода операции и адреса команды:
    * «0» – резерв;
    * «1» – выдается только код операции, адреса нет; (SPIFI_CONFIG_CMD_FRAMEFORM_OPCODE_NOADDR)
    * «2» – код операции и младший байт адреса;
    * «3» – код операции и два младших байта адреса;
    * «4» – код операции и три младших байта адреса;
    * «5» – код операции и 4 байта адреса;
    * «6» – нет кода операции, три младших байта адре-са;
    * «7» – нет кода операции, 4 байта адреса
    * 
    * FIELDFORM - Формат вывода полей команды:
    * «0» – все поля выводятся в последовательном режиме; (SPIFI_CONFIG_CMD_FIELDFORM_ALL_SERIAL)
    * «1» – данные выводятся в четырех или двух битовом режиме, а остальные поля в последовательном режиме;
    * «2» – код операции выводится в последовательном режиме, а остальные в четырех или двух битовом;
    * «3» – все поля в четырех или двух битовом режиме
    * 
    */
    SPIFI_CONFIG->STAT |= SPIFI_CONFIG_STAT_INTRQ_M;
    SPIFI_CONFIG->CMD = (READ_SREG_COMMAND << SPIFI_CONFIG_CMD_OPCODE_S) |
                        (SPIFI_CONFIG_CMD_FRAMEFORM_OPCODE_NOADDR << SPIFI_CONFIG_CMD_FRAMEFORM_S)      |
                        (SPIFI_CONFIG_CMD_FIELDFORM_ALL_SERIAL << SPIFI_CONFIG_CMD_FIELDFORM_S)      |
                        (READ_SREG << SPIFI_CONFIG_CMD_DATALEN_S);
    if(SPIFI_WaitIntrqTimeout(SPIFI_CONFIG, TIMEOUT) == 0)
    {
        xprintf("Timeout executing read sreg1 command\n");
        return 0;
    }

    read_sreg = SPIFI_CONFIG->DATA8;
    return read_sreg;
    
}

void wait_busy()
{
    xprintf("Wait\n");
    uint8_t sreg;
    while (1)
    {
        sreg = read_sreg_1();
        if(!(sreg & SREG1_BUSY))
        break;
    }
}

void chip_erase()
{
    xprintf("Start erase\n");
    SPIFI_CONFIG->STAT |= SPIFI_CONFIG_STAT_INTRQ_M;
    SPIFI_CONFIG->CMD = (CHIP_ERASE_COMMAND << SPIFI_CONFIG_CMD_OPCODE_S) |
                        (SPIFI_CONFIG_CMD_FRAMEFORM_OPCODE_NOADDR << SPIFI_CONFIG_CMD_FRAMEFORM_S)      |
                        (SPIFI_CONFIG_CMD_FIELDFORM_ALL_SERIAL << SPIFI_CONFIG_CMD_FIELDFORM_S);
    if(SPIFI_WaitIntrqTimeout(SPIFI_CONFIG, TIMEOUT) == 0)
    {
        xprintf("Timeout executing chip erase command");
        return;
    }
}   

void read_data(unsigned int address, int byte_count)
{
    
    xprintf("read data\n");
    char read_data[byte_count];
    SPIFI_CONFIG->ADDR = address;

    /*
    *
    * CMD  код операции
    * OPCODE - код операции
    * FRAMEFORM - Бит управления полями кода операции и адреса команды:
    * «0» – резерв;
    * «1» – выдается только код операции, адреса нет; (SPIFI_CONFIG_CMD_FRAMEFORM_OPCODE_NOADDR)
    * «2» – код операции и младший байт адреса;
    * «3» – код операции и два младших байта адреса;
    * «4» – код операции и три младших байта адреса;
    * «5» – код операции и 4 байта адреса;
    * «6» – нет кода операции, три младших байта адре-са;
    * «7» – нет кода операции, 4 байта адреса
    * 
    * FIELDFORM - Формат вывода полей команды:
    * «0» – все поля выводятся в последовательном режиме; (SPIFI_CONFIG_CMD_FIELDFORM_ALL_SERIAL)
    * «1» – данные выводятся в четырех или двух битовом режиме, а остальные поля в последовательном режиме;
    * «2» – код операции выводится в последовательном режиме, а остальные в четырех или двух битовом;
    * «3» – все поля в четырех или двух битовом режиме
    * 
    */
    SPIFI_CONFIG->STAT |= SPIFI_CONFIG_STAT_INTRQ_M;
    SPIFI_CONFIG->CMD = (READ_DATA_COMMAND << SPIFI_CONFIG_CMD_OPCODE_S) |
                        (SPIFI_CONFIG_CMD_FRAMEFORM_OPCODE_3ADDR << SPIFI_CONFIG_CMD_FRAMEFORM_S)      |
                        (SPIFI_CONFIG_CMD_FIELDFORM_ALL_SERIAL << SPIFI_CONFIG_CMD_FIELDFORM_S)      |
                        (byte_count << SPIFI_CONFIG_CMD_DATALEN_S);
    
    // if(SPIFI_WaitIntrqTimeout(SPIFI_CONFIG, TIMEOUT) == 0)
    // {
    //     TEST_ERROR("Timeout executing read data command");
    //     return;
    // }

    for (int i = 0; i < byte_count; i++)    
    {
        read_data[i] = SPIFI_CONFIG->DATA8;
        //xprintf("DATA[%d] = 0x%02x\n", address +i, read_data);
    }

    for (int i = 0; i < byte_count; i++)
    {
        if(read_data[i] != bin_data[address + i])
        {
            xprintf("DATA[%d] = 0x%02x - ошибка\n", address + i, read_data[i]);
        }
        
    }
}

void page_program(unsigned int ByteAddress, char data[], int byte_count)  {

    if(byte_count > 256)
    {
        xprintf("Количество байт больше 256\n");
    }
    xprintf("Start page program\n");

    //STAT:INTRQ
	SPIFI_CONFIG->STAT |= SPIFI_CONFIG_STAT_INTRQ_M;

	//ADDRESS
    SPIFI_CONFIG->ADDR = ByteAddress;

    //IDATA
    SPIFI_CONFIG->IDATA = 0x00;

    //CLIMIT
    SPIFI_CONFIG->CLIMIT = 0x00000000;

    //CMD
    SPIFI_CONFIG->CMD = (PAGE_PROGRAM_COMMAND << SPIFI_CONFIG_CMD_OPCODE_S) | 
                        (SPIFI_CONFIG_CMD_FRAMEFORM_OPCODE_3ADDR << SPIFI_CONFIG_CMD_FRAMEFORM_S) | 
                        (SPIFI_CONFIG_CMD_FIELDFORM_ALL_SERIAL << SPIFI_CONFIG_CMD_FIELDFORM_S) |
                        (0 << SPIFI_CONFIG_CMD_INTLEN_S) | 
                        (1 << SPIFI_CONFIG_CMD_DOUT_S) |
                        (0 << SPIFI_CONFIG_CMD_POLL_S) |
                        (byte_count << SPIFI_CONFIG_CMD_DATALEN_S);

    for(int i = ByteAddress; i < (ByteAddress + byte_count); i++)
    {
        SPIFI_CONFIG->DATA8 = data[i];
    }
    
    SPIFI_CONFIG->STAT |= SPIFI_CONFIG_STAT_INTRQ_M;

}


void erase()
{
    write_enable();
    chip_erase();
    wait_busy();
}

void write(int address, char data[], int data_len)
{
    if(data_len > 256)
    {
        xprintf("Количество байт больше 256\n");
        return;
    }
    
    write_enable();
    page_program(address, data, data_len);
    wait_busy();

    xprintf("written\n");
}


int main()
{       
    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);
    spifi_init();
    erase();
    int bin_data_len = sizeof(bin_data);
    xprintf("bin_data_len = %d\n", bin_data_len);
    int address = 0;

    for(address = 0; address < bin_data_len; address += 256)
    {
        if(address + 256 > bin_data_len)
        {
            break;
        }
        xprintf("address = %d\n", address);
        write(address, bin_data, 256);
        read_data(address, 256);
    }
    if((sizeof(bin_data) % 256) != 0)
    {
        xprintf("address = %d, +%d[%d]\n", address, bin_data_len - address-1, address + bin_data_len - address-1);
        write(address, bin_data, bin_data_len - address);
        read_data(address, bin_data_len - address);
    }
    
    xprintf("end\n");
    while (1)
    {
        // read_data();
        // for (int i = 0; i < 10000000; i++);
    }
}
