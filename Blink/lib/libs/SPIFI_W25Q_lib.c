#include "W25Q.h"
#include "SPIFI_W25Q_lib.h"
#include "spifi.h"
#include "mcu32_memory_map.h"


static unsigned int SPIFI_WaitCmdSet(void);
static unsigned int SPIFI_WaitCmdClear(void);
static unsigned int SPIFI_StatIntrqWaitSet(void);
static unsigned int SPIFI_StatMcinitWaitSet(void);

unsigned int SPIFI_W25Q_ReadID(uint8_t *ID)  {
	unsigned int RgCmd = 0;
	unsigned int ResIntrqWaitSet = 0;

	//����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

	//ADDRESS
    SPIFI_CONFIG->ADDR = 0x00;

    //IDATA
    SPIFI_CONFIG->IDATA = 0x00;

    //CLIMIT
    SPIFI_CONFIG->CLIMIT = 0x00000000;

    //CMD
    RgCmd |= (W25Q_READ_JEDEC_ID << SPIFI_CONFIG_CMD_OPCODE_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FRAMEFORM_OPCODE_NOADDR << SPIFI_CONFIG_CMD_FRAMEFORM_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FIELDFORM_ALL_SERIAL << SPIFI_CONFIG_CMD_FIELDFORM_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_INTLEN_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_DOUT_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_POLL_S);
    RgCmd |= (3 << SPIFI_CONFIG_CMD_DATALEN_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_DONE_VALUE_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_INDEX_S);
    SPIFI_CONFIG->CMD = RgCmd;

    //�������� ��������� ���� STAT->INTRQ
    ResIntrqWaitSet = SPIFI_StatIntrqWaitSet();
    if(ResIntrqWaitSet==0)  {
    	//���� �������� ��������� ���� STAT:RESET
    	return 0;
    }
    //����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

    ID[0] = SPIFI_CONFIG->DATA8;
	ID[1] = SPIFI_CONFIG->DATA8;
	ID[2] = SPIFI_CONFIG->DATA8;

	return 1;
}



unsigned int SPIFI_W25Q_ReadStatusRegister1(uint8_t *RgStatus1)  {
	unsigned int RgCmd = 0;
	unsigned int ResIntrqWaitSet = 0;

	//����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

	//ADDRESS
    SPIFI_CONFIG->ADDR = 0x00;

    //IDATA
    SPIFI_CONFIG->IDATA = 0x00;

    //CLIMIT
    SPIFI_CONFIG->CLIMIT = 0x00000000;

    //CMD
    RgCmd |= (W25Q_READ_STATUS_REGISTER_1 << SPIFI_CONFIG_CMD_OPCODE_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FRAMEFORM_OPCODE_NOADDR << SPIFI_CONFIG_CMD_FRAMEFORM_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FIELDFORM_ALL_SERIAL << SPIFI_CONFIG_CMD_FIELDFORM_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_INTLEN_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_DOUT_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_POLL_S);
    RgCmd |= (1 << SPIFI_CONFIG_CMD_DATALEN_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_DONE_VALUE_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_INDEX_S);
    SPIFI_CONFIG->CMD = RgCmd;

    //�������� ��������� ���� STAT->INTRQ
    ResIntrqWaitSet = SPIFI_StatIntrqWaitSet();
    if(ResIntrqWaitSet==0)  {
    	//���� �������� ��������� ���� STAT:RESET
    	return 0;
    }
    //����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

    *RgStatus1 = SPIFI_CONFIG->DATA8;

	return 1;
}



unsigned int SPIFI_W25Q_ReadStatusRegister2(uint8_t *RgStatus2)  {
	unsigned int RgCmd = 0;
	unsigned int ResIntrqWaitSet = 0;

	//����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

	//ADDRESS
    SPIFI_CONFIG->ADDR = 0x00;

    //IDATA
    SPIFI_CONFIG->IDATA = 0x00;

    //CLIMIT
    SPIFI_CONFIG->CLIMIT = 0x00000000;

    //CMD
    RgCmd |= (W25Q_READ_STATUS_REGISTER_2 << SPIFI_CONFIG_CMD_OPCODE_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FRAMEFORM_OPCODE_NOADDR << SPIFI_CONFIG_CMD_FRAMEFORM_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FIELDFORM_ALL_SERIAL << SPIFI_CONFIG_CMD_FIELDFORM_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_INTLEN_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_DOUT_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_POLL_S);
    RgCmd |= (1 << SPIFI_CONFIG_CMD_DATALEN_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_DONE_VALUE_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_INDEX_S);
    SPIFI_CONFIG->CMD = RgCmd;

    //�������� ��������� ���� STAT->INTRQ
    ResIntrqWaitSet = SPIFI_StatIntrqWaitSet();
    if(ResIntrqWaitSet==0)  {
    	//���� �������� ��������� ���� STAT:RESET
    	return 0;
    }
    //����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

    *RgStatus2 = SPIFI_CONFIG->DATA8;

	return 1;
}



