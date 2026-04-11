/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2011-2012 Freescale Semiconductor, Inc.
 */

/*
 * T4240QDS — T4240RDB-shaped baseline (SoC, NOR map, DUART, DPAA, PCI).
 * QDS-specific pieces: QIXIS (IFC + dynamic sysclk), six-DIMM SPD map,
 * MDIO mux / FMan fixups in board code. Alternate boot (NAND/SD/eMMC) stays
 * in this header for later defconfigs; the main NOR image matches RDB Kconfig.
 */

/*
 * T4240 QDS board configuration file
 */
#ifndef __CONFIG_H
#define __CONFIG_H

#include <linux/stringify.h>

#define CFG_ICS307_REFCLK_HZ		25000000  /* ICS307 ref clk freq */

#ifdef CONFIG_RAMBOOT_PBL
#define CONFIG_SYS_FSL_PBL_PBI board/freescale/t4qds/t4_pbi.cfg
#if !defined(CONFIG_MTD_RAW_NAND) && !defined(CONFIG_SDCARD)
#define CONFIG_RAMBOOT_TEXT_BASE        CONFIG_TEXT_BASE
#define CFG_RESET_VECTOR_ADDRESS	0xfffffffc
#else
#define CONFIG_SPL_FLUSH_IMAGE
#define CONFIG_SPL_PAD_TO		0x40000
#define CONFIG_SPL_MAX_SIZE		0x28000
#define RESET_VECTOR_OFFSET		0x27FFC
#define BOOT_PAGE_OFFSET		0x27000

#ifdef	CONFIG_MTD_RAW_NAND
#define CFG_SYS_NAND_U_BOOT_SIZE	(768 << 10)
#define CFG_SYS_NAND_U_BOOT_DST	0x00200000
#define CFG_SYS_NAND_U_BOOT_START	0x00200000
#define CFG_SYS_NAND_U_BOOT_OFFS	(256 << 10)
#define CONFIG_SYS_FSL_PBL_RCW board/freescale/t4qds/t4_nand_rcw.cfg
#endif

#ifdef	CONFIG_SDCARD
#define CFG_RESET_VECTOR_ADDRESS	0x200FFC
#define CFG_SYS_MMC_U_BOOT_SIZE	(768 << 10)
#define CFG_SYS_MMC_U_BOOT_DST	0x00200000
#define CFG_SYS_MMC_U_BOOT_START	0x00200000
#define CFG_SYS_MMC_U_BOOT_OFFS	(260 << 10)
#ifndef CONFIG_SPL_BUILD
#define	CONFIG_SYS_MPC85XX_NO_RESETVEC
#endif
#define CONFIG_SYS_FSL_PBL_RCW board/freescale/t4qds/t4_sd_rcw.cfg
#endif

#ifdef CONFIG_SPL_BUILD
#define CONFIG_SPL_SKIP_RELOCATE
#define CONFIG_SPL_COMMON_INIT_DDR
#ifndef CONFIG_SPL_SYS_CCSR_DO_NOT_RELOCATE
#define CONFIG_SPL_SYS_CCSR_DO_NOT_RELOCATE	1
#endif
#endif

#endif
#endif /* CONFIG_RAMBOOT_PBL */

#ifdef CONFIG_SRIO_PCIE_BOOT_SLAVE
#define CFG_SYS_SRIO_PCIE_BOOT_SLAVE_ADDR (CONFIG_TEXT_BASE & 0xfff00000)
#define CFG_SYS_SRIO_PCIE_BOOT_SLAVE_ADDR_PHYS \
		(0x300000000ull | CFG_SYS_SRIO_PCIE_BOOT_SLAVE_ADDR)
#define CFG_RESET_VECTOR_ADDRESS	0xfffffffc
#endif

#define CONFIG_SRIO_PCIE_BOOT_MASTER

#ifndef CFG_RESET_VECTOR_ADDRESS
#define CFG_RESET_VECTOR_ADDRESS	0xeffffffc
#endif

#define CFG_SYS_NUM_CPC		CONFIG_SYS_NUM_DDR_CTLRS

#define CFG_SYS_INIT_L3_ADDR		0xFFFC0000
#define SPL_ENV_ADDR			(CONFIG_SPL_GD_ADDR + 4 * 1024)

#define CFG_SYS_DCSRBAR		0xf0000000
#define CFG_SYS_DCSRBAR_PHYS		0xf00000000ull

#define CFG_SYS_DDR_SDRAM_BASE	0x00000000
#define CFG_SYS_SDRAM_BASE		CFG_SYS_DDR_SDRAM_BASE

#define CFG_SYS_FLASH_BASE	0xe0000000
#define CFG_SYS_FLASH_BASE_PHYS	(0xf00000000ull | CFG_SYS_FLASH_BASE)

