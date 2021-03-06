/*
 * Copyright (C) 2018 Marvell International Ltd.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 * https://spdx.org/licenses
 */
 
#include <armada_common.h>
#include <ap_setup.h>
#include <cp110_setup.h>
#include <debug.h>
#include <marvell_plat_priv.h>
#include <marvell_pm.h>
#include <mmio.h>
#include <mci.h>
#include <plat_marvell.h>
#include <mc_trustzone/mc_trustzone.h>

#include <mss_ipc_drv.h>
#include <mss_mem.h>

/* In Armada-8k family AP806/AP807, CP0 connected to PIDI
 * and CP1 connected to IHB via MCI #0
 */
#define MVEBU_MCI0		0

static _Bool pm_fw_running;

/* Set a weak stub for platforms that don't need to configure GPIO */
#pragma weak marvell_gpio_config
int marvell_gpio_config(void)
{
	return 0;
}

static void marvell_bl31_mpp_init(int cp)
{
	uint32_t reg;

	/* need to do for CP#0 only */
	if (cp)
		return;


	/*
	 * Enable CP0 I2C MPPs (MPP: 37-38)
	 * U-Boot rely on proper MPP settings for I2C EEPROM usage
	 * (only for CP0)
	 */
	reg = mmio_read_32(MVEBU_CP_MPP_REGS(0, 4));
	mmio_write_32(MVEBU_CP_MPP_REGS(0, 4), reg | 0x2200000);
}

void marvell_bl31_mss_init(void)
{
	struct mss_pm_ctrl_block *mss_pm_crtl =
			(struct mss_pm_ctrl_block *)MSS_SRAM_PM_CONTROL_BASE;

	/* Check that the image was loaded successfully */
	if (mss_pm_crtl->handshake != HOST_ACKNOWLEDGMENT) {
		NOTICE("MSS PM is not supported in this build\n");
		return;
	}

	/* If we got here it means that the PM firmware is running */
	pm_fw_running = 1;

	INFO("MSS IPC init\n");

	if (mss_pm_crtl->ipc_state == IPC_INITIALIZED)
		mv_pm_ipc_init(mss_pm_crtl->ipc_base_address | MVEBU_REGS_BASE);
}

_Bool is_pm_fw_running(void)
{
	return pm_fw_running;
}

/* For TrusTzone we treat the "target" filed of addr_map_win
 * stuct as attribute
 */
static const struct addr_map_win tz_map[] = {
	{PLAT_MARVELL_ATF_BASE, 0x200000, TZ_PERM_ABORT}
};

/* Configure MC TrustZone regions */
static void plat_marvell_security_setup(void)
{
	int tz_nr, win_id;

	tz_nr = ARRAY_SIZE(tz_map);

	for (win_id = 0; win_id < tz_nr; win_id++)
		tz_enable_win(MVEBU_AP0, tz_map, win_id);
}

#ifdef MVEBU_PCIE_ECAM_WA
extern int bl31_ext_lib_main(void);
#endif

/* This function overruns the same function in marvell_bl31_setup.c */
void bl31_plat_arch_setup(void)
{
	int cp;
	uintptr_t *mailbox = (void *)PLAT_MARVELL_MAILBOX_BASE;

	/* initiliaze the timer for mdelay/udelay functionality */
	plat_delay_timer_init();

	/* configure apn806 */
	ap_init();

	/* In marvell_bl31_plat_arch_setup, el3 mmu is configured.
	 * el3 mmu configuration MUST be called after apn806_init, if not,
	 * this will cause an hang in init_io_win
	 * (after setting the IO windows GCR values).
	 */
	if (mailbox[MBOX_IDX_MAGIC] != MVEBU_MAILBOX_MAGIC_NUM ||
	    mailbox[MBOX_IDX_SUSPEND_MAGIC] != MVEBU_MAILBOX_SUSPEND_STATE)
		marvell_bl31_plat_arch_setup();

	for (cp = 0; cp < CP_COUNT; cp++) {
	/* configure cp110 for CP0*/
		if (cp == 1)
			mci_initialize(MVEBU_MCI0);

	/* initialize MCI & CP1 */
		cp110_init(MVEBU_CP_REGS_BASE(cp),
			   STREAM_ID_BASE + (cp * MAX_STREAM_ID_PER_CP));

	/* Should be called only after setting IOB windows */
		marvell_bl31_mpp_init(cp);
	}

	/* initialize IPC between MSS and ATF */
	if (mailbox[MBOX_IDX_MAGIC] != MVEBU_MAILBOX_MAGIC_NUM ||
	    mailbox[MBOX_IDX_SUSPEND_MAGIC] != MVEBU_MAILBOX_SUSPEND_STATE)
		marvell_bl31_mss_init();

	/* Configure GPIO */
	marvell_gpio_config();

	plat_marvell_security_setup();

#ifdef MVEBU_PCIE_ECAM_WA
	bl31_ext_lib_main();
#endif
}
