/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: ext2.h
 * DATE: September 8, 2021
 * DESCRIPTION: ext2 filesystem driver
 */
#include <ext2.h>

#include <ata.h>
#include <kmalloc.h>
#include <kout.h>

#include "string.h"

struct superblock sblk;

struct block_group_desc *block_group_desc_table;

// number of block groups in volume
static int block_groups;

static int sectors_per_block;

static u32 alloc_inode_id();
static u32 alloc_block();
static void print_inode(u32);
static struct inode *read_inode(struct inode *, u32);
static void write_inode(struct inode *, u32);

/**
 * macros to manipulate block/inode bitmaps
 * pass in buffer to bitmap and index to be worked with
 */
#define BMAP_SET(map, bit)		    (map[bit / 8] |=  (1 << (bit % 8)))
#define BMAP_CLEAR(map, bit)	    (map[bit / 8] &= ~(1 << (bit % 8)))
#define BMAP_TEST(map, bit)		    (map[bit / 8] &   (1 << (bit % 8)))

/**
 * reads ext2 filesystem block(s) from disk
 * @param buff buffer of at least BLOCK_SIZE bytes to read into
 * @param blk index of block in filesystem to read
 * @param n number of blocks to read
 */
static inline void read_block(u8 *buff, uint blk, int n)
{
    ata_read(buff, EXT2_OFFSET + blk * EXT2_SECTORS_PER_BLOCK, n * EXT2_SECTORS_PER_BLOCK);
}

/**
 * reads ext2 filesystem block(s) from disk
 * @param buff buffer of at least BLOCK_SIZE bytes to read into
 * @param blk index of block in filesystem to read
 * @param n number of blocks to read
 */
static inline void write_block(u8 *buff, uint blk, int n)
{
    ata_write(buff, EXT2_OFFSET + blk * EXT2_SECTORS_PER_BLOCK, n * EXT2_SECTORS_PER_BLOCK);
}

/**
 * writes the block group descriptor table to disk
 */
static inline void flush_block_group_descriptor_table()
{
    write_block(block_group_desc_table, EXT2_BLOCK_DESCRIPTOR, get_num_blocks(block_groups * sizeof(struct block_group_desc)));
}

/**
 * writes the superblock to disk
 */
static inline void flush_superblock()
{
    write_block(&sblk, EXT2_SUPERBLOCK, get_num_blocks(sizeof(sblk)));
}

void ext2_init()
{
    read_block((u8 *) &sblk, EXT2_SUPERBLOCK, 1);

    int inodes = sblk.inode_count;
    int blocks = sblk.block_count;
    int block_size = 1024 << sblk.log_block_size;
    sectors_per_block = block_size / 512;

    /*
    *   determine number of block groups
    *   as a sanity check, use both blocks and inodes to calculate
    *   round up the total number of inodes / num inodes per group
    *   round up the total number of blocks / num blocks per group
    */
	int block_groups_inode = inodes * 1.0f / sblk.inodes_per_group + .5f;
	int block_groups_block = blocks * 1.0f / sblk.blocks_per_group + .5f;

	if (block_groups_inode != block_groups_block)
		kprintf("ext2: inconsistency in block group count\n");

    // set global block groups to calculates number of block groups
    // this could be block_groups_inode or block_groups_block, they are equivalent
	block_groups = block_groups_inode;

    // allocate space for block group descriptor table
	block_group_desc_table = kmalloc(sizeof(struct block_group_desc) * block_groups);

    read_block((u8 *) block_group_desc_table, EXT2_BLOCK_DESCRIPTOR, get_num_blocks(sizeof(struct block_group_desc) * block_groups));

    print_inode(ROOT_INODE);

    // get this group's inode bitmap
    u8 buff[BLOCK_SIZE];
    read_block(buff, block_group_desc_table[0].inode_bitmap, 1);




    kprintf("First 16 inodes: ");
    for (int i = 0; i < 16; i++) {
        kprintf("%d ", BMAP_TEST(buff, i));
    }
    kprintf("\n");

    u32 in = alloc_inode_id();
    kprintf("%d %d\n", block_group_desc_table[0].inode_bitmap, in);
}