unsigned int SPIFI_W25Q_ReadStatusRegister3(uint8_t *RgStatus3)  {
	unsigned int Error = 0;
	unsigned int RgCmd = 0;
	unsigned int i = 0;
	unsigned int ResIntrqWaitSet = 0;

	//����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

	//ADDRESS
    SPIFI_CONFIG->ADDR = 0x00;

    //IDATA
    SPIFI_CONFIG->IDATA = 0x00;

    //CLIMIT
    SPIFI_CONFIG->CLIMIT = 0x00000000;

    //CMD
    RgCmd |= (W25Q_READ_STATUS_REGISTER_3 << SPIFI_CONFIG_CMD_OPCODE_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FRAMEFORM_OPCODE_NOADDR << SPIFI_CONFIG_CMD_FRAMEFORM_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FIELDFORM_ALL_SERIAL << SPIFI_CONFIG_CMD_FIELDFORM_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_INTLEN_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_DOUT_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_POLL_S);
    RgCmd |= (1 << SPIFI_CONFIG_CMD_DATALEN_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_DONE_VALUE_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_INDEX_S);
    SPIFI_CONFIG->CMD = RgCmd;

    //�������� ��������� ���� STAT->INTRQ
    ResIntrqWaitSet = SPIFI_StatIntrqWaitSet();
    if(ResIntrqWaitSet==0)  {
    	//���� �������� ��������� ���� STAT:RESET
    	return 0;
    }
    //����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

    *RgStatus3 = SPIFI_CONFIG->DATA8;

	return 1;
}



unsigned int SPIFI_W25Q_WriteStatusRegister1(uint8_t RgStatus1)  {
	unsigned int Error = 0;
	unsigned int RgCmd = 0;
	unsigned int i = 0;
	unsigned int ResWriteEnable = 0;
	unsigned int ResIntrqWaitSet = 0;
	unsigned int ResBusyWait = 0;

	//����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

	//-----������� WriteEnable-------
	ResWriteEnable = SPIFI_W25Q_WriteEnable();
	if(ResWriteEnable==0)  {
		return 0;
	}

	//ADDRESS
    SPIFI_CONFIG->ADDR = 0x00;

    //IDATA
    SPIFI_CONFIG->IDATA = 0x00;

    //CLIMIT
    SPIFI_CONFIG->CLIMIT = 0x00000000;

    //CMD
    RgCmd |= (W25Q_WRITE_STATUS_REGISTER_1 << SPIFI_CONFIG_CMD_OPCODE_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FRAMEFORM_OPCODE_NOADDR << SPIFI_CONFIG_CMD_FRAMEFORM_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FIELDFORM_ALL_SERIAL << SPIFI_CONFIG_CMD_FIELDFORM_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_INTLEN_S);
    RgCmd |= (1 << SPIFI_CONFIG_CMD_DOUT_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_POLL_S);
    RgCmd |= (1 << SPIFI_CONFIG_CMD_DATALEN_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_DONE_VALUE_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_INDEX_S);
    SPIFI_CONFIG->CMD = RgCmd;

    //DATA
    SPIFI_CONFIG->DATA8 = RgStatus1;

    //�������� ��������� ���� STAT->INTRQ
    ResIntrqWaitSet = SPIFI_StatIntrqWaitSet();
    if(ResIntrqWaitSet==0)  {
    	//���� �������� ��������� ���� STAT:RESET
    	return 0;
    }
    //����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

    //�������� ��������� ������ ��������
    ResBusyWait = SPIFI_W25Q_WaitBusyClear();
    if(ResBusyWait==0)  {
    	//���� �������� ��������� ���� STAT:RESET
    	return 0;
    }

	return 1;
}



unsigned int SPIFI_W25Q_WriteStatusRegister2(uint8_t RgStatus2)  {
	unsigned int Error = 0;
	unsigned int RgCmd = 0;
	unsigned int i = 0;
	unsigned int ResWriteEnable = 0;
	unsigned int ResIntrqWaitSet = 0;
	unsigned int ResBusyWait = 0;

	//����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

	//-----������� WriteEnable-------
	ResWriteEnable = SPIFI_W25Q_WriteEnable();
	if(ResWriteEnable==0)  {
		return 0;
	}

	//ADDRESS
    SPIFI_CONFIG->ADDR = 0x00;

    //IDATA
    SPIFI_CONFIG->IDATA = 0x00;

    //CLIMIT
    SPIFI_CONFIG->CLIMIT = 0x00000000;

    //CMD
    RgCmd |= (W25Q_WRITE_STATUS_REGISTER_2 << SPIFI_CONFIG_CMD_OPCODE_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FRAMEFORM_OPCODE_NOADDR << SPIFI_CONFIG_CMD_FRAMEFORM_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FIELDFORM_ALL_SERIAL << SPIFI_CONFIG_CMD_FIELDFORM_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_INTLEN_S);
    RgCmd |= (1 << SPIFI_CONFIG_CMD_DOUT_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_POLL_S);
    RgCmd |= (1 << SPIFI_CONFIG_CMD_DATALEN_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_DONE_VALUE_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_INDEX_S);
    SPIFI_CONFIG->CMD = RgCmd;

    //DATA
    SPIFI_CONFIG->DATA8 = RgStatus2;

    //�������� ��������� ���� STAT->INTRQ
    ResIntrqWaitSet = SPIFI_StatIntrqWaitSet();
    if(ResIntrqWaitSet==0)  {
    	//���� �������� ��������� ���� STAT:RESET
    	return 0;
    }
    //����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

    //�������� ��������� ������ ��������
    ResBusyWait = SPIFI_W25Q_WaitBusyClear();
    if(ResBusyWait==0)  {
    	//���� �������� ��������� ���� STAT:RESET
    	return 0;
    }

	return 1;
}