#define CFG_SYS_INIT_RAM_ADDR	0xfdd00000
#define CFG_SYS_INIT_RAM_ADDR_PHYS_HIGH	0xf
#define CFG_SYS_INIT_RAM_ADDR_PHYS_LOW	0xfe03c000
#define CFG_SYS_INIT_RAM_ADDR_PHYS \
	((CFG_SYS_INIT_RAM_ADDR_PHYS_HIGH * 1ull << 32) | \
	  CFG_SYS_INIT_RAM_ADDR_PHYS_LOW)
#define CFG_SYS_INIT_RAM_SIZE		0x00004000

#define CFG_SYS_INIT_SP_OFFSET	(CFG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)

#if !CONFIG_IS_ENABLED(DM_SERIAL)
#define CFG_SYS_NS16550_CLK		(get_bus_freq(0)/2)
#endif

#define CFG_SYS_BAUDRATE_TABLE	\
	{300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200}

#define CFG_SYS_NS16550_COM1	(CFG_SYS_CCSRBAR+0x11C500)
#define CFG_SYS_NS16550_COM2	(CFG_SYS_CCSRBAR+0x11C600)
#define CFG_SYS_NS16550_COM3	(CFG_SYS_CCSRBAR+0x11D500)
#define CFG_SYS_NS16550_COM4	(CFG_SYS_CCSRBAR+0x11D600)

#define CFG_SYS_PCIE1_MEM_VIRT	0x80000000
#define CFG_SYS_PCIE1_MEM_PHYS	0xc00000000ull
#define CFG_SYS_PCIE1_IO_VIRT	0xf8000000
#define CFG_SYS_PCIE1_IO_PHYS	0xff8000000ull

#define CFG_SYS_PCIE2_MEM_VIRT	0xa0000000
#define CFG_SYS_PCIE2_MEM_PHYS	0xc20000000ull
#define CFG_SYS_PCIE2_IO_VIRT	0xf8010000
#define CFG_SYS_PCIE2_IO_PHYS	0xff8010000ull

#define CFG_SYS_PCIE3_MEM_VIRT	0xc0000000
#define CFG_SYS_PCIE3_MEM_PHYS	0xc40000000ull
#define CFG_SYS_PCIE3_ADDR	(CONFIG_SYS_IMMR + CFG_SYS_MPC85xx_PCIE3_OFFSET)
#define CFG_SYS_PCIE4_MEM_BUS	0xe0000000
#define CFG_SYS_PCIE4_MEM_PHYS	0xc60000000ull
#define CFG_SYS_PCIE4_ADDR	(CONFIG_SYS_IMMR + CFG_SYS_MPC85xx_PCIE4_OFFSET)

#define CFG_SYS_BOOTMAPSZ	(64 << 20)

#if defined(CONFIG_SPIFLASH)
#elif defined(CONFIG_SDCARD)
#define CFG_SYS_MMC_ENV_DEV		0
#endif

/* EEPROM: set CONFIG_SYS_I2C_EEPROM_ADDR in defconfig when ID_EEPROM=y */
#define CONFIG_SYS_EEPROM_BUS_NUM	0
#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN	1

/*
 * DDR — T4240QDS (vs T4240RDB)
 *
 * Six independent DIMM sockets: two per DDR controller (DDR1..DDR3), i.e.
 * three controllers × two slots. Timing and geometry are read from SPD EEPROM
 * on each populated slot at boot (fsl_ddr_sdram); total DRAM size is not fixed.
 *
 * SPD I2C (QorIQ T4240 QDS Reference Manual, primary mux CH0):
 *   0x50 — RCW + PBL AT24C64A; 0x57 — System ID EEPROM; 0x68 — DS3232 RTC.
 *   SPD (Linux dts: atmel,24c256 — DM I2C needs CONFIG_SPD_EEPROM_ALEN=2):
 *   DDR1 DIMM1 0x51, DIMM2 0x52; DDR2 DIMM1 0x53, DIMM2 0x54; DDR3 DIMM1 0x55,
 *   DIMM2 0x56 — matches spd_i2c_addr[][] in
 *   drivers/ddr/fsl/main.c for CONFIG_SYS_NUM_DDR_CTLRS=3 and
 *   CONFIG_DIMM_SLOTS_PER_CTLR=2.
 *
 * Interleaving (default env hwconfig, T4240 / CONFIG_ARCH_T4240):
 *   ctlr_intlv=3way_4KB — hash DRAM across the three controllers on 4 KiB
 *     boundaries (see doc/README.fsl-ddr).
 *   bank_intlv=auto — chip-select / rank interleave within each controller is
 *     chosen from the SPD-derived geometry (variable DIMM types and fill).
 *
 * Kconfig must set CONFIG_DIMM_SLOTS_PER_CTLR=2 for this board so the SPD map
 * above is used; default 1 would only probe 0x51–0x53 (wrong for QDS).
 */
#define CONFIG_SYS_SPD_BUS_NUM	0
#define SPD_EEPROM_ADDRESS1	0x51
#define SPD_EEPROM_ADDRESS2	0x52
#define SPD_EEPROM_ADDRESS3	0x53
#define SPD_EEPROM_ADDRESS4	0x54
#define SPD_EEPROM_ADDRESS5	0x55
#define SPD_EEPROM_ADDRESS6	0x56
#define SPD_EEPROM_ADDRESS	SPD_EEPROM_ADDRESS1	/* legacy single-slot boards */
/* Upper bound (MiB) for tools that still expect a compile-time cap; runtime RAM
 * comes from SPD + training in dram_init().
 */
