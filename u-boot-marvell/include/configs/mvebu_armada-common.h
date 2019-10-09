/*
 * Copyright (C) 2016 Stefan Roese <sr@denx.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _CONFIG_MVEBU_ARMADA_H
#define _CONFIG_MVEBU_ARMADA_H

#include <asm/arch/soc.h>

/*
 * High Level Configuration Options (easy to change)
 */
#define CONFIG_DISPLAY_BOARDINFO_LATE

/* additions for new ARM relocation support */
#define CONFIG_SYS_SDRAM_BASE	0x00000000

/* auto boot */
#define CONFIG_PREBOOT

#define CONFIG_BAUDRATE			115200
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, \
					  115200, 230400, 460800, 921600 }

/* Default Env vars */
#define CONFIG_IPADDR			192.168.1.119	/* In order to cause an error */
#define CONFIG_SERVERIP			192.168.1.254	/* In order to cause an error */
#define CONFIG_NETMASK			255.255.255.0
#define CONFIG_GATEWAYIP		192.168.1.254
#define CONFIG_HAS_ETH1
#define CONFIG_HAS_ETH2
#define CONFIG_ETHPRIME			"eth0"
#define CONFIG_ROOTPATH                 "/srv/nfs/" /* Default Dir for NFS */
#define CONFIG_ENV_OVERWRITE		/* ethaddr can be reprogrammed */
#define CONFIG_EXTRA_ENV_SETTINGS	\
					"kernel_addr=0x7000000\0"	\
					"initrd_addr=0x1100000\0"	\
					"initrd_size=0x2000000\0"	\
					"fdt_addr=0x6000000\0"	\
					"loadaddr=0x8000000\0"		\
					"fdt_high=0xffffffffffffffff\0"	\
					"scriptaddr=0x6d00000\0"	\
					"hostname=catdrive\0"		\
					"netdev=eth0\0"			\
					"ethaddr=00:11:32:11:22:00\0"	\
					"image_name=zImage\0"		\
					"console=" CONFIG_DEFAULT_CONSOLE "\0"\
				\
					"boot_targets=usb mmc0 sata\0"	\
					"boot_prefixes=/ /boot/\0"	\
					"boot_a_script=fatload ${boot_interface} ${devnum}:1 ${scriptaddr} ${prefix}${scriptname}; source ${scriptaddr};\0" \
					"scan_dev_for_boot=for prefix in ${boot_prefixes}; do echo ${prefix}; run boot_a_script; done;\0" \
				\
					"boot_usb=setenv devnum 0; usb start; setenv boot_interface usb; run scan_dev_for_boot;\0" \
					"boot_sata=setenv devnum 0; scsi scan; scsi dev 0; setenv boot_interface scsi; run scan_dev_for_boot;\0" \
					"boot_mmc0=setenv devnum 0; setenv boot_interface mmc; run scan_dev_for_boot;\0" \
					"boot_syno=setenv devnum 0; setenv boot_interface mmc; run scan_dev_for_boot;\0" \
				\
					"bootcmd_normal=setenv scriptname syno.scr; run boot_syno;\0" \
					"bootcmd_button=setenv scriptname boot.scr; for target in ${boot_targets}; do run boot_${target}; done;\0" \
					"bootcmd=gpio input GPIO23; if test $? = $default_mode; then echo \"Enter button mode\"; run bootcmd_button; else echo \"Enter normal Mode\"; run bootcmd_normal; fi\0" \
				\
					"default_mode=0\0"
/*
 * For booting Linux, the board info and command line data
 * have to be in the first 8 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
#define CONFIG_CMDLINE_TAG		/* enable passing of ATAGs  */
#define CONFIG_INITRD_TAG		/* enable INITRD tag */
#define CONFIG_SETUP_MEMORY_TAGS	/* enable memory tag */

#define	CONFIG_SYS_CBSIZE	1024	/* Console I/O Buff Size */
#define	CONFIG_SYS_PBSIZE	(CONFIG_SYS_CBSIZE \
		+ sizeof(CONFIG_SYS_PROMPT) + 16)	/* Print Buff */

/*
 * Size of malloc() pool
 */
#define CONFIG_SYS_MALLOC_LEN	(9 << 20) /* 16MiB for malloc() */