unsigned int SPIFI_W25Q_WriteStatusRegister3(uint8_t RgStatus3)  {
	unsigned int Error = 0;
	unsigned int RgCmd = 0;
	unsigned int i = 0;
	unsigned int ResWriteEnable = 0;
	unsigned int ResIntrqWaitSet = 0;
	unsigned int ResBusyWait = 0;

	//����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

	//-----������� WriteEnable-------
	ResWriteEnable = SPIFI_W25Q_WriteEnable();
	if(ResWriteEnable==0)  {
		return 0;
	}

	//ADDRESS
    SPIFI_CONFIG->ADDR = 0x00;

    //IDATA
    SPIFI_CONFIG->IDATA = 0x00;

    //CLIMIT
    SPIFI_CONFIG->CLIMIT = 0x00000000;

    //CMD
    RgCmd |= (W25Q_WRITE_STATUS_REGISTER_3 << SPIFI_CONFIG_CMD_OPCODE_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FRAMEFORM_OPCODE_NOADDR << SPIFI_CONFIG_CMD_FRAMEFORM_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FIELDFORM_ALL_SERIAL << SPIFI_CONFIG_CMD_FIELDFORM_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_INTLEN_S);
    RgCmd |= (1 << SPIFI_CONFIG_CMD_DOUT_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_POLL_S);
    RgCmd |= (1 << SPIFI_CONFIG_CMD_DATALEN_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_DONE_VALUE_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_INDEX_S);
    SPIFI_CONFIG->CMD = RgCmd;

    //DATA
    SPIFI_CONFIG->DATA8 = RgStatus3;

    //�������� ��������� ���� STAT->INTRQ
    ResIntrqWaitSet = SPIFI_StatIntrqWaitSet();
    if(ResIntrqWaitSet==0)  {
    	//���� �������� ��������� ���� STAT:RESET
    	return 0;
    }
    //����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

    //�������� ��������� ������ ��������
    ResBusyWait = SPIFI_W25Q_WaitBusyClear();
    if(ResBusyWait==0)  {
    	//���� �������� ��������� ���� STAT:RESET
    	return 0;
    }

	return 1;
}



unsigned int SPIFI_W25Q_WriteEnable(void)  {
	unsigned int RgCmd = 0;
	unsigned int ResIntrqWaitSet = 0;

	//����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

	//ADDRESS
    SPIFI_CONFIG->ADDR = 0x00;

    //IDATA
    SPIFI_CONFIG->IDATA = 0x00;

    //CLIMIT
    SPIFI_CONFIG->CLIMIT = 0x00000000;

    //CMD
    RgCmd |= (W25Q_WRITE_ENABLE << SPIFI_CONFIG_CMD_OPCODE_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FRAMEFORM_OPCODE_NOADDR << SPIFI_CONFIG_CMD_FRAMEFORM_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FIELDFORM_ALL_SERIAL << SPIFI_CONFIG_CMD_FIELDFORM_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_INTLEN_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_DOUT_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_POLL_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_DONE_VALUE_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_INDEX_S);
    SPIFI_CONFIG->CMD = RgCmd;

    //�������� ��������� ���� STAT->INTRQ
    ResIntrqWaitSet = SPIFI_StatIntrqWaitSet();
    if(ResIntrqWaitSet==0)  {
    	//���� �������� ��������� ���� STAT:RESET
    	return 0;
    }
    //����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

	return 1;
}



unsigned int SPIFI_W25Q_BlockUnlock(unsigned int BlockAddress)  {
	unsigned int RgCmd = 0;
	unsigned int ResIntrqWaitSet = 0;
	unsigned int ResWriteEnable = 0;

	//������� Write Enable
    ResWriteEnable = SPIFI_W25Q_WriteEnable();
    if(ResWriteEnable==0)  {
    	return 0;
    }

	//����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

	//ADDRESS
    SPIFI_CONFIG->ADDR = BlockAddress;

    //IDATA
    SPIFI_CONFIG->IDATA = 0x00;

    //CLIMIT
    SPIFI_CONFIG->CLIMIT = 0x00000000;

    //CMD
    RgCmd |= (W25Q_BLOCK_UNLOCK << SPIFI_CONFIG_CMD_OPCODE_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FRAMEFORM_OPCODE_3ADDR << SPIFI_CONFIG_CMD_FRAMEFORM_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FIELDFORM_ALL_SERIAL << SPIFI_CONFIG_CMD_FIELDFORM_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_INTLEN_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_DOUT_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_POLL_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_DONE_VALUE_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_INDEX_S);
    SPIFI_CONFIG->CMD = RgCmd;

    //�������� ��������� ���� STAT->INTRQ
    ResIntrqWaitSet = SPIFI_StatIntrqWaitSet();
    if(ResIntrqWaitSet==0)  {
    	//���� �������� ��������� ���� STAT:RESET
    	return 0;
    }
    //����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

	return 1;
}