#define CONFIG_SYS_SDRAM_SIZE	4096

/*
 * IFC — NOR flash (default boot, same window as T4240RDB)
 *
 * Virtual:  CFG_SYS_FLASH_BASE 0xe0000000 (256 MiB window to IFC NOR)
 * Physical: CFG_SYS_FLASH_BASE_PHYS 0xf_e000_0000
 *
 * Two 128 MiB devices (IFC_AMASK 128 MiB each):
 *   NOR0 CSPR -> phys base + 0x0800_0000 (upper half of map)
 *   NOR1 CSPR -> phys base (lower half)
 * CFG_SYS_FLASH_BANKS_LIST order matches CFI: { upper phys, lower phys }.
 *
 * Typical NOR image placement (must match what you program with codewarrior/
 * QCVS or known-good images; aligns with T4240RDB_defconfig):
 *   RCW / PBL region      0xEC000000  (effective addr in this NOR map; used when
 *                         programming RCW into NOR — not U-Boot TEXT_BASE)
 *                         (0xE8000000 + 0x04000000: upper 128 MiB bank, +64 MiB)
 *   CONFIG_TEXT_BASE     0xEFF40000  (U-Boot link / run address in flash window)
 *   CONFIG_ENV_ADDR      0xEFF20000  (env in flash, CONFIG_ENV_IS_IN_FLASH)
 *   FMan firmware blob   0xEFF00000  (CONFIG_SYS_FMAN_FW_ADDR when in NOR)
 *
 * QIXIS (FPGA) is IFC CS3 at QIXIS_BASE 0xffdf0000, not NOR.
 */
#define CFG_SYS_NOR0_CSPR_EXT	(0xf)
#define CFG_SYS_NOR0_CSPR	(CSPR_PHYS_ADDR(CFG_SYS_FLASH_BASE_PHYS \
				+ 0x8000000) | \
				CSPR_PORT_SIZE_16 | \
				CSPR_MSEL_NOR | \
				CSPR_V)
#define CFG_SYS_NOR1_CSPR_EXT	(0xf)
#define CFG_SYS_NOR1_CSPR	(CSPR_PHYS_ADDR(CFG_SYS_FLASH_BASE_PHYS) | \
				CSPR_PORT_SIZE_16 | \
				CSPR_MSEL_NOR | \
				CSPR_V)
#define CFG_SYS_NOR_AMASK	IFC_AMASK(128*1024*1024)
/* NOR Flash Timing Params */
#define CFG_SYS_NOR_CSOR	CSOR_NAND_TRHZ_80

#define CFG_SYS_NOR_FTIM0	(FTIM0_NOR_TACSE(0x4) | \
				FTIM0_NOR_TEADC(0x5) | \
				FTIM0_NOR_TEAHC(0x5))
#define CFG_SYS_NOR_FTIM1	(FTIM1_NOR_TACO(0x35) | \
				FTIM1_NOR_TRAD_NOR(0x1A) |\
				FTIM1_NOR_TSEQRAD_NOR(0x13))
#define CFG_SYS_NOR_FTIM2	(FTIM2_NOR_TCS(0x4) | \
				FTIM2_NOR_TCH(0x4) | \
				FTIM2_NOR_TWPH(0x0E) | \
				FTIM2_NOR_TWP(0x1c))
#define CFG_SYS_NOR_FTIM3	0x0

#define CONFIG_FLASH_SHOW_PROGRESS	45 /* count down from 45/5: 9..1 */

#define CFG_SYS_FLASH_BANKS_LIST	{CFG_SYS_FLASH_BASE_PHYS \
					+ 0x8000000, CFG_SYS_FLASH_BASE_PHYS}

/* QIXIS: enable via Kconfig (CONFIG_FSL_QIXIS) — board target selects it. */
#define QIXIS_BASE			0xffdf0000
#define QIXIS_LBMAP_SWITCH		6
#define QIXIS_LBMAP_MASK		0x0f
#define QIXIS_LBMAP_SHIFT		0
#define QIXIS_LBMAP_DFLTBANK		0x00
#define QIXIS_LBMAP_ALTBANK		0x04
#define QIXIS_RST_CTL_RESET		0x83
#define QIXIS_RST_FORCE_MEM		0x1
#define QIXIS_RCFG_CTL_RECONFIG_IDLE	0x20
#define QIXIS_RCFG_CTL_RECONFIG_START	0x21
#define QIXIS_RCFG_CTL_WATCHDOG_ENBLE	0x08
#define QIXIS_BRDCFG5			0x55
#define QIXIS_MUX_SDHC			2
#define QIXIS_MUX_SDHC_WIDTH8		1
#define QIXIS_BASE_PHYS		(0xf00000000ull | QIXIS_BASE)

