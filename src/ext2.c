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

struct superblock sblk;

struct block_group_desc *block_group_desc_table;

// number of block groups in volume
static int block_groups;

static int sectors_per_block;

void read_inode(u32);

void ext2_init()
{
    ata_read(&sblk, EXT2_OFFSET + 2, sizeof(sblk) / 512);

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

    ata_read(block_group_desc_table, EXT2_OFFSET + 2 + 1 * sectors_per_block, sizeof(struct block_group_desc) * block_groups / 512);

    read_inode(ROOT_INODE);
}

void read_inode(u32 ino)
{
    // find which block group the inode belongs to
    int bg = (ino - 1) / sblk.inodes_per_group;

    // block group descriptor corresponding to the group the inodes belongs to
    struct block_group_desc bgd = block_group_desc_table[bg];

    // read inode table for this block group into memory
    struct inode *inode_table = kmalloc(sizeof(struct inode) * sblk.inodes_per_group);
    ata_read(inode_table, EXT2_OFFSET + bgd.inode_table * sectors_per_block, sizeof(struct inode) * sblk.inodes_per_group / 512);

    // index of inode in inode table (NOTE - inode index starts at 1)
    int index = (ino - 1) % sblk.inodes_per_group;

    struct inode *node = &inode_table[index];

    // buffer to hold block
    u8 buff[BLOCK_SIZE];
    ata_read(buff, EXT2_OFFSET + node->block_ptr[0] * sectors_per_block, sizeof(buff) / 512);

    // inode is a directory
    if (node->mode & INODE_MODE_DIR)
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
    else if (node->mode & INODE_MODE_REG)
    {
        kprintf("Reading inode of regular file %d\n", node->size);
    }

    // release resources
    kfree(inode_table);
}