unsigned int SPIFI_W25Q_ByteRead(unsigned int ByteAddress, uint8_t *Byte)  {
	unsigned int RgCmd = 0;
	unsigned int ResIntrqWaitSet = 0;

	//����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

	//ADDRESS
    SPIFI_CONFIG->ADDR = ByteAddress;

    //IDATA
    SPIFI_CONFIG->IDATA = 0x00;

    //CLIMIT
    SPIFI_CONFIG->CLIMIT = 0x00000000;

    //CMD
    RgCmd |= (W25Q_READ_DATA << SPIFI_CONFIG_CMD_OPCODE_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FRAMEFORM_OPCODE_3ADDR << SPIFI_CONFIG_CMD_FRAMEFORM_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FIELDFORM_ALL_SERIAL << SPIFI_CONFIG_CMD_FIELDFORM_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_INTLEN_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_DOUT_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_POLL_S);
    RgCmd |= (1 << SPIFI_CONFIG_CMD_DATALEN_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_DONE_VALUE_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_INDEX_S);
    SPIFI_CONFIG->CMD = RgCmd;

    //�������� ��������� ���� STAT->INTRQ
    ResIntrqWaitSet = SPIFI_StatIntrqWaitSet();
    if(ResIntrqWaitSet==0)  {
    	//���� �������� ��������� ���� STAT:RESET
    	return 0;
    }
    //����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

    *Byte = SPIFI_CONFIG->DATA8;

    return 1;
}



unsigned int SPIFI_W25Q_ByteFastRead(unsigned int ByteAddress, uint8_t *Byte)  {
	unsigned int RgCmd = 0;
	unsigned int ResIntrqWaitSet = 0;

	//����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

	//ADDRESS
    SPIFI_CONFIG->ADDR = ByteAddress;

    //IDATA
    SPIFI_CONFIG->IDATA = 0x55;

    //CLIMIT
    SPIFI_CONFIG->CLIMIT = 0x00000000;

    //CMD
    RgCmd |= (W25Q_FAST_READ << SPIFI_CONFIG_CMD_OPCODE_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FRAMEFORM_OPCODE_3ADDR << SPIFI_CONFIG_CMD_FRAMEFORM_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FIELDFORM_ALL_SERIAL << SPIFI_CONFIG_CMD_FIELDFORM_S);
    RgCmd |= (1 << SPIFI_CONFIG_CMD_INTLEN_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_DOUT_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_POLL_S);
    RgCmd |= (1 << SPIFI_CONFIG_CMD_DATALEN_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_DONE_VALUE_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_INDEX_S);
    SPIFI_CONFIG->CMD = RgCmd;

    //�������� ��������� ���� STAT->INTRQ
    ResIntrqWaitSet = SPIFI_StatIntrqWaitSet();
    if(ResIntrqWaitSet==0)  {
    	//���� �������� ��������� ���� STAT:RESET
    	return 0;
    }
    //����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

    *Byte = SPIFI_CONFIG->DATA8;

    return 1;
}



unsigned int SPIFI_W25Q_ByteFastReadDual(unsigned int ByteAddress, uint8_t *Byte)  {
	unsigned int RgCmd = 0;
	unsigned int ResIntrqWaitSet = 0;

	//����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

	//ADDRESS
    SPIFI_CONFIG->ADDR = ByteAddress;

    //IDATA
    SPIFI_CONFIG->IDATA = 0x55;

    //CLIMIT
    SPIFI_CONFIG->CLIMIT = 0x00000000;

    //SPIFI_CONFIG->CTRL:DUAL
    SPIFI_CONFIG->CTRL |= (1<<SPIFI_CONFIG_CTRL_DUAL_S);

    //CMD
    RgCmd |= (W25Q_FAST_READ_DUAL_OUTPUT << SPIFI_CONFIG_CMD_OPCODE_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FRAMEFORM_OPCODE_3ADDR << SPIFI_CONFIG_CMD_FRAMEFORM_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FIELDFORM_DATA_PARALLEL << SPIFI_CONFIG_CMD_FIELDFORM_S);
    RgCmd |= (1 << SPIFI_CONFIG_CMD_INTLEN_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_DOUT_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_POLL_S);
    RgCmd |= (1 << SPIFI_CONFIG_CMD_DATALEN_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_DONE_VALUE_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_INDEX_S);
    SPIFI_CONFIG->CMD = RgCmd;

    //�������� ��������� ���� STAT->INTRQ
    ResIntrqWaitSet = SPIFI_StatIntrqWaitSet();
    if(ResIntrqWaitSet==0)  {
    	//���� �������� ��������� ���� STAT:RESET
    	return 0;
    }
    //����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

    *Byte = SPIFI_CONFIG->DATA8;

    return 1;
}



unsigned int SPIFI_W25Q_ByteFastReadQuad(unsigned int ByteAddress, uint8_t *Byte)  {
	unsigned int RgCmd = 0;
	unsigned int ResIntrqWaitSet = 0;

	//����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

	//ADDRESS
    SPIFI_CONFIG->ADDR = ByteAddress;

    //IDATA
    SPIFI_CONFIG->IDATA = 0x55;

    //CLIMIT
    SPIFI_CONFIG->CLIMIT = 0x00000000;

    //���������� ��� CTRL:DUAL, ����� ������������ 4 �������
    SPIFI_CONFIG->CTRL &= ~(1<<SPIFI_CONFIG_CTRL_DUAL_S);

    //CMD
    RgCmd |= (W25Q_FAST_READ_QUAD_OUTPUT << SPIFI_CONFIG_CMD_OPCODE_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FRAMEFORM_OPCODE_3ADDR << SPIFI_CONFIG_CMD_FRAMEFORM_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FIELDFORM_DATA_PARALLEL << SPIFI_CONFIG_CMD_FIELDFORM_S);
    RgCmd |= (1 << SPIFI_CONFIG_CMD_INTLEN_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_DOUT_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_POLL_S);
    RgCmd |= (1 << SPIFI_CONFIG_CMD_DATALEN_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_DONE_VALUE_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_INDEX_S);
    SPIFI_CONFIG->CMD = RgCmd;

    //�������� ��������� ���� STAT->INTRQ
    ResIntrqWaitSet = SPIFI_StatIntrqWaitSet();
    if(ResIntrqWaitSet==0)  {
    	//���� �������� ��������� ���� STAT:RESET
    	return 0;
    }
    //����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

    *Byte = SPIFI_CONFIG->DATA8;

    return 1;
}