#define CFG_SYS_CSPR3_EXT	(0xf)
#define CFG_SYS_CSPR3	(CSPR_PHYS_ADDR(QIXIS_BASE_PHYS) \
				| CSPR_PORT_SIZE_8 \
				| CSPR_MSEL_GPCM \
				| CSPR_V)
#define CFG_SYS_AMASK3	IFC_AMASK(64 * 1024)
#define CFG_SYS_CSOR3	0x0
/* QIXIS Timing parameters for IFC CS3 */
#define CFG_SYS_CS3_FTIM0		(FTIM0_GPCM_TACSE(0x0e) | \
					FTIM0_GPCM_TEADC(0x0e) | \
					FTIM0_GPCM_TEAHC(0x0e))
#define CFG_SYS_CS3_FTIM1		(FTIM1_GPCM_TACO(0xff) | \
					FTIM1_GPCM_TRAD(0x3f))
#define CFG_SYS_CS3_FTIM2		(FTIM2_GPCM_TCS(0x0e) | \
					FTIM2_GPCM_TCH(0x8) | \
					FTIM2_GPCM_TWP(0x1f))
#define CFG_SYS_CS3_FTIM3		0x0

/* NAND Flash on IFC */
#define CFG_SYS_NAND_BASE		0xff800000
#define CFG_SYS_NAND_BASE_PHYS	(0xf00000000ull | CFG_SYS_NAND_BASE)

#define CFG_SYS_NAND_CSPR_EXT	(0xf)
#define CFG_SYS_NAND_CSPR	(CSPR_PHYS_ADDR(CFG_SYS_NAND_BASE_PHYS) \
				| CSPR_PORT_SIZE_8 /* Port Size = 8 bit */ \
				| CSPR_MSEL_NAND	/* MSEL = NAND */ \
				| CSPR_V)
#define CFG_SYS_NAND_AMASK	IFC_AMASK(64*1024)

#define CFG_SYS_NAND_CSOR    (CSOR_NAND_ECC_ENC_EN   /* ECC on encode */ \
				| CSOR_NAND_ECC_DEC_EN  /* ECC on decode */ \
				| CSOR_NAND_ECC_MODE_4  /* 4-bit ECC */ \
				| CSOR_NAND_RAL_3	/* RAL = 2Byes */ \
				| CSOR_NAND_PGS_2K	/* Page Size = 2K */ \
				| CSOR_NAND_SPRZ_64/* Spare size = 64 */ \
				| CSOR_NAND_PB(64))	/*Pages Per Block = 64*/

#define CONFIG_SYS_NAND_ONFI_DETECTION

/* ONFI NAND Flash mode0 Timing Params */
#define CFG_SYS_NAND_FTIM0		(FTIM0_NAND_TCCST(0x07) | \
					FTIM0_NAND_TWP(0x18)   | \
					FTIM0_NAND_TWCHT(0x07) | \
					FTIM0_NAND_TWH(0x0a))
#define CFG_SYS_NAND_FTIM1		(FTIM1_NAND_TADLE(0x32) | \
					FTIM1_NAND_TWBE(0x39)  | \
					FTIM1_NAND_TRR(0x0e)   | \
					FTIM1_NAND_TRP(0x18))
#define CFG_SYS_NAND_FTIM2		(FTIM2_NAND_TRAD(0x0f) | \
					FTIM2_NAND_TREH(0x0a) | \
					FTIM2_NAND_TWHRE(0x1e))
#define CFG_SYS_NAND_FTIM3		0x0

#define CFG_SYS_NAND_DDR_LAW		11

#define CFG_SYS_NAND_BASE_LIST	{ CFG_SYS_NAND_BASE }
#define CFG_SYS_MAX_NAND_DEVICE	1

#define CFG_SYS_NAND_BLOCK_SIZE	(128 * 1024)
#define CONFIG_SYS_MAX_NAND_DEVICE	CFG_SYS_MAX_NAND_DEVICE
#define CONFIG_SYS_NAND_BLOCK_SIZE	CFG_SYS_NAND_BLOCK_SIZE
#define CFG_SYS_NAND_MAX_OOBFREE	2
#define CFG_SYS_NAND_MAX_ECCPOS	256