void print_inode(u32 ino)
{
    // find which block group the inode belongs to
    // int bg = (ino - 1) / sblk.inodes_per_group;

    // // block group descriptor corresponding to the group the inodes belongs to
    // struct block_group_desc bgd = block_group_desc_table[bg];

    // // read inode table for this block group into memory
    // struct inode *inode_table = kmalloc(sizeof(struct inode) * sblk.inodes_per_group);
    // read_block((u8 *) inode_table, bgd.inode_table, get_num_blocks(sizeof(struct inode) * sblk.inodes_per_group));

    // // index of inode in inode table (NOTE - inode index starts at 1)
    // int index = (ino - 1) % sblk.inodes_per_group;

    // struct inode *node = &inode_table[index];

    struct inode node;
    read_inode(&node, ino);

    // // buffer to hold block
    u8 buff[BLOCK_SIZE];
    read_block(buff, node.block_ptr[0], 1);

    // inode is a directory
    if (node.mode & INODE_MODE_DIR)
    {
        struct ext2_dir_entry *entry = (struct ext2_dir_entry *) buff;
        uint bytes_read = 0;
        do
        {
            char *ptr = (char *) entry;
            char *name = ptr + 8;
            name[entry->name_len] = '\0';
            kprintf("{ .inode: %d, .rec_len: %d, .name_len: %d, .name: %s }\n", entry->inode, entry->rec_len, entry->name_len, name);
            bytes_read += entry->rec_len;
            ptr += entry->rec_len;
            entry = (struct ext2_dir_entry  *) ptr;
        } while (bytes_read < 1024); // directory entries must fit in size of 1 block
    }

    // inode is a regular file
    else if (node.mode & INODE_MODE_REG)
    {
        kprintf("Reading inode of regular file %d\n", node.size);
    }

    // release resources
    //kfree(inode_table);
}

/**
 * finds the id of the first unused id
 * also updates the block group descriptor it's inode bitmap
 * @return id of the free inode or error if none is found
 */
static u32 alloc_inode_id()
{
    for (int i = 0; i < block_groups; ++i)
    {
        struct block_group_desc *bgd = &block_group_desc_table[i];

        // no free inodes in this block group
        if (bgd->free_inode_count <= 0)
            continue;
        
        // get this group's inode bitmap
        u8 buff[BLOCK_SIZE];
        read_block(buff, bgd->inode_bitmap, 1);

        for (int n = 0; n < sblk.inodes_per_group / 8; ++n)
        {
            if (!BMAP_TEST(buff, n))
            {
                // inode indeces start at 1, this is why we add 1 to the free bit we found
                u32 index = i * sblk.inodes_per_group + n + 1;
                bgd->free_inode_count--;
                BMAP_SET(buff, n);
                write_block(buff, bgd->inode_bitmap, 1);
                flush_block_group_descriptor_table();
                return index;
            }
        }

        // should never get here
        kprintf("EXT2 alloc_inode_id: something has gone terribly wrong!\n");
    }

    // no free inode could be found
    return EXT2_ALLOC_ERROR;
}

/**
 * finds the id of the first unused block
 * also updates the block group descriptor it's block bitmap
 * @return id of the block inode or error if none is found
 */
static u32 alloc_block()
{
    for (int i = 0; i < block_groups; ++i)
    {
        struct block_group_desc *bgd = &block_group_desc_table[i];

        // no free inodes in this block group
        if (bgd->free_block_count <= 0)
            continue;
        
        // get this group's inode bitmap
        u8 buff[BLOCK_SIZE];
        read_block(buff, bgd->block_bitmap, 1);

        for (int n = 0; n < sblk.blocks_per_group / 8; ++n)
        {
            if (!BMAP_TEST(buff, n))
            {
                u32 index = i * sblk.blocks_per_group + n;
                bgd->free_block_count--;
                BMAP_SET(buff, n);
                write_block(buff, bgd->block_bitmap, 1);
                flush_block_group_descriptor_table();
                return index;
            }
        }

        // should never get here
        kprintf("EXT2 alloc_block_id: something has gone terribly wrong!\n");
    }

    // no free block could be found
    return EXT2_ALLOC_ERROR;
}

static struct inode *read_inode(struct inode *inode, u32 idx)
{
    // find which block group the inode belongs to
    int bg = (idx - 1) / sblk.inodes_per_group;

    // block group descriptor corresponding to the group the inodes belongs to
    struct block_group_desc bgd = block_group_desc_table[bg];

    // index of inode in inode table (NOTE - inode index starts at 1)
    int index = (idx - 1) % sblk.inodes_per_group;
    int offset_within_block = index % (BLOCK_SIZE / sizeof(struct inode));

    // read block in inode table for this block group into memory
    uint8_t buff[BLOCK_SIZE];
    read_block(buff, bgd.inode_table + (index * sizeof(struct inode) / BLOCK_SIZE), 1);

    memcpy(inode, &buff[offset_within_block * sizeof(struct inode)], sizeof(struct inode));

    return inode;
}

