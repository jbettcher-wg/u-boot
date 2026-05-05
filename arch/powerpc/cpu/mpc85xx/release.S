/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2008-2012 Freescale Semiconductor, Inc.
 * Kumar Gala <kumar.gala@freescale.com>
 */

#include <asm-offsets.h>
#include <config.h>
#include <mpc85xx.h>

#include <ppc_asm.tmpl>
#include <ppc_defs.h>

#include <asm/cache.h>
#include <asm/mmu.h>

/* To boot secondary cpus, we need a place for them to start up.
 * Normally, they start at 0xfffffffc, but that's usually the
 * firmware, and we don't want to have to run the firmware again.
 * Instead, the primary cpu will set the BPTR to point here to
 * this page.  We then set up the core, and head to
 * start_secondary.  Note that this means that the code below
 * must never exceed 1023 instructions (the branch at the end
 * would then be the 1024th).
 */
	.globl	__secondary_start_page
	.align	12
__secondary_start_page:
#ifdef CONFIG_SYS_FSL_ERRATUM_A005125
	msync
	isync
	mfspr	r3, SPRN_HDBCR0
	oris	r3, r3, 0x0080
	mtspr	SPRN_HDBCR0, r3
#endif
/* First do some preliminary setup */
	lis	r3, HID0_EMCP@h		/* enable machine check */
#ifndef CONFIG_E500MC
	ori	r3,r3,HID0_TBEN@l	/* enable Timebase */
#endif
#ifdef CONFIG_PHYS_64BIT
	ori	r3,r3,HID0_ENMAS7@l	/* enable MAS7 updates */
#endif
	mtspr	SPRN_HID0,r3

#ifndef CONFIG_E500MC
	li	r3,(HID1_ASTME|HID1_ABE)@l	/* Addr streaming & broadcast */
	mfspr   r0,PVR
	andi.	r0,r0,0xff
	cmpwi	r0,0x50@l	/* if we are rev 5.0 or greater set MBDD */
	blt 1f
	/* Set MBDD bit also */
	ori r3, r3, HID1_MBDD@l
1:
	mtspr	SPRN_HID1,r3
#endif

#ifdef CONFIG_SYS_FSL_ERRATUM_CPU_A003999
	mfspr	r3,SPRN_HDBCR1
	oris	r3,r3,0x0100
	mtspr	SPRN_HDBCR1,r3
#endif

#ifdef CONFIG_SYS_FSL_ERRATUM_A004510
	mfspr	r3,SPRN_SVR
	rlwinm	r3,r3,0,0xff
	li	r4,CONFIG_SYS_FSL_ERRATUM_A004510_SVR_REV
	cmpw	r3,r4
	beq	1f

#ifdef CONFIG_SYS_FSL_ERRATUM_A004510_SVR_REV2
	li	r4,CONFIG_SYS_FSL_ERRATUM_A004510_SVR_REV2
	cmpw	r3,r4
	beq	1f
#endif

	/* Not a supported revision affected by erratum */
	b	2f

1:	/* Erratum says set bits 55:60 to 001001 */
	msync
	isync
	mfspr	r3,SPRN_HDBCR0
	li	r4,0x48
	rlwimi	r3,r4,0,0x1f8
	mtspr	SPRN_HDBCR0,r3
	isync
2:
#endif

	/* Enable branch prediction */
	lis	r3,BUCSR_ENABLE@h
	ori	r3,r3,BUCSR_ENABLE@l
	mtspr	SPRN_BUCSR,r3

	/* Ensure TB is 0 */
	li	r3,0
	mttbl	r3
	mttbu	r3

	/* Enable/invalidate the I-Cache */
	lis	r2,(L1CSR1_ICFI|L1CSR1_ICLFR)@h
	ori	r2,r2,(L1CSR1_ICFI|L1CSR1_ICLFR)@l
	mtspr	SPRN_L1CSR1,r2
1:
	mfspr	r3,SPRN_L1CSR1
	and.	r1,r3,r2
	bne	1b

	lis	r3,(L1CSR1_CPE|L1CSR1_ICE)@h
	ori	r3,r3,(L1CSR1_CPE|L1CSR1_ICE)@l
	mtspr	SPRN_L1CSR1,r3
	isync
2:
	mfspr	r3,SPRN_L1CSR1
	andi.	r1,r3,L1CSR1_ICE@l
	beq	2b

	/* Enable/invalidate the D-Cache */
	lis	r2,(L1CSR0_DCFI|L1CSR0_DCLFR)@h
	ori	r2,r2,(L1CSR0_DCFI|L1CSR0_DCLFR)@l
	mtspr	SPRN_L1CSR0,r2
