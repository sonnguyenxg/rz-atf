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

void spi_multi_setup_device( void )
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

	*((volatile uint32_t*)((0x10060000) + 0x0080U))= 0x31511144;		/* DDRTMG[1:0](Bit[29:28]) == B'11 */

}