#if defined(CONFIG_MTD_RAW_NAND)
#define CFG_SYS_CSPR0_EXT		CFG_SYS_NAND_CSPR_EXT
#define CFG_SYS_CSPR0		CFG_SYS_NAND_CSPR
#define CFG_SYS_AMASK0		CFG_SYS_NAND_AMASK
#define CFG_SYS_CSOR0		CFG_SYS_NAND_CSOR
#define CFG_SYS_CS0_FTIM0		CFG_SYS_NAND_FTIM0
#define CFG_SYS_CS0_FTIM1		CFG_SYS_NAND_FTIM1
#define CFG_SYS_CS0_FTIM2		CFG_SYS_NAND_FTIM2
#define CFG_SYS_CS0_FTIM3		CFG_SYS_NAND_FTIM3
#define CFG_SYS_CSPR1_EXT		CFG_SYS_NOR0_CSPR_EXT
#define CFG_SYS_CSPR1		CFG_SYS_NOR0_CSPR
#define CFG_SYS_AMASK1		CFG_SYS_NOR_AMASK
#define CFG_SYS_CSOR1		CFG_SYS_NOR_CSOR
#define CFG_SYS_CS1_FTIM0		CFG_SYS_NOR_FTIM0
#define CFG_SYS_CS1_FTIM1		CFG_SYS_NOR_FTIM1
#define CFG_SYS_CS1_FTIM2		CFG_SYS_NOR_FTIM2
#define CFG_SYS_CS1_FTIM3		CFG_SYS_NOR_FTIM3
#define CFG_SYS_CSPR2_EXT		CFG_SYS_NOR1_CSPR_EXT
#define CFG_SYS_CSPR2		CFG_SYS_NOR1_CSPR
#define CFG_SYS_AMASK2		CFG_SYS_NOR_AMASK
#define CFG_SYS_CSOR2		CFG_SYS_NOR_CSOR
#define CFG_SYS_CS2_FTIM0		CFG_SYS_NOR_FTIM0
#define CFG_SYS_CS2_FTIM1		CFG_SYS_NOR_FTIM1
#define CFG_SYS_CS2_FTIM2		CFG_SYS_NOR_FTIM2
#define CFG_SYS_CS2_FTIM3		CFG_SYS_NOR_FTIM3
#else
#define CFG_SYS_CSPR0_EXT		CFG_SYS_NOR0_CSPR_EXT
#define CFG_SYS_CSPR0		CFG_SYS_NOR0_CSPR
#define CFG_SYS_AMASK0		CFG_SYS_NOR_AMASK
#define CFG_SYS_CSOR0		CFG_SYS_NOR_CSOR
#define CFG_SYS_CS0_FTIM0		CFG_SYS_NOR_FTIM0
#define CFG_SYS_CS0_FTIM1		CFG_SYS_NOR_FTIM1
#define CFG_SYS_CS0_FTIM2		CFG_SYS_NOR_FTIM2
#define CFG_SYS_CS0_FTIM3		CFG_SYS_NOR_FTIM3
#define CFG_SYS_CSPR1_EXT		CFG_SYS_NOR1_CSPR_EXT
#define CFG_SYS_CSPR1		CFG_SYS_NOR1_CSPR
#define CFG_SYS_AMASK1		CFG_SYS_NOR_AMASK
#define CFG_SYS_CSOR1		CFG_SYS_NOR_CSOR
#define CFG_SYS_CS1_FTIM0		CFG_SYS_NOR_FTIM0
#define CFG_SYS_CS1_FTIM1		CFG_SYS_NOR_FTIM1
#define CFG_SYS_CS1_FTIM2		CFG_SYS_NOR_FTIM2
#define CFG_SYS_CS1_FTIM3		CFG_SYS_NOR_FTIM3
#define CFG_SYS_CSPR2_EXT		CFG_SYS_NAND_CSPR_EXT
#define CFG_SYS_CSPR2		CFG_SYS_NAND_CSPR
#define CFG_SYS_AMASK2		CFG_SYS_NAND_AMASK
#define CFG_SYS_CSOR2		CFG_SYS_NAND_CSOR
#define CFG_SYS_CS2_FTIM0		CFG_SYS_NAND_FTIM0
#define CFG_SYS_CS2_FTIM1		CFG_SYS_NAND_FTIM1
#define CFG_SYS_CS2_FTIM2		CFG_SYS_NAND_FTIM2
#define CFG_SYS_CS2_FTIM3		CFG_SYS_NAND_FTIM3
#endif

#if defined(CONFIG_RAMBOOT_PBL)
#define CONFIG_SYS_RAMBOOT
#endif

/* I2C */
#ifndef CONFIG_DM_I2C
#define CONFIG_SYS_I2C
#else
#undef CONFIG_SYS_I2C
#undef CONFIG_SYS_FSL_I2C2_OFFSET
#undef CONFIG_SYS_FSL_I2C2_SLAVE
#undef CONFIG_SYS_FSL_I2C2_SPEED
#undef CONFIG_SYS_FSL_I2C_SLAVE
#undef CONFIG_SYS_FSL_I2C_SPEED
#undef CONFIG_SYS_FSL_I2C_OFFSET
#endif

#define CONFIG_SYS_FSL_I2C_SPEED	100000	/* I2C speed */
#define CONFIG_SYS_FSL_I2C2_SPEED	100000	/* I2C2 speed */
/*
 * @0x77: RM calls this "PCA9557"; Linux DT and U-Boot use nxp,pca9547 and the
 * PCA9547 control byte (channel | 0x8) in drivers/i2c/muxes/pca954x.c — CH0
 * for SPD/RCW/RTC is 0x8 (channel 0). CH1 (RM: SYSCLK/DDRCLK + SerDes synths
 * @0x60, 0x6C–0x6F) uses other select values below.
 */