1:
	mfspr	r3,SPRN_L1CSR0
	and.	r1,r3,r2
	bne	1b

	lis	r3,(L1CSR0_CPE|L1CSR0_DCE)@h
	ori	r3,r3,(L1CSR0_CPE|L1CSR0_DCE)@l
	mtspr	SPRN_L1CSR0,r3
	isync
2:
	mfspr	r3,SPRN_L1CSR0
	andi.	r1,r3,L1CSR0_DCE@l
	beq	2b

#define toreset(x) (x - __secondary_start_page + 0xfffff000)

	/* get our PIR to figure out our table entry */
	lis	r3,toreset(__spin_table_addr)@h
	ori	r3,r3,toreset(__spin_table_addr)@l
	lwz	r3,0(r3)

	mfspr	r0,SPRN_PIR
#ifdef CONFIG_SYS_FSL_QORIQ_CHASSIS2
/*
 * PIR definition for Chassis 2
 * 0-17 Reserved (logic 0s)
 * 18-19 CHIP_ID,    2'b00      - SoC 1
 *                  all others - reserved
 * 20-24 CLUSTER_ID 5'b00000   - CCM 1
 *                  all others - reserved
 * 25-26 CORE_CLUSTER_ID 2'b00 - cluster 1
 *                       2'b01 - cluster 2
 *                       2'b10 - cluster 3
 *                       2'b11 - cluster 4
 * 27-28 CORE_ID         2'b00 - core 0
 *                       2'b01 - core 1
 *                       2'b10 - core 2
 *                       2'b11 - core 3
 * 29-31 THREAD_ID       3'b000 - thread 0
 *                       3'b001 - thread 1
 *
 * Power-on PIR increments threads by 0x01, cores within a cluster by 0x08
 * and clusters by 0x20.
 *
 * We renumber PIR so that all threads in the system are consecutive.
 */

	rlwinm	r8,r0,29,0x03	/* r8 = core within cluster */
	srwi	r10,r0,5	/* r10 = cluster */

	mulli	r5,r10,CONFIG_SYS_FSL_CORES_PER_CLUSTER
	add	r5,r5,r8	/* for spin table index */
	mulli	r4,r5,CONFIG_SYS_FSL_THREADS_PER_CORE	/* for PIR */
#elif	defined(CONFIG_E500MC)
	rlwinm	r4,r0,27,27,31
	mr	r5,r4
#else
	mr	r4,r0
	mr	r5,r4
#endif

	/*
	 * r10 has the base address for the entry.
	 * we cannot access it yet before setting up a new TLB
	 */
	slwi	r8,r5,6	/* spin table is padded to 64 byte */
	add	r10,r3,r8

	mtspr	SPRN_PIR,r4	/* write to PIR register */

#ifdef CONFIG_SYS_FSL_ERRATUM_A007907
	mfspr	r8, L1CSR2
	clrrwi	r8, r8, 10	/* clear bit [54-63] DCSTASHID */
	mtspr	L1CSR2, r8
#else
#ifdef CONFIG_SYS_CACHE_STASHING
	/* set stash id to (coreID) * 2 + 32 + L1 CT (0) */
	slwi	r8,r4,1
	addi	r8,r8,32
	mtspr	L1CSR2,r8
#endif
#endif	/* CONFIG_SYS_FSL_ERRATUM_A007907 */

#if defined(CONFIG_SYS_P4080_ERRATUM_CPU22) || \
	defined(CONFIG_SYS_FSL_ERRATUM_NMG_CPU_A011)
	/*
	 * CPU22 applies to P4080 rev 1.0, 2.0, fixed in 3.0
	 * NMG_CPU_A011 applies to P4080 rev 1.0, 2.0, fixed in 3.0
	 * also appleis to P3041 rev 1.0, 1.1, P2041 rev 1.0, 1.1
	 */
	mfspr   r3,SPRN_SVR
	rlwinm	r6,r3,24,~0x800		/* clear E bit */

	lis	r5,SVR_P4080@h
	ori	r5,r5,SVR_P4080@l
	cmpw	r6,r5
	bne	1f

	rlwinm  r3,r3,0,0xf0
	li      r5,0x30
	cmpw    r3,r5
	bge     2f
1:
#ifdef	CONFIG_SYS_FSL_ERRATUM_NMG_CPU_A011
	lis	r3,toreset(enable_cpu_a011_workaround)@ha
	lwz	r3,toreset(enable_cpu_a011_workaround)@l(r3)
	cmpwi	r3,0
	beq	2f
