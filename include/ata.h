/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: ata.h
 * DATE: August 28, 2021
 * DESCRIPTION: ATA hard disk driver
 */
#ifndef ATA_H
#define ATA_H

#include <maestro.h>

#define ATA_CMD_READ			0x20
#define ATA_CMD_WRITE			0x30

#define ATA_DATA_PORT			0x1f0
#define ATA_SECTOR_COUNT_PORT	0x1f2
#define ATA_LBA_LOW_PORT		0x1f3
#define ATA_LBA_MID_PORT		0x1f4
#define ATA_LBA_HIGH_PORT		0x1f5
#define ATA_LBA_PORT			0x1f6 
#define ATA_CMD_PORT			0x1f7	// used to write commands to the disk
#define ATA_STATUS_PORT			0x1f7	// used to read status from the disk

enum ata_wait_occasion
{
	ATA_WAIT_RDY = 1 << 6,
	ATA_WAIT_BSY = 1 << 7,	// we invert this because we want to wait until this bit is 0
} __attribute__((packed));

void ata_read(void *, uint, size_t);
void ata_write(void *, uint, size_t);
void ata_read_sector(u16 *, uint);
void ata_write_sector(u16 *, uint);

#endif