/*
unsigned int SPIFI_W25Q_SectorErase4K(unsigned int SectorAddress)  {
	unsigned int i = 0;
	unsigned int NBytes = 4;
	uint8_t Cmd[4] = {0};
	uint8_t Reply[4] = {0};

	W25Q_WriteEnable();

	Cmd[0] = W25Q_SECTOR_ERASE_4K;
	Cmd[1] = (SectorAddress>>16) & 0xFF;
	Cmd[2] = (SectorAddress>>8) & 0xFF;
	Cmd[3] = (SectorAddress>>0) & 0xFF;

	SPI_CS_Enable(W25Q_SPI, W25Q_SPI_CS);
	SPI_Exchange(W25Q_SPI, Cmd, Reply, NBytes);
	SPI_CS_Disable(W25Q_SPI, W25Q_SPI_CS);

	SomeDelay(100);
    	//while((K1636_ReadStatusRegister() & K1636RR_STATUS_BSY_M)!=0) {}	//!!!���� ���������!!!
	for(i=0; i<W25Q_TIMEOUT; i++)  {
		if((W25Q_ReadStatusRegister1() & W25Q_STATUS_1_BUSY_M) == 0)  {
			return 1;
		}
	}
	return 0;
}
*/


unsigned int SPIFI_W25Q_BlockErase64K(unsigned int BlockAddress)  {
	unsigned int RgCmd = 0;
	unsigned int ResBlockUnlock = 0;
	unsigned int ResIntrqWaitSet = 0;
	unsigned int ResWriteEnable = 0;

	//������� Block Unlock
    ResBlockUnlock = SPIFI_W25Q_BlockUnlock(BlockAddress);
    if(ResBlockUnlock==0)  {
    	return 0;
    }

	//������� Write Enable
    ResWriteEnable = SPIFI_W25Q_WriteEnable();
    if(ResWriteEnable==0)  {
    	return 0;
    }

	//����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

	//ADDRESS
    SPIFI_CONFIG->ADDR = BlockAddress;

    //IDATA
    SPIFI_CONFIG->IDATA = 0x00;

    //CLIMIT
    SPIFI_CONFIG->CLIMIT = 0x00000000;

    //CMD
    RgCmd |= (W25Q_BLOCK_ERASE_64K << SPIFI_CONFIG_CMD_OPCODE_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FRAMEFORM_OPCODE_3ADDR << SPIFI_CONFIG_CMD_FRAMEFORM_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FIELDFORM_ALL_SERIAL << SPIFI_CONFIG_CMD_FIELDFORM_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_INTLEN_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_DOUT_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_POLL_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_DONE_VALUE_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_INDEX_S);
    SPIFI_CONFIG->CMD = RgCmd;

    //�������� ��������� ���� STAT->INTRQ
    ResIntrqWaitSet = SPIFI_StatIntrqWaitSet();
    if(ResIntrqWaitSet==0)  {
    	//���� �������� ��������� ���� STAT:RESET
    	return 0;
    }
    //����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

    return 1;
}


unsigned int SPIFI_W25Q_ChipErase()  {
    unsigned int RgCmd = 0;
    unsigned int ResIntrqWaitSet = 0;
    unsigned int ResWriteEnable = 0;


    //������� Write Enable
    ResWriteEnable = SPIFI_W25Q_WriteEnable();
    if(ResWriteEnable==0)  {
        return 0;
    }

    //����� ���� STAT:INTRQ
    SPIFI_StatIntrqClear();

    //ADDRESS
    SPIFI_CONFIG->ADDR = 0000;

    //IDATA
    SPIFI_CONFIG->IDATA = 0x00;

    //CLIMIT
    SPIFI_CONFIG->CLIMIT = 0x00000000;

    //CMD
    RgCmd |= (W25Q_CHIP_ERASE << SPIFI_CONFIG_CMD_OPCODE_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FRAMEFORM_OPCODE_NOADDR << SPIFI_CONFIG_CMD_FRAMEFORM_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FIELDFORM_ALL_SERIAL << SPIFI_CONFIG_CMD_FIELDFORM_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_INTLEN_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_DOUT_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_POLL_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_S);
        //RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_DONE_VALUE_S);
        //RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_INDEX_S);
    SPIFI_CONFIG->CMD = RgCmd;

    
    //�������� ��������� ���� STAT->INTRQ
    ResIntrqWaitSet = SPIFI_StatIntrqWaitSet();
    if(ResIntrqWaitSet==0)  {
        //���� �������� ��������� ���� STAT:RESET
        return 0;
    }
    //����� ���� STAT:INTRQ
    SPIFI_StatIntrqClear();

    return 1;
}