#endif
	mfspr	r3,L1CSR2
	oris	r3,r3,(L1CSR2_DCWS)@h
	mtspr	L1CSR2,r3
2:
#endif

#ifdef CONFIG_SYS_FSL_ERRATUM_A005812
	/*
	 * A-005812 workaround sets bit 32 of SPR 976 for SoCs running in
	 * write shadow mode. This code should run after other code setting
	 * DCWS.
	 */
	mfspr	r3,L1CSR2
	andis.	r3,r3,(L1CSR2_DCWS)@h
	beq	1f
	mfspr	r3, SPRN_HDBCR0
	oris	r3, r3, 0x8000
	mtspr	SPRN_HDBCR0, r3
1:
#endif

#ifdef CONFIG_BACKSIDE_L2_CACHE
	/* skip L2 setup on P2040/P2040E as they have no L2 */
	mfspr	r3,SPRN_SVR
	rlwinm	r6,r3,24,~0x800		/* clear E bit of SVR */

	lis	r3,SVR_P2040@h
	ori	r3,r3,SVR_P2040@l
	cmpw	r6,r3
	beq 3f

	/* Enable/invalidate the L2 cache */
	msync
	lis	r2,(L2CSR0_L2FI|L2CSR0_L2LFC)@h
	ori	r2,r2,(L2CSR0_L2FI|L2CSR0_L2LFC)@l
	mtspr	SPRN_L2CSR0,r2
1:
	mfspr	r3,SPRN_L2CSR0
	and.	r1,r3,r2
	bne	1b

#ifdef CONFIG_SYS_CACHE_STASHING
	/* set stash id to (coreID) * 2 + 32 + L2 (1) */
	addi	r3,r8,1
	mtspr	SPRN_L2CSR1,r3
#endif

	lis	r3,CFG_SYS_INIT_L2CSR0@h
	ori	r3,r3,CFG_SYS_INIT_L2CSR0@l
	mtspr	SPRN_L2CSR0,r3
	isync
2:
	mfspr	r3,SPRN_L2CSR0
	andis.	r1,r3,L2CSR0_L2E@h
	beq	2b
#endif
3:
	/* setup mapping for the spin table, WIMGE=0b00100 */
	lis	r13,toreset(__spin_table_addr)@h
	ori	r13,r13,toreset(__spin_table_addr)@l
	lwz	r13,0(r13)
	/* mask by 4K */
	rlwinm	r13,r13,0,0,19

	lis	r11,(MAS0_TLBSEL(1)|MAS0_ESEL(1))@h
	mtspr	SPRN_MAS0,r11
	lis	r11,(MAS1_VALID|MAS1_IPROT)@h
	ori	r11,r11,(MAS1_TS|MAS1_TSIZE(BOOKE_PAGESZ_4K))@l
	mtspr	SPRN_MAS1,r11
	oris	r11,r13,(MAS2_M|MAS2_G|MAS2_E)@h
	ori	r11,r13,(MAS2_M|MAS2_G|MAS2_E)@l
	mtspr	SPRN_MAS2,r11
	oris	r11,r13,(MAS3_SX|MAS3_SW|MAS3_SR)@h
	ori	r11,r13,(MAS3_SX|MAS3_SW|MAS3_SR)@l
	mtspr	SPRN_MAS3,r11
	li	r11,0
	mtspr	SPRN_MAS7,r11
	tlbwe

	/*
	 * __bootpg_addr has the address of __second_half_boot_page
	 * jump there in AS=1 space with cache enabled
	 */
	lis	r13,toreset(__bootpg_addr)@h
	ori	r13,r13,toreset(__bootpg_addr)@l
	lwz	r11,0(r13)
	mtspr	SPRN_SRR0,r11
	mfmsr	r13
	ori	r12,r13,MSR_IS|MSR_DS@l
	mtspr	SPRN_SRR1,r12
	rfi

	/*
	 * Allocate some space for the SDRAM address of the bootpg.
	 * This variable has to be in the boot page so that it can
	 * be accessed by secondary cores when they come out of reset.
	 */
	.align L1_CACHE_SHIFT
	.globl __bootpg_addr
__bootpg_addr:
	.long	0

	.global __spin_table_addr
__spin_table_addr:
	.long	0

	/*
	 * This variable is set by cpu_init_r() after parsing hwconfig
	 * to enable workaround for erratum NMG_CPU_A011.
	 */
	.align L1_CACHE_SHIFT
	.global enable_cpu_a011_workaround
