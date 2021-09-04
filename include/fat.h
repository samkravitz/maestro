/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: fat.h
 * DATE: September 3, 2021
 * DESCRIPTION: FAT32 driver
 * RESOURCES:
 * http://download.microsoft.com/download/1/6/1/161ba512-40e2-4cc9-843a-923143f3456c/fatgen103.doc
 * https://www.pjrc.com/tech/8051/ide/fat32.html
 */
#ifndef FAT_H
#define FAT_H

#include <maestro.h>

// minimum amount of clusters a FAT32 filesystem can have
#define FAT32_MIN_CLUSTERS  65525

// FAT filesystem start sector
#define FAT_OFFSET          2048

// BIOS parameter block
struct bpb
{
    u8 jmp[3];
    u8 oem[8];
    u16 bytes_per_sector;
    u8 sectors_per_cluster;
    u16 reserved_sectors;
    u8 num_fats;
    u16 num_dir_entries;
    u16 total_sectors;
    u8 media_descriptor;
    u16 sectors_per_fat16;      // FAT12/FAT16 only
    u16 sectors_per_track;
    u16 heads;                  // number of heads or sides on the storage media
    u32 num_hidden_sectors;
    u32 large_sector_count;

    // extended boot record
    u32 sectors_per_fat;        // size of the FAT in sectors
    u16 flags;
    u16 fat_version;
    u32 root_cluster;           // cluster number of root directory
    u16 fsinfo_sector;          // sector number of FSInfo structure
    u16 backup_boot_sector;     // sector number of backup boot sector
    u8 reserved[12];            // when volume is formatted this should be 0
    u8 drive_num;
    u8 win_flags;               // used in Windows NT
    u8 signature;               // must be 0x28 or 0x29
    u32 volume_id;
    u8 volume_label[11];
    u8 sys_id[8];               // never trust this string for any  use
    u8 boot_code[420];
    u16 boot_signature;         // must be 0xaa55;
} __attribute__((packed));

// FSInfo struct (FAT32 only)
struct fsinfo
{
    u32 lead_signature;         // must be 0x41615252
    u8 reserved[480];
    u32 mid_signature;          // must be 0x61417272
    u32 free_clusters;          // if the value is 0xffffffff, then the free count is unknown and must be computed. However, this value might be incorrect and should at least be range checked (<= volume cluster count)

    /*
     * Indicates the cluster number at which the filesystem driver should start looking for available clusters
     * If the value is 0xffffffff, then there is no hint and the driver should start searching at 2
     * Typically this value is set to the last allocated cluster number. As the previous field, this value should be range checked.
     */
    u32 start_cluster;
    u8 rsvd[12];
    u32 trail_signature;        // should be 0xaa550000
} __attribute__((packed));

static enum fat_attr
{
    ATTR_READ_ONLY      =   0x01,
    ATTR_HIDDEN         = 	0x02,
    ATTR_SYSTEM         = 	0x04,
    ATTR_VOLUME_ID      = 	0x08,
    ATTR_DIRECTORY      =	0x10,
    ATTR_ARCHIVE        =  	0x20,
    ATTR_LONG_NAME      = 	ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID,
    ATTR_LONG_NAME_MASK	=   ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID | ATTR_DIRECTORY | ATTR_ARCHIVE,
};

// long directory entry
struct dirent_long
{
    u8 ord;
    u16 name1[5];
    u8 attr;
    u8 type;
    u8 checksum;
    u16 name2[6];
    u16 cluster_low;            // must be 0
    u16 name3[2];

    #define LAST_LONG_ENTRY     0x40
} __attribute__((packed));

// short directory entry
struct dirent_short
{
    char name[11];
    u8 attr;
    u8 rsvd;
    u8 ctime_tenths;            // milliseconds at creation time
    u8 empty[6];
    u16 cluster_high;           // high word of first cluster
    u16 mtime;                  // last modification time
    u16 mdate;                  // last modification date
    u16 cluster_low;            // low word of first cluster
    u32 size;                   // size in bytes
} __attribute__((packed));

static u32 root_dir_sectors = 0;    // in FAT32, this is always 0
static u32 first_data_sector;
static u32 data_sectors;            // number of data sectors on disk
static u32 cluster_count;           // count of data clusers STARTING at cluster 2
static uint active_cluster;

void fatinit();

static uint first_sector_of_cluster(uint);

// given any valid cluster number, where in the FAT(s) is the entry for the cluster?
static uint fat_offset(uint, uint);

void print_bpb(struct bpb *);
void print_fsinfo(struct fsinfo *);

#endif // FAT_H