unsigned int SPIFI_W25Q_WaitBusyClear(void)  {
	unsigned int RgCmd = 0;
	unsigned int ResIntrqWaitSet = 0;
	uint8_t RgStatus = 0;

	//����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

	//ADDRESS
    SPIFI_CONFIG->ADDR = 0x00;

    //IDATA
    SPIFI_CONFIG->IDATA = 0x00;

    //CLIMIT
    SPIFI_CONFIG->CLIMIT = 0x00000000;

    //CMD
    RgCmd |= (W25Q_READ_STATUS_REGISTER_1 << SPIFI_CONFIG_CMD_OPCODE_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FRAMEFORM_OPCODE_NOADDR << SPIFI_CONFIG_CMD_FRAMEFORM_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FIELDFORM_ALL_SERIAL << SPIFI_CONFIG_CMD_FIELDFORM_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_INTLEN_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_DOUT_S);
    RgCmd |= (1 << SPIFI_CONFIG_CMD_POLL_S);
    	//RgCmd |= (1 << SPIFI_CONFIG_CMD_DATALEN_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_DONE_VALUE_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_INDEX_S);
    SPIFI_CONFIG->CMD = RgCmd;

    //�������� ������ ���� STAT->BUSY
    //����� BUSY ������ ������ ����, ������� ����� ��������� ����������,
    //� ����������� ��� INTRQ
    ResIntrqWaitSet = SPIFI_StatIntrqWaitSet();
    if(ResIntrqWaitSet==0)  {
    	//���� �������� ��������� ���� STAT:RESET
    	return 0;
    }
    //����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

	//������ FIFO, �.�. ��� ������� ������� ������� ����������
	RgStatus = SPIFI_CONFIG->DATA8;

    return 1;
}



unsigned int SPIFI_W25Q_ByteWrite(unsigned int ByteAddress, uint8_t Byte)  {
	unsigned int Error = 0;
	unsigned int RgCmd = 0;
	unsigned int i = 0;
	unsigned int ResWriteEnable = 0;
	unsigned int ResIntrqWaitSet = 0;
	unsigned int ResBusyWait = 0;

	//����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

	//-----������� WriteEnable-------
	ResWriteEnable = SPIFI_W25Q_WriteEnable();
	if(ResWriteEnable==0)  {
		return 0;
	}

	//����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

	//ADDRESS
    SPIFI_CONFIG->ADDR = ByteAddress;

    //IDATA
    SPIFI_CONFIG->IDATA = 0x00;

    //CLIMIT
    SPIFI_CONFIG->CLIMIT = 0x00000000;

    //CMD
    RgCmd |= (W25Q_PAGE_PROGRAM << SPIFI_CONFIG_CMD_OPCODE_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FRAMEFORM_OPCODE_3ADDR << SPIFI_CONFIG_CMD_FRAMEFORM_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FIELDFORM_ALL_SERIAL << SPIFI_CONFIG_CMD_FIELDFORM_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_INTLEN_S);
    RgCmd |= (1 << SPIFI_CONFIG_CMD_DOUT_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_POLL_S);
    RgCmd |= (1 << SPIFI_CONFIG_CMD_DATALEN_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_DONE_VALUE_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_INDEX_S);
    SPIFI_CONFIG->CMD = RgCmd;

    //DATA
    SPIFI_CONFIG->DATA8 = Byte;

    //�������� ������ ���� STAT->BUSY
    //����� BUSY ������ ������ ����, ������� ����� ��������� ����������,
    //� ����������� ��� INTRQ
    ResIntrqWaitSet = SPIFI_StatIntrqWaitSet();
    if(ResIntrqWaitSet==0)  {
    	//���� �������� ��������� ���� STAT:RESET
    	return 0;
    }
    //����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

    //-----------����� ����� Busy-------------
    ResBusyWait = SPIFI_W25Q_WaitBusyClear();
    if(ResBusyWait==0)  {
    	//���� �������� ��������� ���� STAT:RESET
    	return 0;
    }

	return 1;
}



unsigned int SPIFI_W25Q_PageRead(unsigned int PageAddress, uint8_t *Page)  {
	unsigned int Error = 0;
	unsigned int RgCmd = 0;
	unsigned int i = 0;
	unsigned int ResIntrqWaitSet = 0;

	unsigned int SizeTemp = 16;

	//����� ����������� SPIFI
	SPIFI_StatResetSet();

	//����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

	//ADDRESS
    SPIFI_CONFIG->ADDR = PageAddress;

    //IDATA
    SPIFI_CONFIG->IDATA = 0x00;

    //CLIMIT
    SPIFI_CONFIG->CLIMIT = 0x00000000;

    //CMD
    RgCmd |= (W25Q_READ_DATA << SPIFI_CONFIG_CMD_OPCODE_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FRAMEFORM_OPCODE_3ADDR << SPIFI_CONFIG_CMD_FRAMEFORM_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FIELDFORM_ALL_SERIAL << SPIFI_CONFIG_CMD_FIELDFORM_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_INTLEN_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_DOUT_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_POLL_S);
    RgCmd |= (SizeTemp << SPIFI_CONFIG_CMD_DATALEN_S);
    	//RgCmd |= (W25Q_PAGE_SIZE << SPIFI_CONFIG_CMD_DATALEN_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_DONE_VALUE_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_INDEX_S);
    SPIFI_CONFIG->CMD = RgCmd;

    //�������� ��������� ���� STAT->INTRQ
    ResIntrqWaitSet = SPIFI_StatIntrqWaitSet();
    if(ResIntrqWaitSet==0)  {
    	//���� �������� ��������� ���� STAT:RESET
    	return 0;
    }
    //����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

	for(i=0; i<SizeTemp; i++)  {
		Page[i] = SPIFI_CONFIG->DATA8;
	}
	//for(i=0; i<W25Q_PAGE_SIZE; i++)  {
	//	Page[i] = SPIFI_CONFIG->DATA8;
	//}

    return 1;
}



