#ifndef SPIFI_W25Q_LIB_H_INCLUDED
#define SPIFI_W25Q_LIB_H_INCLUDED

#include "inttypes.h"

#define SPIFI_TIMEOUT				100000

#define SPIFI_ERROR_NO					1
#define SPIFI_ERROR_CMD_SET_TIMEOUT		2
#define SPIFI_ERROR_CMD_CLEAR_TIMEOUT	3
#define SPIFI_ERROR_INTRQ_SET_TIMEOUT	4

void SPIFI_StatIntrqClear(void);

unsigned int SPIFI_W25Q_ReadID(uint8_t *ID);
unsigned int SPIFI_W25Q_ReadStatusRegister1(uint8_t *RgStatus1);
unsigned int SPIFI_W25Q_ReadStatusRegister2(uint8_t *RgStatus2);
unsigned int SPIFI_W25Q_ReadStatusRegister3(uint8_t *RgStatus3);
unsigned int SPIFI_W25Q_WriteStatusRegister1(uint8_t RgStatus1);
unsigned int SPIFI_W25Q_WriteStatusRegister2(uint8_t RgStatus2);
unsigned int SPIFI_W25Q_WriteStatusRegister3(uint8_t RgStatus3);
unsigned int SPIFI_W25Q_WriteEnable(void);
unsigned int SPIFI_W25Q_BlockUnlock(unsigned int BlockAddress);
unsigned int SPIFI_W25Q_WaitBusyClear(void);
unsigned int SPIFI_W25Q_ByteRead(unsigned int ByteAddress, uint8_t *Byte);
unsigned int SPIFI_W25Q_ByteFastRead(unsigned int ByteAddress, uint8_t *Byte);
unsigned int SPIFI_W25Q_ByteFastReadDual(unsigned int ByteAddress, uint8_t *Byte);
unsigned int SPIFI_W25Q_ByteFastReadQuad(unsigned int ByteAddress, uint8_t *Byte);
unsigned int SPIFI_W25Q_SectorErase4K(unsigned int SectorAddress);
unsigned int SPIFI_W25Q_BlockErase64K(unsigned int BlockAddress);
unsigned int SPIFI_W25Q_ChipErase();
unsigned int SPIFI_W25Q_ByteWrite(unsigned int ByteAddress, uint8_t Byte);


void SPIFI_W25Q_ResetEnable(void);
void SPIFI_W25Q_Reset(void);
unsigned int SPIFI_W25Q_PageRead(unsigned int PageAddress, uint8_t *Page);
unsigned int SPIFI_W25Q_PageFastRead(unsigned int PageAddress, uint8_t *Page);
unsigned int SPIFI_W25Q_InitMemmode(void);
unsigned int SPIFI_W25Q_InitMemmodeQuad(void);

#endif

