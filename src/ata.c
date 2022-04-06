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

static void ata_wait_bsy();
static void ata_wait_rdy();

void ata_read(void *buff, uint lba, size_t sector_count)
{
	u16 *p = (u16 *) buff;

	for (uint i = 0; i < sector_count; ++i, ++lba, p += 256)
		ata_read_sector(p, lba);
}

void ata_write(void *buff, uint lba, size_t sector_count)
{
	u16 *p = (u16 *) buff;

	for (uint i = 0; i < sector_count; ++i, ++lba, p += 256)
		ata_write_sector(p, lba);
}

void ata_read_sector(u16 *buff, uint lba)
{
	// wait until disk is not busy
	ata_wait_bsy();

	// send 0xe0 ORed with the highest 4 bits of the LBA to port 0x1f6:
	outb(ATA_LBA_PORT, 0xe0 | (lba >> 24 & 0xf));

	// reading just 1 sector
	outb(ATA_SECTOR_COUNT_PORT, 1);

	// send lba to the disk byte by byte
	outb(ATA_LBA_LOW_PORT, lba >> 0);
	io_wait();
	outb(ATA_LBA_MID_PORT, lba >> 8);
	io_wait();
	outb(ATA_LBA_HIGH_PORT, lba >> 16);
	io_wait();

	// send the read command
	outb(ATA_CMD_PORT, ATA_CMD_READ);
	io_wait();

	// wait until disk is ready to transfer data
	ata_wait_bsy();
	ata_wait_rdy();

	// 256 instead of 512 because we transfer by word
	for (int i = 0; i < 256; ++i)
		buff[i] = inw(ATA_DATA_PORT);
}

void ata_write_sector(u16 *buff, uint lba)
{
	// wait until disk is not busy
	ata_wait_bsy();

	outb(ATA_LBA_PORT, 0xe0 | (lba >> 24 & 0xf));
	io_wait();

	// write just one sector
	outb(ATA_SECTOR_COUNT_PORT, 1);
	io_wait();

	// send lba to the disk byte by byte
	outb(ATA_LBA_LOW_PORT, lba >> 0);
	io_wait();
	outb(ATA_LBA_MID_PORT, lba >> 8);
	io_wait();
	outb(ATA_LBA_HIGH_PORT, lba >> 16);
	io_wait();

	// send the write command
	outb(ATA_CMD_PORT, ATA_CMD_WRITE);
	io_wait();

	// wait until disk is ready to transfer data
	ata_wait_bsy();
	ata_wait_rdy();

	// 256 instead of 512 because we transfer by word
	for (int i = 0; i < 256; ++i)
		outw(ATA_DATA_PORT, buff[i]);
}

static void ata_wait_bsy()
{
	while (inb(ATA_STATUS_PORT) & ATA_WAIT_BSY)
		;
}

static void ata_wait_rdy()
{
	while (!(inb(ATA_STATUS_PORT) & ATA_WAIT_RDY))
		;
}