enable_cpu_a011_workaround:
	.long	1

	/* Fill in the empty space.  The actual reset vector is
	 * the last word of the page */
__secondary_start_code_end:
	.space 4092 - (__secondary_start_code_end - __secondary_start_page)
__secondary_reset_vector:
	b	__secondary_start_page


/* this is a separated page for the spin table and cacheable boot code */
	.align L1_CACHE_SHIFT
	.global __second_half_boot_page
__second_half_boot_page:
	/*
	 * We are now executing in Little Endian mode!
	 * All instructions below are byte-swapped `.long` hex values
	 * generated by powerpc64le-linux-as to execute natively as LE.
	 */
	.long 0x00006038 /* li      r3,0              */
	.long 0x01000039 /* li      r8,1              */
	.long 0xa6429e7c /* mfspr   r4,SPRN_PIR       */
	.long 0x00006a90 /* stw     r3,ENTRY_ADDR_UPPER(r10) */
	.long 0x08006a90 /* stw     r3,ENTRY_R3_UPPER(r10)   */
	.long 0x0c008a90 /* stw     r4,ENTRY_R3_LOWER(r10)   */
	.long 0x10006a90 /* stw     r3,ENTRY_RESV(r10)       */
	.long 0x14008a90 /* stw     r4,ENTRY_PIR(r10)        */
	.long 0xac04007c /* msync                     */
	.long 0x04000a91 /* stw     r8,ENTRY_ADDR_LOWER(r10) */

3:
	.long 0x04008a80 /* lwz     r4,ENTRY_ADDR_LOWER(r10) */
	.long 0x01008b70 /* andi.   r11,r4,1          */
	.long 0xf8ff8240 /* bne     3b                */
	.long 0x2c01004c /* isync                     */

	.long 0x00006a81 /* lwz     r11,ENTRY_ADDR_UPPER(r10)*/
	.long 0xa6039a7c /* mtspr   SPRN_SRR0,r4      */

	.long 0x03000039 /* li      r8,3              */
	.long 0x04000a91 /* stw     r8,ENTRY_ADDR_LOWER(r10) */

	.long 0x0a008c54 /* rlwinm  r12,r4,0,0,5      */

	.long 0x0c006a80 /* lwz     r3,ENTRY_R3_LOWER(r10)   */
	.long 0x00008038 /* li      r4,0              */
	.long 0x0000a038 /* li      r5,0              */
	.long 0x0000c038 /* li      r6,0              */
	.long 0x0004e03c /* lis     r7,1024           */ /* 64M */
	.long 0x00000039 /* li      r8,0              */
	.long 0x00002039 /* li      r9,0              */

	.long 0x14000a80 /* lwz     r0,ENTRY_PIR(r10)        */
	.long 0xa6431e7c /* mtspr   SPRN_PIR,r0       */
	.long 0xa6421e7c /* mfspr   r0,SPRN_PIR       */
	.long 0x14000a90 /* stw     r0,ENTRY_PIR(r10)        */

	.long 0xa60b9f7d /* mtspr   IVPR,r12          */

	.long 0x0010403d /* lis     r10,4096          */ /* MAS0_TLBSEL(1)|MAS0_ESEL(0) */
	.long 0xa69b507d /* mtspr   SPRN_MAS0,r10     */
	.long 0x00c0403d /* lis     r10,-16384        */ /* MAS1_VALID|MAS1_IPROT */
	.long 0x800e4a61 /* ori     r10,r10,3712      */ /* MAS1_TSIZE(64M) */
	.long 0xa69b517d /* mtspr   SPRN_MAS1,r10     */
	.long 0xa69b927d /* mtspr   SPRN_MAS2,r12     */
	.long 0x15008c61 /* ori     r12,r12,21        */ /* MAS3_SX|MAS3_SW|MAS3_SR */
	.long 0xa69b937d /* mtspr   SPRN_MAS3,r12     */
	.long 0xa407007c /* tlbwe                     */

	.long 0xa603bb7d /* mtspr   SPRN_SRR1,r13     */
	.long 0x6400004c /* rfi                       */


	.align 6
	.globl __spin_table
__spin_table:
#define ENTRY_SIZE		64
	.space CONFIG_MAX_CPUS*ENTRY_SIZE

#ifdef CONFIG_PPC_SPINTABLE_COMPATIBLE
	.align L1_CACHE_SHIFT
	.global spin_table_compat
spin_table_compat:
	.long	1

#endif

__spin_table_end:
	.space 4096 - (__spin_table_end - __spin_table)
