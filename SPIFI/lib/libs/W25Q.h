#ifndef W25Q_H_INCLUDED
#define W25Q_H_INCLUDED

#define MF_0	(0xEF)
#define ID_1	(0x40)
#define ID_0	(0x18)

#define W25Q_READ_DATA 	                0x03			//K1636RR compatible
#define W25Q_FAST_READ       			0x0B			//K1636RR compatible
#define W25Q_FAST_READ_DUAL_OUTPUT		0x3B
#define W25Q_FAST_READ_QUAD_OUTPUT		0x6B			
#define W25Q_SECTOR_ERASE_4K            0x20			//K1636RR compatible
#define W25Q_PAGE_PROGRAM               0x02			//K1636RR compatible
#define W25Q_CHIP_ERASE                 0xC7

#define W25Q_SECTOR_ERASE_64K            0xD8
#define W25Q_SECTOR_ERASE_4K             0x20			//K1636RR compatible
#define W25Q_BLOCK_ERASE_64K             0xD8
#define W25Q_WRITE_ENABLE                0x06			//K1636RR compatible
#define W25Q_WRITE_DISABLE               0x04			//K1636RR compatible
#define W25Q_BLOCK_LOCK           		0x36			//K1636RR compatible
#define W25Q_BLOCK_UNLOCK           		0x39			//K1636RR compatible
#define W25Q_READ_STATUS_REGISTER_1      0x05			//K1636RR compatible
#define W25Q_WRITE_STATUS_REGISTER_1     0x01			//K1636RR compatible
#define W25Q_READ_STATUS_REGISTER_2      0x35
#define W25Q_WRITE_STATUS_REGISTER_2     0x31
#define W25Q_READ_STATUS_REGISTER_3      0x15
#define W25Q_WRITE_STATUS_REGISTER_3     0x11
#define W25Q_ENABLE_RESET                0x66
#define W25Q_RESET_DEVICE                0x99
#define W25Q_READ_JEDEC_ID               0x9F

#define W25Q_STATUS_1_SPR0_S				7
#define W25Q_STATUS_1_SPR0_M             (1 << W25Q_STATUS_1_SPR0_S)
#define W25Q_STATUS_1_SEC_S             	6
#define W25Q_STATUS_1_SEC_M             	(1 << W25Q_STATUS_1_SEC_S)
#define W25Q_STATUS_1_TB_S              	5
#define W25Q_STATUS_1_TB_M              	(1 << W25Q_STATUS_1_TB_S)
#define W25Q_STATUS_1_BP2_S              4
#define W25Q_STATUS_1_BP2_M              (1 << W25Q_STATUS_1_BP2_S)
#define W25Q_STATUS_1_BP1_S              3
#define W25Q_STATUS_1_BP1_M              (1 << W25Q_STATUS_1_BP1_S)
#define W25Q_STATUS_1_BP0_S              2
#define W25Q_STATUS_1_BP0_M              (1 << W25Q_STATUS_1_BP0_S)
#define W25Q_STATUS_1_WEL_S              1
#define W25Q_STATUS_1_WEL_M              (1 << W25Q_STATUS_1_WEL_S)
#define W25Q_STATUS_1_BUSY_S             0
#define W25Q_STATUS_1_BUSY_M             (1 << W25Q_STATUS_1_BUSY_S)

#define W25Q_STATUS_2_SUS_S				7
#define W25Q_STATUS_2_SUS_M             	(1 << W25Q_STATUS_2_SUS_S)
#define W25Q_STATUS_2_CMP_S				6                    
#define W25Q_STATUS_2_CMP_M             	(1 << W25Q_STATUS_2_CMP_S)
#define W25Q_STATUS_2_LB3_S				5                    
#define W25Q_STATUS_2_LB3_M             	(1 << W25Q_STATUS_2_LB3_S)
#define W25Q_STATUS_2_LB2_S				4                    
#define W25Q_STATUS_2_LB2_M             	(1 << W25Q_STATUS_2_LB2_S)
#define W25Q_STATUS_2_LB1_S				3                    
#define W25Q_STATUS_2_LB1_M             	(1 << W25Q_STATUS_2_LB1_S)
#define W25Q_STATUS_2_QE_S				1                    
#define W25Q_STATUS_2_QE_M             	(1 << W25Q_STATUS_2_QE_S)
#define W25Q_STATUS_2_SPR1_S				0                    
#define W25Q_STATUS_2_SPR1_M             (1 << W25Q_STATUS_2_SPR1_S)

#define W25Q_STATUS_3_HOLD_RST_S			7
#define W25Q_STATUS_3_HOLD_RST_M         (1 << W25Q_STATUS_3_HOLD_RST_S)
#define W25Q_STATUS_3_DRV1_S				6                    
#define W25Q_STATUS_3_DRV1_M         	(1 << W25Q_STATUS_3_DRV1_S)
#define W25Q_STATUS_3_DRV0_S				5                    
#define W25Q_STATUS_3_DRV0_M         	(1 << W25Q_STATUS_3_DRV0_S)
#define W25Q_STATUS_3_WPS_S				2                    
#define W25Q_STATUS_3_WPS_M         		(1 << W25Q_STATUS_3_WPS_S)

#define W25Q_TIMEOUT			30000

#define W25Q_ADDRESS_SIZE                3

#define W25Q_PAGE_SIZE					(256)
#define W25Q_SECTOR_4K_SIZE				(4*1024)
#define W25Q_BLOCK_64K_SIZE				(64*1024)	//Block  0..255

#define W25Q_BLOCK_ADDRESS(BlockNum)		(BlockNum*W25Q_BLOCK_64K_SIZE)

#endif // W25Q_H_INCLUDED