#define I2C_MUX_PCA_ADDR_PRI		0x77 /* I2C bus multiplexer, primary */
#define I2C_MUX_PCA_ADDR_SEC		0x76 /* I2C bus multiplexer, secondary */

#define I2C_MUX_CH_DEFAULT	0x8	/* CH0: SPD, RCW@0x50, SystemID@0x57, RTC@0x68 */
#define I2C_MUX_CH_VOL_MONITOR	0xa
#define I2C_MUX_CH_TEMP_REMOTE	0xb	/* CH3 (3|0x8): ADT7481A @0x4c on primary mux */
#define I2C_MUX_CH_VSC3316_FS	0xc
#define I2C_MUX_CH_VSC3316_BS	0xd

/*
 * DT aliases (t4240qds.dts): i2c0 = SoC @0x118000 (SPD + PCA9547); i2c1 = @0x118100.
 * CONFIG_SYS_SPD_BUS_NUM is 0 (i2c0). Devices behind the PCA9547 use the channel
 * byte (channel | 0x8), e.g. ADT7481A @0x4c on mux channel 3 => I2C_MUX_CH_TEMP_REMOTE.
 *
 * Debug-only: RCW@0x50 / NXID@0x57 on CH0; temp sanity selects CH3 then restores CH0.
 */
#define T4240QDS_DBG_SANITY_TEMP_ADDR	0x4c
#define T4240QDS_RCW_EEPROM_ADDR	0x50

/* Voltage monitor on channel 2*/
#define I2C_VOL_MONITOR_ADDR		0x40
#define I2C_VOL_MONITOR_BUS_V_OFFSET	0x2
#define I2C_VOL_MONITOR_BUS_V_OVF	0x1
#define I2C_VOL_MONITOR_BUS_V_SHIFT	3
#define VDD_MV_MIN			819
#define VDD_MV_MAX			1212
/* VSC Crossbar switches */
#define VSC3316_FSM_TX_ADDR	0x70
#define VSC3316_FSM_RX_ADDR	0x71

/*
 * RapidIO
 */

/*
 * for slave u-boot IMAGE instored in master memory space,
 * PHYS must be aligned based on the SIZE
 */
#define CONFIG_SRIO_PCIE_BOOT_IMAGE_MEM_PHYS 0xfef200000ull
#define CONFIG_SRIO_PCIE_BOOT_IMAGE_MEM_BUS1 0xfff00000ull
#define CONFIG_SRIO_PCIE_BOOT_IMAGE_SIZE 0x100000	/* 1M */
#define CONFIG_SRIO_PCIE_BOOT_IMAGE_MEM_BUS2 0x3fff00000ull
/*
 * for slave UCODE and ENV instored in master memory space,
 * PHYS must be aligned based on the SIZE
 */
#define CONFIG_SRIO_PCIE_BOOT_UCODE_ENV_MEM_PHYS 0xfef100000ull
#define CONFIG_SRIO_PCIE_BOOT_UCODE_ENV_MEM_BUS 0x3ffe00000ull
#define CONFIG_SRIO_PCIE_BOOT_UCODE_ENV_SIZE 0x40000	/* 256K */

/* slave core release by master*/
#define CONFIG_SRIO_PCIE_BOOT_BRR_OFFSET 0xe00e4
#define CONFIG_SRIO_PCIE_BOOT_RELEASE_MASK 0x00000001 /* release core 0 */

/*
 * SRIO_PCIE_BOOT - SLAVE
 */
#ifdef CONFIG_SRIO_PCIE_BOOT_SLAVE
#define CFG_SYS_SRIO_PCIE_BOOT_UCODE_ENV_ADDR 0xFFE00000
#define CFG_SYS_SRIO_PCIE_BOOT_UCODE_ENV_ADDR_PHYS \
		(0x300000000ull | CFG_SYS_SRIO_PCIE_BOOT_UCODE_ENV_ADDR)
#endif
/*
 * eSPI - Enhanced SPI
 */

/* Qman/Bman */
#ifndef CONFIG_NOBQFMAN
#define CFG_SYS_BMAN_NUM_PORTALS	50
#define CFG_SYS_BMAN_MEM_BASE	0xf4000000
#define CFG_SYS_BMAN_MEM_PHYS	0xff4000000ull
#define CFG_SYS_BMAN_MEM_SIZE	0x02000000
#define CFG_SYS_BMAN_SP_CENA_SIZE    0x4000
#define CFG_SYS_BMAN_SP_CINH_SIZE    0x1000
#define CFG_SYS_BMAN_CENA_BASE       CFG_SYS_BMAN_MEM_BASE
#define CFG_SYS_BMAN_CENA_SIZE       (CFG_SYS_BMAN_MEM_SIZE >> 1)
#define CFG_SYS_BMAN_CINH_BASE       (CFG_SYS_BMAN_MEM_BASE + \
					CFG_SYS_BMAN_CENA_SIZE)