/*
 * Other required minimal configurations
 */
#define CONFIG_ARCH_CPU_INIT		/* call arch_cpu_init() */
#define CONFIG_SYS_LOAD_ADDR	0x00800000	/* default load adr- 8M */
#define CONFIG_SYS_MEMTEST_START 0x00800000	/* 8M */
#define CONFIG_SYS_MEMTEST_END	0x00ffffff	/*(_16M -1) */
#define CONFIG_SYS_RESET_ADDRESS 0xffff0000	/* Rst Vector Adr */
#define CONFIG_SYS_MAXARGS	32	/* max number of command args */

#define CONFIG_SYS_ALT_MEMTEST

/* End of 16M scrubbed by training in bootrom */
#define CONFIG_SYS_INIT_SP_ADDR         (CONFIG_SYS_TEXT_BASE + 0xFF0000)

/*
 * SPI Flash configuration
 */
#define CONFIG_ENV_SPI_BUS		0
#define CONFIG_ENV_SPI_CS		0

/* SPI NOR flash default params, used by sf commands */
#define CONFIG_SF_DEFAULT_SPEED		40000000
#define CONFIG_SF_DEFAULT_MODE		SPI_MODE_0
#define CONFIG_ENV_SPI_MODE		CONFIG_SF_DEFAULT_MODE

/*
 * Assume minimum flash/eMMC boot partition size of 4MB
 * and save the environment at the end of the boot device
 * There is one exclusion from this rule - the EspressoBIN board with eMMC.
 * The eMMC device found on some EspressoBIN V7 boards has 2MB boot partition.
 */
#define CONFIG_ENV_SIZE			(64 << 7) /* 8KiB */
#define CONFIG_ENV_SECT_SIZE		(64 << 7) /* 8KiB sectors */

#ifdef CONFIG_MVEBU_NAND_BOOT
/* In case of NAND, we want to start the environment on page boundary */
#define CONFIG_ENV_OFFSET		0x400000
#else
#if defined(CONFIG_ENV_IS_IN_MMC) && \
defined(CONFIG_TARGET_MVEBU_ARMADA_37XX) && defined(CONFIG_MV88E6XXX_SWITCH)
/* This one should be EspressoBin, since A3700 DB does not have
 * MV88E6XXX switch enabled. Is there any other way to detect it?
 */
#define CONFIG_ENV_OFFSET		(0x200000 - CONFIG_ENV_SIZE)
#else
#define CONFIG_ENV_OFFSET		(0x7fb000 - 0)
#endif
#endif

#ifdef CONFIG_ENV_IS_IN_MMC
#ifdef CONFIG_TARGET_MVEBU_ARMADA_37XX
#define CONFIG_SYS_MMC_ENV_DEV		1 /* A3700 emmc is on sdhci-1*/
#define CONFIG_SYS_MMC_ENV_PART		1 /* 0-DATA, 1-BOOT0, 2-BOOT1 */
#else
#define CONFIG_SYS_MMC_ENV_DEV		0
#define CONFIG_SYS_MMC_ENV_PART		0 /* 0-DATA, 1-BOOT0, 2-BOOT1 */
#endif
#endif

/*
 * SATA/SCSI/AHCI configuration
 */
#define CONFIG_SCSI_AHCI_PLAT
#define CONFIG_LBA48
#define CONFIG_SYS_64BIT_LBA

#define CONFIG_SYS_SCSI_MAX_SCSI_ID	2
#define CONFIG_SYS_SCSI_MAX_LUN		1
#define CONFIG_SYS_SCSI_MAX_DEVICE	(CONFIG_SYS_SCSI_MAX_SCSI_ID * \
					 CONFIG_SYS_SCSI_MAX_LUN)

/*
 * Ethernet Driver configuration
 */
#define CONFIG_ARP_TIMEOUT	200
#define CONFIG_NET_RETRY_COUNT	50

/*
 * The EEPROM ST M24C64 has 32 byte page write mode and takes up to 10 msec.
 */
#define CONFIG_SYS_EEPROM_PAGE_WRITE_DELAY_MS 10

#endif /* _CONFIG_MVEBU_ARMADA_H */