unsigned int SPIFI_W25Q_PageFastRead(unsigned int PageAddress, uint8_t *Page)  {
	//unsigned int Error = 0;
	unsigned int RgCmd = 0;
	unsigned int i = 0;
	unsigned int ResIntrqWaitSet = 0;

	unsigned int timeout = 0;

	//����� ����������� SPIFI
	SPIFI_StatResetSet();

	//����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

	//ADDRESS
    SPIFI_CONFIG->ADDR = PageAddress;

    //IDATA
    SPIFI_CONFIG->IDATA = 0x55;

    //CLIMIT
    SPIFI_CONFIG->CLIMIT = 0x00000000;

    //CMD
    RgCmd |= (W25Q_FAST_READ << SPIFI_CONFIG_CMD_OPCODE_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FRAMEFORM_OPCODE_3ADDR << SPIFI_CONFIG_CMD_FRAMEFORM_S);
    RgCmd |= (SPIFI_CONFIG_CMD_FIELDFORM_ALL_SERIAL << SPIFI_CONFIG_CMD_FIELDFORM_S);
    RgCmd |= (1 << SPIFI_CONFIG_CMD_INTLEN_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_DOUT_S);
    RgCmd |= (0 << SPIFI_CONFIG_CMD_POLL_S);
    RgCmd |= (W25Q_PAGE_SIZE << SPIFI_CONFIG_CMD_DATALEN_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_DONE_VALUE_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_INDEX_S);
    SPIFI_CONFIG->CMD = RgCmd;

    //�������� ��������� ���� STAT->INTRQ
    ResIntrqWaitSet = SPIFI_StatIntrqWaitSet();
    if(ResIntrqWaitSet==0)  {
    	//���� �������� ��������� ���� STAT:RESET
    	return 0;
    }

    //�������� ���������� ������
    //for(timeout=0; timeout<500000; timeout++)  {
    //	if((SPIFI_CONFIG->STAT & SPIFI_CONFIG_STAT_INTRQ_M) != 0)  {
    //		break;
    //	}
    //}

    //����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

	for(i=0; i<W25Q_PAGE_SIZE; i++)  {
		Page[i] = SPIFI_CONFIG->DATA8;
	}

    //return 1;
	return timeout;
}


/*
void SPIFI_W25Q_ResetEnable(void)  {
	unsigned int NBytes = 1;
	uint8_t Cmd[1] = {0};
	uint8_t Reply[1] = {0};

	Cmd[0] = W25Q_ENABLE_RESET;

	SPI_CS_Enable(W25Q_SPI, W25Q_SPI_CS);
	SPI_Exchange(W25Q_SPI, Cmd, Reply, NBytes);
	SPI_CS_Disable(W25Q_SPI, W25Q_SPI_CS);
}



void SPIFI_W25Q_Reset(void)  {
	unsigned int NBytes = 1;
	uint8_t Cmd[1] = {0};
	uint8_t Reply[1] = {0};

	Cmd[0] = W25Q_RESET_DEVICE;

	SPI_CS_Enable(W25Q_SPI, W25Q_SPI_CS);
	SPI_Exchange(W25Q_SPI, Cmd, Reply, NBytes);
	SPI_CS_Disable(W25Q_SPI, W25Q_SPI_CS);
}
*/


//�������� ��������� ���� STATUS->CMD
unsigned int SPIFI_WaitCmdSet(void)  {
	unsigned int i = 0;

    for(i=0; i<SPIFI_TIMEOUT; i++)  {
    	if((SPIFI_CONFIG->STAT & SPIFI_CONFIG_STAT_CMD_M) != 0)  {
    		return 1;
    	}
    }
    if(i==SPIFI_TIMEOUT)  {
    	return 0;
    }
}



//�������� ������ ���� STATUS->CMD
unsigned int SPIFI_WaitCmdClear(void)  {
	unsigned int i = 0;

    for(i=0; i<SPIFI_TIMEOUT; i++)  {
    	if((SPIFI_CONFIG->STAT & SPIFI_CONFIG_STAT_CMD_M) == 0)  {
    		return 1;
    	}
    }
    if(i==SPIFI_TIMEOUT)  {
    	return 0;
    }
}



//�������� ��������� ���� STATUS->INTRQ
unsigned int SPIFI_StatIntrqWaitSet(void)  {
	unsigned int i = 0;

    for(i=0; i<SPIFI_TIMEOUT; i++)  {
    	if((SPIFI_CONFIG->STAT & SPIFI_CONFIG_STAT_INTRQ_M) != 0)  {
    		return 1;
    	}
    }
    if(i==SPIFI_TIMEOUT)  {
    	return 0;
    }
}



