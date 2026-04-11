/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2012 Freescale Semiconductor, Inc.
 */

#ifndef __T4QDS_H
#define __T4QDS_H

#ifndef __ASSEMBLY__
struct bd_info;
void fdt_fixup_board_enet(void *blob);
void pci_of_setup(void *blob, struct bd_info *bd);
#endif

#endif /* __T4QDS_H */