static void write_inode(struct inode *inode, u32 idx)
{
    // find which block group the inode belongs to
    int bg = (idx - 1) / sblk.inodes_per_group;

    // block group descriptor corresponding to the group the inodes belongs to
    struct block_group_desc bgd = block_group_desc_table[bg];

    // index of inode in inode table (NOTE - inode index starts at 1)
    int index = (idx - 1) % sblk.inodes_per_group;
    int offset_within_block = index % (BLOCK_SIZE / sizeof(struct inode));

    // read block in inode table for this block group into memory
    uint8_t buff[BLOCK_SIZE];
    read_block(buff, bgd.inode_table + (index * sizeof(struct inode) / BLOCK_SIZE), 1);

    memcpy(&buff[offset_within_block * sizeof(struct inode)], inode, sizeof(struct inode));
    write_block(buff, bgd.inode_table + (index * sizeof(struct inode) / BLOCK_SIZE), 1);
}

/**
 * creates a new, empty ext2 directory
 * @param path absolute path of directory to create (must be null-terminated!)
 */
void ext2_mkdir(const char *path)
{
    u32 inode_idx = alloc_inode_id();
    u32 block_idx = alloc_block();

    // unsuccessful at finding a free block or free inode
    if (inode_idx == EXT2_ALLOC_ERROR || block_idx == EXT2_ALLOC_ERROR)
    {
        kprintf("ext2_mkdir: no free inode or block\n");
        return;
    }

    // inode representing the new directory
    struct inode dir;
    memset(&dir, 0, sizeof(dir));

    dir.mode |= INODE_MODE_DIR;
    dir.block_ptr[0] = block_idx;
    dir.links_count = 1;
    dir.size = 1024;

    // r/w/x permissions for user, group, others for now
    dir.mode |= INODE_MODE_RUSR;
	dir.mode |= INODE_MODE_WUSR;
	dir.mode |= INODE_MODE_XUSR;
	dir.mode |= INODE_MODE_RGRP;
	dir.mode |= INODE_MODE_WGRP;
	dir.mode |= INODE_MODE_XGRP;
	dir.mode |= INODE_MODE_ROTH;
	dir.mode |= INODE_MODE_WOTH;
	dir.mode |= INODE_MODE_XOTH;
    write_inode(&dir, inode_idx);

    struct inode root;
    read_inode(&root, ROOT_INODE);

    // buffer to hold block
    u8 buff[BLOCK_SIZE];
    read_block(buff, root.block_ptr[0], 1);

    struct ext2_dir_entry *entry = (struct ext2_dir_entry *) buff;
    uint bytes_read = 0;
    while (1)
    {
        char *ptr = (char *) entry;

        if (bytes_read + entry->rec_len == BLOCK_SIZE)
        {
            // adjust previously last entry's length
            entry->rec_len = round(sizeof(entry->inode) +
                                    sizeof(entry->rec_len) +
                                    sizeof(entry->name_len) +
                                    sizeof(entry->type) +
                                    entry->name_len, 4);

            // create entry for new directory
            struct ext2_dir_entry new_dir_entry;
            size_t name_len = strlen(path);
            new_dir_entry.inode = inode_idx;
            new_dir_entry.name_len = name_len;
            new_dir_entry.type = DIR_TYPE_DIR;
            new_dir_entry.rec_len = round(sizeof(new_dir_entry.inode) +
                                            sizeof(new_dir_entry.rec_len) +
                                            sizeof(new_dir_entry.name_len) +
                                            sizeof(new_dir_entry.type) +
                                            new_dir_entry.name_len, 4);
            
            // create . and .. entries for new directory
            char dotbuff[BLOCK_SIZE];
            struct ext2_dir_entry dot = {
                .inode = ROOT_INODE,
                .rec_len = 12,
                .name_len = 1,
                .type = DIR_TYPE_DIR,
            };
            struct ext2_dir_entry dotdot = {
                .inode = ROOT_INODE,
                .rec_len = 1024 - 12,
                .name_len = 2,
                .type = DIR_TYPE_DIR,
            };
            memcpy(&dotbuff[0], &dot, sizeof(dot));
            memcpy(&dotbuff[12], &dotdot, sizeof(dotdot));
            strcpy(&dotbuff[0 + 8], ".");
            strcpy(&dotbuff[12 + 8], "..");
            write_block(dotbuff, block_idx, 1);

            new_dir_entry.rec_len = BLOCK_SIZE - bytes_read - entry->rec_len;
            memcpy(&buff[bytes_read + entry->rec_len], &new_dir_entry, sizeof(new_dir_entry));
            strcpy(&buff[bytes_read + entry->rec_len + 8], path);
            break;
        }

        bytes_read += entry->rec_len;
        ptr += entry->rec_len;
        entry = (struct ext2_dir_entry *) ptr;
    }

    write_block(buff, root.block_ptr[0], 1);
}