//����� ���� STATUS->INTRQ
void SPIFI_StatIntrqClear(void)  {
	unsigned int i = 0;

	SPIFI_CONFIG->STAT |= SPIFI_CONFIG_STAT_INTRQ_M;
}



//��������� ���� STATUS->RESET
void SPIFI_StatResetSet(void)  {
	unsigned int i = 0;

	SPIFI_CONFIG->STAT |= SPIFI_CONFIG_STAT_RESET_M;
}



//�������� ��������� ���� STATUS->MCINIT
unsigned int SPIFI_StatMcinitWaitSet(void)  {
	unsigned int i = 0;

    for(i=0; i<SPIFI_TIMEOUT; i++)  {
    	if((SPIFI_CONFIG->STAT & SPIFI_CONFIG_STAT_MCINIT_M) != 0)  {
    		return 1;
    	}
    }
    if(i==SPIFI_TIMEOUT)  {
    	return 0;
    }
}



//������������� ������ ������ � �������
unsigned int SPIFI_W25Q_InitMemmode(void)  {
	//unsigned int Error = 0;
	unsigned int RgMcmd = 0;
	//unsigned int i = 0;
	unsigned int ResMcinitWaitSet = 0;

	//����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

	//ADDRESS
    SPIFI_CONFIG->ADDR = 0x00;

    //IDATA
    SPIFI_CONFIG->IDATA = 0x00;

    //CLIMIT
    SPIFI_CONFIG->CLIMIT = 0x00000000;

    //CMD
    RgMcmd |= (W25Q_READ_DATA << SPIFI_CONFIG_CMD_OPCODE_S);
    RgMcmd |= (SPIFI_CONFIG_CMD_FRAMEFORM_OPCODE_3ADDR << SPIFI_CONFIG_CMD_FRAMEFORM_S);
    RgMcmd |= (SPIFI_CONFIG_CMD_FIELDFORM_ALL_SERIAL << SPIFI_CONFIG_CMD_FIELDFORM_S);
    RgMcmd |= (0 << SPIFI_CONFIG_CMD_INTLEN_S);
    RgMcmd |= (0 << SPIFI_CONFIG_CMD_DOUT_S);
    RgMcmd |= (0 << SPIFI_CONFIG_CMD_POLL_S);
    	//RgMcmd |= (1 << SPIFI_CONFIG_CMD_DATALEN_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_DONE_VALUE_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_INDEX_S);
    SPIFI_CONFIG->MCMD = RgMcmd;

    //�������� ��������� ���� STAT->MCINIT
    ResMcinitWaitSet = SPIFI_StatMcinitWaitSet();
    if(ResMcinitWaitSet==0)  {
    	//���� �������� ��������� ���� STAT:RESET
    	return 0;
    }

    //����� ���� STAT:INTRQ
	//SPIFI_StatIntrqClear();

    return 1;
}



//������������� ������ ������ � �������
unsigned int SPIFI_W25Q_InitMemmodeQuad(void)  {
	//unsigned int Error = 0;
	unsigned int RgMcmd = 0;
	//unsigned int i = 0;
	unsigned int ResMcinitWaitSet = 0;

	//����� ���� STAT:INTRQ
	SPIFI_StatIntrqClear();

	//ADDRESS
    SPIFI_CONFIG->ADDR = 0x00;

    //IDATA
    SPIFI_CONFIG->IDATA = 0x00;

    //CLIMIT
    SPIFI_CONFIG->CLIMIT = 0x00000000;

    //���������� ��� CTRL:DUAL, ����� ������������ 4 �������
    SPIFI_CONFIG->CTRL &= ~(1<<SPIFI_CONFIG_CTRL_DUAL_S);

    //MCMD
    RgMcmd |= (W25Q_FAST_READ_QUAD_OUTPUT << SPIFI_CONFIG_CMD_OPCODE_S);
    RgMcmd |= (SPIFI_CONFIG_CMD_FRAMEFORM_OPCODE_3ADDR << SPIFI_CONFIG_CMD_FRAMEFORM_S);
    RgMcmd |= (SPIFI_CONFIG_CMD_FIELDFORM_DATA_PARALLEL << SPIFI_CONFIG_CMD_FIELDFORM_S);
    RgMcmd |= (1 << SPIFI_CONFIG_CMD_INTLEN_S);
    RgMcmd |= (0 << SPIFI_CONFIG_CMD_DOUT_S);
    RgMcmd |= (0 << SPIFI_CONFIG_CMD_POLL_S);
    RgMcmd |= (1 << SPIFI_CONFIG_CMD_DATALEN_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_DONE_VALUE_S);
    	//RgCmd |= (0 << SPIFI_CONFIG_CMD_DATALEN_BUSY_INDEX_S);
    SPIFI_CONFIG->MCMD = RgMcmd;

    //�������� ��������� ���� STAT->MCINIT
    ResMcinitWaitSet = SPIFI_StatMcinitWaitSet();
    if(ResMcinitWaitSet==0)  {
    	//���� �������� ��������� ���� STAT:RESET
    	return 0;
    }

    //����� ���� STAT:INTRQ
	//SPIFI_StatIntrqClear();

    return 1;
}





