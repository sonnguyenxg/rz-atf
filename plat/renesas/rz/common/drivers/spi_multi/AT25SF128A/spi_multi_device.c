/*
 * Copyright (c) 2020, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdint.h>
#include <lib/utils_def.h>
#include <lib/mmio.h>
#include <arch_helpers.h>
#include <spi_multi_regs.h>
#include <spi_multi.h>
#include <spi_multi_reg_values.h>
#include <common/debug.h>
#include <drivers/delay_timer.h>
#include <spi_multi.h>

void WaitRpcTxEnd(void)
{
	/* Wait until the transfer is complete */
	uint32_t val=0;
	do {
		val = mmio_read_32(SPIM_CMNSR);
	} while ((val & CMNSR_TEND) == 0);
}



uint32_t ReadStatusQspiFlash(uint32_t *readData, uint32_t res)	//for QSPIx1ch
{
	*((volatile uint32_t*)SPIM_PHYCNT)    = 0x00030260;
	*((volatile uint32_t*)SPIM_PHYCNT)    = 0x80030260;
	*((volatile uint32_t*)SPIM_CMNCR)      = 0x81FFF300;
		//bit31  MD       =  1 : Manual mode
		//bit1-0 BSZ[1:0] = 00 : QSPI Flash x 1
	*((volatile uint32_t*)SPIM_SMCMR)      = 0x00050000;
		//bit23-16 CMD[7:0] = 0x05 : Status Read command (for Palladium QSPI model)
	*((volatile uint32_t*)SPIM_SMDRENR)    = 0x00000000;
		//bit8 ADDRE  = 0 : Address SDR transfer
		//bit0 SPIDRE = 0 : DATA SDR transfer
	*((volatile uint32_t*)SPIM_SMENR)      = 0x0000400F;
		//bit31-30 CDB[1:0]   =   00 : 1bit width command (QSPI0_MOSI)
		//bit25-24 ADB[1:0]   =   00 : 1bit width address (QSPI0_MOSI)
		//bit17-16 SPIDB[1:0] =   00 : 1bit width transfer data (QSPI0_MISO)
		//bit15    DME        =    0 : No dummy cycle
		//bit14    CDE        =    1 : Command enable
		//bit11-8  ADE[3:0]   = 0000 : Address output disable
		//bit3-0   SPIDE[3:0] = 1111 : 32bit transfer
	*((volatile uint32_t*)SPIM_SMCR)       = res;
		//bit2     SPIRE      = 1 : Data read enable
		//bit1     SPIWE      = 0 : Data write disable
		//bit0     SPIE       = 1 : SPI transfer start

	WaitRpcTxEnd();

	readData[0] = *((volatile uint32_t*)SPIM_SMRDR0);	//read data[31:0]

	return(readData[0]);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Set_SPIM_SSL_Delay(void)
{
	uint32_t dataL=0;

	*((volatile uint32_t*)SPIM_PHYADJ2) = 0xA5390000;
	*((volatile uint32_t*)SPIM_PHYADJ1) = 0x80000000;
	*((volatile uint32_t*)SPIM_PHYADJ2) = 0x00008080;
	*((volatile uint32_t*)SPIM_PHYADJ1) = 0x80000022;
	*((volatile uint32_t*)SPIM_PHYADJ2) = 0x00008080;
	*((volatile uint32_t*)SPIM_PHYADJ1) = 0x80000024;

	dataL = *((volatile uint32_t*)SPIM_PHYCNT);
	*((volatile uint32_t*)SPIM_PHYCNT)  = (dataL | 0x00030000);
	*((volatile uint32_t*)SPIM_PHYADJ2) = 0x00000030;
	*((volatile uint32_t*)SPIM_PHYADJ1) = 0x80000032;


	*((volatile uint32_t*)SPIM_SSLDR) = 0x00000400;
		//bit10-8  SLNDL[2:0] =  100 : 5.5 cycles from QSPIn_SPCLK edge
}

void InitRPC_QspiFlash(uint32_t rpcclk)
{
	// PutStr("05 - QSPI - ID: .",1);
	// PowerOnRPC();

	// SetRPC_ClockMode(0x02);
	Set_SPIM_SSL_Delay();

	*((volatile uint32_t*)((0x10060000) + 0x0080U))= 0x31511144;		/* DDRTMG[1:0](Bit[29:28]) == B'11 */
	// PutStr("06 - QSPI - ID: .",1);


}

void spi_multi_setup_device( void )
{

	InitRPC_QspiFlash(0x01);

	uint32_t	readDevId = 0;
	ReadQspiFlashID(&readDevId);
	NOTICE("BL2: SPI AT25SF128A Device_ID = 0x%x\n", readDevId);

	uint32_t	StatusQspiFlash = 0;
	ReadStatusQspiFlash(&StatusQspiFlash, 0x00000005);
	NOTICE("BL2: SPI AT25SF128A Device Res 05: StatusQspiFlash = 0x%x\n", StatusQspiFlash);

	ReadStatusQspiFlash(&StatusQspiFlash, 0x00000035);
	NOTICE("BL2: SPI AT25SF128A Device Res 35: StatusQspiFlash = 0x%x\n", StatusQspiFlash);

	ReadStatusQspiFlash(&StatusQspiFlash, 0x00000015);
	NOTICE("BL2: SPI AT25SF128A Device Res 15: StatusQspiFlash = 0x%x\n", StatusQspiFlash);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////





// void spi_multi_setup_device( void )
// {
// 	NOTICE("BL2: SPI AT25SF128A Device Setup Start\n");
// 	uint32_t val;
// 	uint8_t  read_status;

// 	mmio_write_32(SPIM_PHYOFFSET1, SPIM_PHYOFFSET1_SET_VALUE);
// 	mmio_write_32(SPIM_PHYOFFSET2, SPIM_PHYOFFSET2_SET_VALUE);
// 	spi_multi_timing_set();
// 	NOTICE("BL2: SPI AT25SF128A Device Setup - 02\n");

// 	/*  Set Data read option */
// 	/* Required when command 0xEB is specified.
// 	 * Not required when a command other than is specified,
// 	 * but there is no problem in operation.
// 	 */
// 	val = SPIM_DROPR_SET_VALUE;
// 	mmio_write_32(SPIM_DROPR, val);

// 	uint32_t	readDevId = 0;
// 	ReadQspiFlashID(&readDevId);
// 	NOTICE("BL2: SPI AT25SF128A Device_ID = 0x%x\n", readDevId);


// 	uint32_t	StatusQspiFlash = 0;

// 	ReadStatusQspiFlash(&StatusQspiFlash, 0x00000005);
// 	NOTICE("BL2: SPI AT25SF128A Device Res 05: StatusQspiFlash = 0x%x\n", StatusQspiFlash);

// 	ReadStatusQspiFlash(&StatusQspiFlash, 0x00000035);
// 	NOTICE("BL2: SPI AT25SF128A Device Res 35: StatusQspiFlash = 0x%x\n", StatusQspiFlash);

// 	ReadStatusQspiFlash(&StatusQspiFlash, 0x00000015);
// 	NOTICE("BL2: SPI AT25SF128A Device Res 15: StatusQspiFlash = 0x%x\n", StatusQspiFlash);

// 	NOTICE("BL2: SPI AT25SF128A Device Setup - 03\n");

// 	read_status = spi_multi_cmd_read(SMCMR_CMD_READ_STATUS_REGISTER_2);
// 	if ((read_status & STATUS_2_QE) == STATUS_2_QE) {
// 		return;
// 	}
// 	NOTICE("BL2: SPI AT25SF128A Device Setup - 04\n");
// 	/* Write Enable Command */
// 	spi_multi_cmd_write(SMCMR_CMD_WRITE_ENABLE,SPI_MANUAL_COMMAND_SIZE_0,0);
// 	/* Write Status Register-2 Command Quad Enable */
// 	val = ((STATUS_2_QE | read_status) << SMWDR0_1BYTE_DATA_BIT_SHIFT);
// 	spi_multi_cmd_write(SMCMR_CMD_WRITE_STATUS_REGISTER_2,SPI_MANUAL_COMMAND_SIZE_8_BIT,val);

// 	NOTICE("BL2: SPI AT25SF128A Device Setup - 05\n");

// 	/* status 1 BUSY check */
// 	while(1) {
// 		read_status = spi_multi_cmd_read(SMCMR_CMD_READ_STATUS_REGISTER_1);
// 		if (( read_status & STATUS_1_BUSY_BIT) == STATUS_1_BUSY) {
// 			udelay(STATUS_BUSY_READ_DELAY_TIME);
// 			continue;
// 		} else {
// 			break;
// 		}
// 	}
// 	NOTICE("BL2: SPI AT25SF128A Device Setup End\n");
// 	return;
// }
