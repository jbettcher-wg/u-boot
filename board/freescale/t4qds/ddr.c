// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright 2012 Freescale Semiconductor, Inc.
 */

#include <config.h>
#include <display_options.h>
#include <hang.h>
#include <i2c.h>
#include <hwconfig.h>
#include <init.h>
#include <log.h>
#include <linux/delay.h>
#include <string.h>
#if defined(CONFIG_DEBUG_FSL_DDR_SPD_I2C) && CONFIG_IS_ENABLED(DM_I2C)
#include <dm.h>
#include <dm/uclass-id.h>
#endif
#include <asm/mmu.h>
#include <asm/global_data.h>
#include <configs/T4240QDS.h>
#include <fsl_ddr_sdram.h>
#include <fsl_ddr_dimm_params.h>
#include <asm/fsl_law.h>
#include "../common/qixis.h"
#include "t4240qds_qixis.h"
#include "ddr.h"

int board_mux_pca9547(u8 ch, int bus_num);

DECLARE_GLOBAL_DATA_PTR;

#define DDRSPD_JEDEC_DDR3_SDRAM	0x0b

void fsl_ddr_board_options(memctl_options_t *popts,
				dimm_params_t *pdimm,
				unsigned int ctrl_num)
{
	const struct board_specific_parameters *pbsp, *pbsp_highest = NULL;
	ulong ddr_freq;

	if (ctrl_num > 2) {
		printf("Not supported controller number %d\n", ctrl_num);
		return;
	}
	if (!pdimm->n_ranks)
		return;

	/*
	 * QDS: six sockets (two per controller); one timing table for all.
	 * For per-controller tables use rdimms[ctrl_num] / udimms[ctrl_num].
	 */
	if (popts->registered_dimm_en)
		pbsp = rdimms[0];
	else
		pbsp = udimms[0];


	/* Get clk_adjust, cpo, write_data_delay,2T, according to the board ddr
	 * freqency and n_banks specified in board_specific_parameters table.
	 */
	ddr_freq = get_ddr_freq(0) / 1000000;
	while (pbsp->datarate_mhz_high) {
		if (pbsp->n_ranks == pdimm->n_ranks &&
		    (pdimm->rank_density >> 30) >= pbsp->rank_gb) {
			if (ddr_freq <= pbsp->datarate_mhz_high) {
				popts->cpo_override = pbsp->cpo;
				popts->write_data_delay =
					pbsp->write_data_delay;
				popts->clk_adjust = pbsp->clk_adjust;
				popts->wrlvl_start = pbsp->wrlvl_start;
				popts->wrlvl_ctl_2 = pbsp->wrlvl_ctl_2;
				popts->wrlvl_ctl_3 = pbsp->wrlvl_ctl_3;
				popts->twot_en = pbsp->force_2t;
				goto found;
			}
			pbsp_highest = pbsp;
		}
		pbsp++;
	}

	if (pbsp_highest) {
		printf("Error: board specific timing not found "
			"for data rate %lu MT/s\n"
			"Trying to use the highest speed (%u) parameters\n",
			ddr_freq, pbsp_highest->datarate_mhz_high);
		popts->cpo_override = pbsp_highest->cpo;
		popts->write_data_delay = pbsp_highest->write_data_delay;
		popts->clk_adjust = pbsp_highest->clk_adjust;
		popts->wrlvl_start = pbsp_highest->wrlvl_start;
		popts->wrlvl_ctl_2 = pbsp_highest->wrlvl_ctl_2;
		popts->wrlvl_ctl_3 = pbsp_highest->wrlvl_ctl_3;
		popts->twot_en = pbsp_highest->force_2t;
	} else {
		panic("DIMM is not supported by this board");
	}
found:
	debug("Found timing match: n_ranks %d, data rate %d, rank_gb %d\n"
		"\tclk_adjust %d, wrlvl_start %d, wrlvl_ctrl_2 0x%x, "
		"wrlvl_ctrl_3 0x%x\n",
		pbsp->n_ranks, pbsp->datarate_mhz_high, pbsp->rank_gb,
		pbsp->clk_adjust, pbsp->wrlvl_start, pbsp->wrlvl_ctl_2,
		pbsp->wrlvl_ctl_3);

	/*
	 * Factors to consider for half-strength driver enable:
	 *	- number of DIMMs installed
	 */
	popts->half_strength_driver_enable = 0;
	/*
	 * Write leveling override
	 */
	popts->wrlvl_override = 1;
	popts->wrlvl_sample = 0xf;

	/*
	 * Rtt and Rtt_WR override
	 */
	popts->rtt_override = 0;

	/* Enable ZQ calibration */
	popts->zq_en = 1;

	/* DHC_EN =1, ODT = 75 Ohm */
	popts->ddr_cdr1 = DDR_CDR1_DHC_EN | DDR_CDR1_ODT(DDR_CDR_ODT_75ohm);
	popts->ddr_cdr2 = DDR_CDR2_ODT(DDR_CDR_ODT_75ohm);

	/* optimize cpo for erratum A-009942 */
	popts->cpo_sample = 0x63;
}

/*
 * drivers/ddr/fsl/main.c calls this before each get_spd() for a slot.
 * Re-select PCA9547 CH0 so earlier init (voltage mux, VSC, etc.) cannot
 * leave the bus switched away from DIMM SPD EEPROMs.
 */
void update_spd_address(unsigned int ctrl_num, unsigned int slot,
			 unsigned int *addr)
{
	(void)ctrl_num;
	(void)slot;
	(void)addr;

	(void)board_mux_pca9547(I2C_MUX_CH_DEFAULT, CONFIG_SYS_SPD_BUS_NUM);
}

int dram_init(void)
{
	phys_size_t dram_size;

	printf("dram_init: starting...\n");

#if defined(CONFIG_SPL_BUILD) || !defined(CONFIG_RAMBOOT_PBL)
	/*
	 * BRDCFG5_IRE steers I2C1 to the FPGA "remote" path. It is cleared again
	 * in board_early_init_r(), but dram_init runs in init_f before that, and
	 * SPL never runs board_early_init_r — local SPD/mux need IRE off first.
	 */
	QIXIS_WRITE(brdcfg[5], QIXIS_READ(brdcfg[5]) & ~BRDCFG5_IRE);
	udelay(1000); /* Allow Qixis FPGA steering logic to settle */
	/*
	 * SPD EEPROMs sit behind PCA9547; ensure channel matches Linux DT
	 * (channel 0 => control byte 0x08 on PCA9547 per NXP mux encoding).
	 */
	(void)board_mux_pca9547(I2C_MUX_CH_DEFAULT, CONFIG_SYS_SPD_BUS_NUM);
	
	dram_size = fsl_ddr_sdram();
	if (!dram_size) {
		printf("DDR: no valid SPD — check I2C mux, DIMMs, and addresses\n");
		hang();
	}
#else
	printf("dram_init: bypassing SPD (RAMBOOT)...\n");
	/* DDR has been initialised by first stage boot loader */
	dram_size = fsl_ddr_sdram_size();
#endif

	printf("dram_init: setup tlbs...\n");
	dram_size = setup_ddr_tlbs(dram_size / 0x100000);
	dram_size *= 0x100000;

	gd->ram_size = dram_size;

	return 0;
}