#define CFG_SYS_BMAN_CINH_SIZE       (CFG_SYS_BMAN_MEM_SIZE >> 1)
#define CFG_SYS_BMAN_SWP_ISDR_REG    0xE08
#define CFG_SYS_QMAN_NUM_PORTALS	50
#define CFG_SYS_QMAN_MEM_BASE	0xf6000000
#define CFG_SYS_QMAN_MEM_PHYS	0xff6000000ull
#define CFG_SYS_QMAN_MEM_SIZE	0x02000000
#define CFG_SYS_QMAN_SP_CENA_SIZE    0x4000
#define CFG_SYS_QMAN_SP_CINH_SIZE    0x1000
#define CFG_SYS_QMAN_CENA_BASE       CFG_SYS_QMAN_MEM_BASE
#define CFG_SYS_QMAN_CENA_SIZE       (CFG_SYS_QMAN_MEM_SIZE >> 1)
#define CFG_SYS_QMAN_CINH_BASE       (CFG_SYS_QMAN_MEM_BASE + \
					CFG_SYS_QMAN_CENA_SIZE)
#define CFG_SYS_QMAN_CINH_SIZE       (CFG_SYS_QMAN_MEM_SIZE >> 1)
#define CFG_SYS_QMAN_SWP_ISDR_REG	0xE08

/* Default address of microcode for the Linux Fman driver */
/*
 * Kconfig may emit CONFIG_SYS_FMAN_FW_ADDR=0 when no explicit value is chosen.
 * Treat zero as "unset" and fall back to board boot-source defaults below.
 */
#if defined(CONFIG_SYS_FMAN_FW_ADDR) && (CONFIG_SYS_FMAN_FW_ADDR == 0)
#undef CONFIG_SYS_FMAN_FW_ADDR
#endif

#ifndef CONFIG_SYS_FMAN_FW_ADDR
#if defined(CONFIG_SPIFLASH)
/*
 * env is stored at 0x100000, sector size is 0x10000, ucode is stored after
 * env, so we got 0x110000.
 */
#define CONFIG_SYS_FMAN_FW_ADDR	0x110000
#elif defined(CONFIG_SDCARD)
/*
 * PBL SD boot image should stored at 0x1000(8 blocks), the size of the image is
 * about 1MB (2048 blocks), Env is stored after the image, and the env size is
 * 0x2000 (16 blocks), 8 + 2048 + 16 = 2072, enlarge it to 2080.
 */
#define CONFIG_SYS_FMAN_FW_ADDR	(512 * 0x820)
#elif defined(CONFIG_MTD_RAW_NAND)
#define CONFIG_SYS_FMAN_FW_ADDR	(11 * CFG_SYS_NAND_BLOCK_SIZE)
#elif defined(CONFIG_SRIO_PCIE_BOOT_SLAVE)
/*
 * Slave has no ucode locally, it can fetch this from remote. When implementing
 * in two corenet boards, slave's ucode could be stored in master's memory
 * space, the address can be mapped from slave TLB->slave LAW->
 * slave SRIO or PCIE outbound window->master inbound window->
 * master LAW->the ucode address in master's memory space.
 */
#define CONFIG_SYS_FMAN_FW_ADDR	0xFFE00000
#else
#define CONFIG_SYS_FMAN_FW_ADDR		0xEFF00000
#endif
#endif
#define CONFIG_SYS_QE_FMAN_FW_LENGTH	0x10000
#ifndef CONFIG_SYS_FDT_PAD
#define CONFIG_SYS_FDT_PAD		(0x3000 + CONFIG_SYS_QE_FMAN_FW_LENGTH)
#endif
#endif /* CONFIG_NOBQFMAN */

#ifdef CONFIG_SYS_DPAA_FMAN
#define SGMII_CARD_PORT1_PHY_ADDR 0x1C
#define SGMII_CARD_PORT2_PHY_ADDR 0x1D
#define SGMII_CARD_PORT3_PHY_ADDR 0x1E
#define SGMII_CARD_PORT4_PHY_ADDR 0x1F
#define FM1_10GEC1_PHY_ADDR	0x0
#define FM1_10GEC2_PHY_ADDR	0x1
#define FM2_10GEC1_PHY_ADDR	0x2
#define FM2_10GEC2_PHY_ADDR	0x3
#endif

/* SATA */
#ifdef CONFIG_FSL_SATA_V2
#define CONFIG_SATA1
#define CONFIG_SYS_SATA1		CONFIG_SYS_MPC85xx_SATA1_ADDR
#define CONFIG_SYS_SATA1_FLAGS		FLAGS_DMA
#define CONFIG_SATA2
#define CONFIG_SYS_SATA2		CONFIG_SYS_MPC85xx_SATA2_ADDR
#define CONFIG_SYS_SATA2_FLAGS		FLAGS_DMA
#endif

#ifdef CONFIG_MMC
#define CFG_SYS_FSL_ESDHC_ADDR		CFG_SYS_MPC85xx_ESDHC_ADDR
#define CONFIG_SYS_FSL_ESDHC_BROKEN_TIMEOUT
#define CONFIG_SYS_FSL_MMC_HAS_CAPBLT_VS33
#define CONFIG_ESDHC_DETECT_QUIRK \
	(!(readb(QIXIS_BASE + QIXIS_BRDCFG5) & QIXIS_MUX_SDHC) || \
	IS_SVR_REV(get_svr(), 1, 0))
