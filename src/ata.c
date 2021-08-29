/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: ata.c
 * DATE: August 28, 2021
 * DESCRIPTION: ATA hard disk driver
 */
#include <ata.h>
#include <io.h>

static void ata_wait(enum ata_wait_occasion);

void ata_read(void *buff, uint lba, size_t len)
{
	u16 *p = (u16 *) buff;

	// get how many sectors we need to transfer
	int sector_count = len / 512;
	if (sector_count == 0)
		sector_count = 1;
	
	else if (len % 512 != 0)
		sector_count++;
	
	for (int i = 0; i < sector_count; ++i, lba++, p += 256)
		ata_read_sector(p, lba);
}

void ata_write(void *buff, uint lba, size_t len)
{
	u16 *p = (u16 *) buff;

	// get how many sectors we need to transfer
	int sector_count = len / 512;
	if (sector_count == 0)
		sector_count = 1;
	
	else if (len % 512 != 0)
		sector_count++;
	
	for (int i = 0; i < sector_count; ++i, lba++, p += 256)
		ata_write_sector(p, lba);
}

void ata_read_sector(u16 *buff, uint lba)
{
	// wait until disk is not busy
	ata_wait(~ATA_WAIT_BSY);

	// send 0xe0 ORed with the highest 4 bits of the LBA to port 0x1f6:
	outb(ATA_LBA_PORT, 0xe0 | (lba >> 24 & 0xf));

	// reading just 1 sector
	outb(ATA_SECTOR_COUNT_PORT, 1);

	// send lba to the disk byte by byte
	outb(ATA_LBA_LOW_PORT,  lba >> 0);
	outb(ATA_LBA_MID_PORT,  lba >> 8);
	outb(ATA_LBA_HIGH_PORT, lba >> 16);

	// send the read command
	outb(ATA_CMD_PORT, ATA_CMD_READ);

	// wait until disk is ready to transfer data
	ata_wait(~ATA_WAIT_BSY);
	ata_wait(ATA_WAIT_RDY);

	// 256 instead of 512 because we transfer by word
	for (int i = 0; i < 256; ++i)
		buff[i] = inw(ATA_DATA_PORT);
}

void ata_write_sector(u16 *buff, uint lba)
{
	ata_wait(~ATA_WAIT_BSY);
	outb(ATA_LBA_PORT, 0xe0 | (lba >> 24 & 0xf));

	// write just one sector
	outb(ATA_SECTOR_COUNT_PORT, 1);

	// send lba to the disk byte by byte
	outb(ATA_LBA_LOW_PORT,  lba >> 0);
	outb(ATA_LBA_MID_PORT,  lba >> 8);
	outb(ATA_LBA_HIGH_PORT, lba >> 16);

	// send the write command 
	outb(ATA_CMD_PORT, ATA_CMD_WRITE);

	// wait until disk is ready to transfer data
	ata_wait(~ATA_WAIT_BSY);
	ata_wait(ATA_WAIT_RDY);

	// 256 instead of 512 because we transfer by word
	for(int i = 0; i < 256; ++i)
		outw(ATA_DATA_PORT, buff[i]);
}

static void ata_wait(enum ata_wait_occasion occasion)
{
	while (!(inb(ATA_STATUS_PORT) & occasion))
		;
}
