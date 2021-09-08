/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: fat.c
 * DATE: September 3, 2021
 * DESCRIPTION: FAT32 driver
 */
#include <fat.h>

#include <ata.h>
#include <kout.h>

static u32 root_dir_sectors = 0;    // in FAT32, this is always 0
static u32 first_data_sector;
static u32 data_sectors;            // number of data sectors on disk
static u32 cluster_count;           // count of data clusers STARTING at cluster 2
static uint active_cluster;

// static uint first_sector_of_cluster(uint);

// given any valid cluster number, where in the FAT(s) is the entry for the cluster?
static uint fat_offset(uint);

struct bpb bpb;
struct fsinfo fsinfo;

u8 fat[512 * 1024];

// buffer to hold a single sector
u8 secbuff[512];

void fatinit()
{
    ata_read(&bpb, FAT_OFFSET, sizeof(bpb));
    ata_read(&fsinfo, bpb.fsinfo_sector + FAT_OFFSET, sizeof(fsinfo));
    ata_read(fat, bpb.reserved_sectors + FAT_OFFSET, sizeof(fat));

    first_data_sector = bpb.reserved_sectors + (bpb.num_fats * bpb.sectors_per_fat) + root_dir_sectors + FAT_OFFSET;
    data_sectors = bpb.large_sector_count - (bpb.reserved_sectors + bpb.num_fats * bpb.sectors_per_fat) + root_dir_sectors;
    cluster_count = data_sectors / bpb.sectors_per_cluster;
    active_cluster = 2;

    if (cluster_count <= FAT32_MIN_CLUSTERS)
    {
        kprintf("Error: non FAT32 volume detected\n");
        while (1)
            ;
    }

    // print_bpb(&bpb);
    // print_fsinfo(&fsinfo);

    ata_read(secbuff, first_data_sector, sizeof(secbuff));

    // print root directory
    u8 *tmp = &secbuff[0];
    while (1)
    {
        // reached end of directory
        if (*tmp == 0)
            break;

        struct dirent_long *dlong = (struct dirent_long *) tmp;

        // this entry is a long name entry
        if (dlong->attr == ATTR_LONG_NAME)
        {
            // this might be the first time I've used an xor in a real application
            uint num_entries = dlong->ord ^ LAST_LONG_ENTRY;
            while (num_entries--)
                tmp += sizeof(struct dirent_long);
        }

        // get subsequent short entry
        struct dirent_short *dshort = (struct dirent_short *) tmp;
        kprintf("dirname: %s\n", dshort->name);
        tmp += sizeof(struct dirent_short);
    }

    // struct dirent_short *s = (struct dirent_short *) (secbuff + 0x20);
    // kprintf("%s\n", s->name);
    // kprintf("%d\n", s->attr);
    // kprintf("%d\n", s->ctime_tenths);
    // kprintf("%x\n", s->cluster_high);
    // kprintf("%x\n", s->mtime);
    // kprintf("%x\n", s->mdate);
    // kprintf("%x\n", s->cluster_low);
    // kprintf("%x\n", s->size);

    // uint root_cluster_offset = fat_offset(2);
    // kprintf("cluster %d\n");
    // u8 * ptr = fat[root_cluster_offset];

    // for (int i = 0; i < 10; i++)
    //     kprintf("%x\n", ptr[i]);
}

static uint first_sector_of_cluster(uint cluster)
{
    return (cluster - 2) * bpb.sectors_per_cluster + first_data_sector;
}

// given any valid cluster number, where in the FAT(s) is the entry for the cluster?
static uint fat_offset(uint cluster)
{
    uint offset = cluster * 4;
    uint secnum = bpb.reserved_sectors + offset / bpb.bytes_per_sector;
    return offset % bpb.bytes_per_sector;
}

void print_bpb(struct bpb *bpb)
{
    kprintf("bytes per sector: %d\n", bpb->bytes_per_sector);
    kprintf("sectors per cluster %d\n", bpb->sectors_per_cluster);
    kprintf("reserved sectors: %d\n", bpb->reserved_sectors);
    kprintf("num fats: %d\n", bpb->num_fats);
    kprintf("num dir entries: %d\n", bpb->num_dir_entries);
    kprintf("total sectors: %d\n", bpb->total_sectors);
    kprintf("media descriptor: %d\n", bpb->media_descriptor);
    kprintf("sectors per fat: %d\n", bpb->sectors_per_fat);
    kprintf("sectors per track: %d\n", bpb->sectors_per_track);
    kprintf("num heads: %d\n", bpb->heads);
    kprintf("num hidden sectors: %d\n", bpb->num_hidden_sectors);
    kprintf("large sector count: %d\n", bpb->large_sector_count);
    kprintf("sectors per fat: %d\n", bpb->sectors_per_fat);
    kprintf("fat version: %d\n", bpb->fat_version);
    kprintf("root cluster: %d\n", bpb->root_cluster);
    kprintf("fsinfo sector: %d\n", bpb->fsinfo_sector);
    kprintf("backup boot sector: %d\n", bpb->backup_boot_sector);
    kprintf("drive num: %d\n", bpb->drive_num);
    kprintf("windows flags: %d\n", bpb->win_flags); 
    kprintf("signature: %x\n", bpb->signature);
    kprintf("volume ID: %x\n", bpb->volume_id);
    kprintf("boot signature: %x\n", bpb->boot_signature);
}

void print_fsinfo(struct fsinfo *fsinfo)
{
    kprintf("lead signature: %x\n", fsinfo->lead_signature);
    kprintf("mid signature: %x\n", fsinfo->mid_signature);
    kprintf("free clusters: %d\n", fsinfo->free_clusters);
    kprintf("start cluster: %d\n", fsinfo->start_cluster);
    kprintf("trail signature: %x\n", fsinfo->trail_signature);
}