#define CONFIG_ESDHC_DETECT_8_BIT_QUIRK \
	(!(readb(QIXIS_BASE + QIXIS_BRDCFG5) & QIXIS_MUX_SDHC_WIDTH8))
#endif


#define __USB_PHY_TYPE	utmi

/*
 * T4240: three DDR controllers; QDS uses 3-way controller interleaving.
 * Alternatives: 3way_1KB, 3way_4KB, 3way_8KB. T4160: two controllers use
 * cacheline/page/bank/superbank. See doc/README.fsl-ddr.
 */
#ifdef CONFIG_ARCH_T4240
#define CTRL_INTLV_PREFERED 3way_4KB
#else
#define CTRL_INTLV_PREFERED cacheline
#endif

#define	CONFIG_EXTRA_ENV_SETTINGS				\
	"hwconfig=fsl_ddr:"					\
	"ctlr_intlv=" __stringify(CTRL_INTLV_PREFERED) ","	\
	"bank_intlv=auto;"					\
	"usb1:dr_mode=host,phy_type=" __stringify(__USB_PHY_TYPE) "\0"\
	"netdev=eth0\0"						\
	"uboot=" __stringify(CONFIG_UBOOTPATH) "\0"			\
	"ubootaddr=" __stringify(CONFIG_TEXT_BASE) "\0"		\
	"tftpflash=tftpboot $loadaddr $uboot && "		\
	"protect off $ubootaddr +$filesize && "			\
	"erase $ubootaddr +$filesize && "			\
	"cp.b $loadaddr $ubootaddr $filesize && "		\
	"protect on $ubootaddr +$filesize && "			\
	"cmp.b $loadaddr $ubootaddr $filesize\0"		\
	"consoledev=ttyS0\0"					\
	"baudrate=115200\0"					\
	"bootcmd_main=" CONFIG_LINUX "\0"			\
	"bootcmd_recovery=echo *** Boot failed, staying at U-Boot console ***; " \
	"echo Use 'run bootcmd_main' or 'run netboot' after checks;\0"	\
	"ramdiskaddr=2000000\0"					\
	"ramdiskfile=t4240qds/ramdisk.uboot\0"			\
	"fdtaddr=1e00000\0"					\
	"fdtfile=t4240qds/t4240qds.dtb\0"				\
	"bdev=sda3\0"

#define CONFIG_HVBOOT				\
	"setenv bootargs config-addr=0x60000000; "	\
	"bootm 0x01000000 - 0x00f00000"

#define CONFIG_ALU				\
	"setenv bootargs root=/dev/$bdev rw "		\
	"console=$consoledev,$baudrate $othbootargs;"	\
	"cpu 1 release 0x01000000 - - -;"		\
	"cpu 2 release 0x01000000 - - -;"		\
	"cpu 3 release 0x01000000 - - -;"		\
	"cpu 4 release 0x01000000 - - -;"		\
	"cpu 5 release 0x01000000 - - -;"		\
	"cpu 6 release 0x01000000 - - -;"		\
	"cpu 7 release 0x01000000 - - -;"		\
	"go 0x01000000"

#define CONFIG_LINUX				\
	"setenv bootargs root=/dev/ram rw "		\
	"console=$consoledev,$baudrate $othbootargs;"	\
	"setenv ramdiskaddr 0x02000000;"		\
	"setenv fdtaddr 0x00c00000;"			\
	"setenv loadaddr 0x1000000;"			\
	"bootm $loadaddr $ramdiskaddr $fdtaddr"

#define CONFIG_HDBOOT					\
	"setenv bootargs root=/dev/$bdev rw "		\
	"console=$consoledev,$baudrate $othbootargs;"	\
	"tftp $loadaddr $bootfile;"			\
	"tftp $fdtaddr $fdtfile;"			\
	"bootm $loadaddr - $fdtaddr"

#define CONFIG_NFSBOOTCOMMAND			\
	"setenv bootargs root=/dev/nfs rw "	\
	"nfsroot=$serverip:$rootpath "		\
	"ip=$ipaddr:$serverip:$gatewayip:$netmask:$hostname:$netdev:off " \
	"console=$consoledev,$baudrate $othbootargs;"	\
	"tftp $loadaddr $bootfile;"		\
	"tftp $fdtaddr $fdtfile;"		\
	"bootm $loadaddr - $fdtaddr"

#define CONFIG_RAMBOOTCOMMAND				\
	"setenv bootargs root=/dev/ram rw "		\
	"console=$consoledev,$baudrate $othbootargs;"	\
	"tftp $ramdiskaddr $ramdiskfile;"		\
	"tftp $loadaddr $bootfile;"			\
	"tftp $fdtaddr $fdtfile;"			\
	"bootm $loadaddr $ramdiskaddr $fdtaddr"

#include <asm/fsl_secure_boot.h>

#endif	/* __CONFIG_H */
