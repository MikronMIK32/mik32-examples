
#include "common.h"
#include "spifi.h"
#include "epic.h"

#define SREG1_BUSY                1

#define READ_SREG                 1
#define READ_LEN                  1
#define TIMEOUT                 10000

#define CHIP_ERASE_COMMAND    0xC7

#define WRITE_ENABLE_COMMAND    0x06
#define WRITE_DISABLE_COMMAND    0x04

#define MEM_CONFIG_COMMAND      0x61
#define MEM_CONFIG_VALUE        0x7F

#define READ_DATA_COMMAND         0x03
#define READ_SREG_COMMAND         0x05

#define PAGE_PROGRAM_COMMAND         0x02


void InitSpifi()
{
    xprintf("Start init SPIFI\n");
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
    SPIFI_CONFIG -> CTRL |= SPIFI_CONFIG_CTRL_SCK_DIV(3); // должно быть 2Мгц
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
        TEST_ERROR("Timeout executing write enable command");
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
                        (1 << SPIFI_CONFIG_CMD_DATALEN_S);
    if(SPIFI_WaitIntrqTimeout(SPIFI_CONFIG, TIMEOUT) == 0)
    {
        TEST_ERROR("Timeout executing read sreg1 command");
        return 0;
    }

    read_sreg = SPIFI_CONFIG->DATA8;
    return read_sreg;
    
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
        TEST_ERROR("Timeout executing chip erase command");
        return;
    }
}   

void read_data()
{
    xprintf("read data\n");
    uint8_t read_data;
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
                        (READ_LEN << SPIFI_CONFIG_CMD_DATALEN_S);
    
    if(SPIFI_WaitIntrqTimeout(SPIFI_CONFIG, TIMEOUT) == 0)
    {
        TEST_ERROR("Timeout executing read data command");
        return;
    }

    read_data = SPIFI_CONFIG->DATA8;
    xprintf("DATA == 0x%02x\n", read_data);
}

void page_program()
{
    xprintf("Start page program\n");
    SPIFI_CONFIG->STAT |= SPIFI_CONFIG_STAT_INTRQ_M;
    SPIFI_CONFIG->CMD = (PAGE_PROGRAM_COMMAND << SPIFI_CONFIG_CMD_OPCODE_S) |
                        (SPIFI_CONFIG_CMD_FRAMEFORM_OPCODE_3ADDR << SPIFI_CONFIG_CMD_FRAMEFORM_S)      |
                        (SPIFI_CONFIG_CMD_FIELDFORM_ALL_SERIAL << SPIFI_CONFIG_CMD_FIELDFORM_S) |
                        SPIFI_CONFIG_CMD_DOUT_M | 
                        SPIFI_CONFIG_CMD_POLL_M |
                        (0b0000 << SPIFI_CONFIG_CMD_DATALEN_S);
    
    SPIFI_CONFIG->DATA8 = 0;
    // for(int i = 0; i < 255; i++)
    // {
    //     SPIFI_CONFIG->DATA8 = i;
    // }
    // if(SPIFI_WaitIntrqTimeout(SPIFI_CONFIG, TIMEOUT) == 0)
    // {
    //     TEST_ERROR("Timeout executing page program command");
    //     return;
    // }
    while ((SPIFI_CONFIG -> STAT) & SPIFI_CONFIG_STAT_CMD_M);
    
    xprintf("Page program Completed\n");
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

void write()
{
    write_enable();
    chip_erase();
    wait_busy();


    write_enable();
    page_program();
    //wait_busy();
}


int main()
{       
    InitSpifi();
    write();
    //read_data();

    while (1)
    {
        // read_data();
        // for (int i = 0; i < 10000000; i++);
    }
    

}
