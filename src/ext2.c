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
#include <kprintf.h>

#include "string.h"

struct superblock_t superblock;

// block group descriptor table
struct block_group_desc *bgdt;

// number of block groups in volume
static int block_groups;

static int alloc_inode();
static int alloc_block();
static void print_inode(u32);
static void print_superblock();
static struct inode_t read_inode(u32);
static void write_inode(struct inode_t *, u32);
static int inode_from_path(char *);
static int parent_inode_from_path(char *);

/**
 * macro to get the name from a dir entry
 * param entry struct ext2_dir_entry *
 * return char *
 */
#define DIRENT_NAME(entry) ((char *) ((u8 *) entry + EXT2_DIRENT_NAME_OFFSET))

/**
 * macros to manipulate block/inode bitmaps
 * pass in buffer to bitmap and index to be worked with
 */
#define BMAP_SET(map, bit)   (map[bit / 8] |= (1 << (bit % 8)))
#define BMAP_CLEAR(map, bit) (map[bit / 8] &= ~(1 << (bit % 8)))
#define BMAP_TEST(map, bit)  (map[bit / 8] & (1 << (bit % 8)))

/**
 * reads ext2 filesystem block(s) from disk
 * @param buff buffer of at least BLOCK_SIZE bytes to read into
 * @param blk index of block in filesystem to read
 * @param n number of blocks to read
 */
static inline void read_block(void *buff, uint blk, int n)
{
	ata_read(buff, blk * EXT2_SECTORS_PER_BLOCK, n * EXT2_SECTORS_PER_BLOCK);
}

/**
 * reads ext2 filesystem block(s) from disk
 * @param buff buffer of at least BLOCK_SIZE bytes to read into
 * @param blk index of block in filesystem to read
 * @param n number of blocks to read
 */
static inline void write_block(void *buff, uint blk, int n)
{
	ata_write(buff, blk * EXT2_SECTORS_PER_BLOCK, n * EXT2_SECTORS_PER_BLOCK);
}

/**
 * writes the block group descriptor table to disk
 */
static inline void flush_block_group_descriptor_table()
{
	write_block(bgdt, EXT2_BLOCK_DESCRIPTOR, get_num_blocks(block_groups * sizeof(struct block_group_desc)));
}

/**
 * writes the superblock to disk
 */
static inline void flush_superblock()
{
	write_block(&superblock, EXT2_SUPERBLOCK, get_num_blocks(sizeof(superblock)));
}

void ext2_init()
{
	read_block(&superblock, EXT2_SUPERBLOCK, get_num_blocks(sizeof(superblock)));

	int inodes     = superblock.inode_count;
	int blocks     = superblock.block_count;
	int block_size = 1024 << superblock.log_block_size;

	/*
    *   determine number of block groups
    *   as a sanity check, use both blocks and inodes to calculate
    *   round up the total number of inodes / num inodes per group
    *   round up the total number of blocks / num blocks per group
    */
	int block_groups_inode = inodes * 1.0f / superblock.inodes_per_group + .5f;
	int block_groups_block = blocks * 1.0f / superblock.blocks_per_group + .5f;

	if (block_groups_inode != block_groups_block)
		kprintf("ext2: inconsistency in block group count\n");

	// set global block groups to calculates number of block groups
	// this could be block_groups_inode or block_groups_block, they are equivalent
	block_groups           = block_groups_inode;

	// allocate space for block group descriptor table
	bgdt = kmalloc(sizeof(struct block_group_desc) * block_groups);

    // read block group descriptor table into memory
    read_block(bgdt, EXT2_BLOCK_DESCRIPTOR, get_num_blocks(sizeof(struct block_group_desc) * block_groups));

	// print_inode(ROOT_INODE);
}

/**
 * @brief prints an inode to the screen
 * @param ino index of which inode to print
 */
void print_inode(u32 ino)
{
	struct inode_t inode = read_inode(ino);

	// buffer to hold block
	u8 buff[BLOCK_SIZE];
	read_block(buff, inode.block_ptr[0], 1);

	// inode is a directory
	if (inode.mode & INODE_MODE_DIR)
	{
		struct ext2_dir_entry *entry = (struct ext2_dir_entry *) buff;
		uint bytes_read = 0;
		do
		{
			char *ptr  = (char *) entry;
			char *name = ptr + 8;
			kprintf("{ .inode: %d, .rec_len: %d, .name_len: %d, .name: %s }\n",
			        entry->inode,
			        entry->rec_len,
			        entry->name_len,
			        name);
			bytes_read += entry->rec_len;
			ptr += entry->rec_len;
			entry = (struct ext2_dir_entry *) ptr;
		} while (bytes_read < BLOCK_SIZE);    // directory entries must fit in size of 1 block
	}

	// inode is a regular file
	else if (inode.mode & INODE_MODE_REG)
	{
		kprintf("Reading inode of regular file %d\n", inode.size);
	}
}

/**
 * @brief allocates an unused inode and updates the bgdt's inode bitmap
 * @return id of the free inode or error if none is found
 */
static int alloc_inode()
{
	if (superblock.free_block_count == 0)
		return EXT2_ALLOC_ERROR;

	for (int i = 0; i < block_groups; ++i)
	{
		struct block_group_desc *bgd = &bgdt[i];

		// no free inodes in this block group
		if (bgd->free_inode_count <= 0)
			continue;

		// get this group's inode bitmap
		u8 buff[BLOCK_SIZE];
		read_block(buff, bgd->inode_bitmap, 1);

		for (int n = 0; n < superblock.inodes_per_group / 8; ++n)
		{
			if (!BMAP_TEST(buff, n))
			{
				// inode indeces start at 1, this is why we add 1 to the free bit we found
				u32 index = i * superblock.inodes_per_group + n + 1;
				bgd->free_inode_count--;
				superblock.free_inode_count--;
				BMAP_SET(buff, n);
				write_block(buff, bgd->inode_bitmap, 1);
				flush_block_group_descriptor_table();
				flush_superblock();
				return index;
			}
		}

		// should never get here
		kprintf("EXT2 alloc_inode: something has gone terribly wrong!\n");
	}

	// no free inode could be found
	return EXT2_ALLOC_ERROR;
}

/**
 * @brief allocates an unused block and updates the bgdt's block bitmap
 * @return id of the block inode or error if none is found
 */
static int alloc_block()
{
	if (superblock.free_block_count == 0)
		return EXT2_ALLOC_ERROR;

	for (int i = 0; i < block_groups; ++i)
	{
		struct block_group_desc *bgd = &bgdt[i];

		// no free inodes in this block group
		if (bgd->free_block_count <= 0)
			continue;

		// get this group's inode bitmap
		u8 buff[BLOCK_SIZE];
		read_block(buff, bgd->block_bitmap, 1);

		for (int n = 0; n < superblock.blocks_per_group / 8; ++n)
		{
			if (!BMAP_TEST(buff, n))
			{
				u32 index = i * superblock.blocks_per_group + n;
				bgd->free_block_count--;
				superblock.free_block_count--;
				BMAP_SET(buff, n);
				write_block(buff, bgd->block_bitmap, 1);
				flush_block_group_descriptor_table();
				flush_superblock();
				return index;
			}
		}

		// should never get here
		kprintf("EXT2 alloc_block_id: something has gone terribly wrong!\n");
	}

	// no free block could be found
	return EXT2_ALLOC_ERROR;
}

/**
 * @brief retrieves a given inode
 * @param idx index of the inode to get
 * @return the inode
 */
static struct inode_t read_inode(u32 idx)
{
	// find which block group the inode belongs to
	int bg                      = (idx - 1) / superblock.inodes_per_group;

	// block group descriptor corresponding to the group the inodes belongs to
	struct block_group_desc bgd = bgdt[bg];

	// index of inode in inode table (NOTE - inode index starts at 1)
	int index                   = (idx - 1) % superblock.inodes_per_group;
	int offset_within_block     = index % (BLOCK_SIZE / sizeof(struct inode_t));

	// read block in inode table for this block group into memory
	u8 buff[BLOCK_SIZE];
	read_block(buff, bgd.inode_table + (index * sizeof(struct inode_t) / BLOCK_SIZE), 1);

    struct inode_t inode;
	memcpy(&inode, &buff[offset_within_block * sizeof(struct inode_t)], sizeof(inode));
	return inode;
}


/**
 * @brief flushes an inode to disk
 * @param inode pointer to inode to write
 * @param idx index of the inode to write
 */
static void write_inode(struct inode_t *inode, u32 idx)
{
	// find which block group the inode belongs to
	int bg                      = (idx - 1) / superblock.inodes_per_group;

	// block group descriptor corresponding to the group the inodes belongs to
	struct block_group_desc bgd = bgdt[bg];

	// index of inode in inode table (NOTE - inode index starts at 1)
	int index                   = (idx - 1) % superblock.inodes_per_group;
	int offset_within_block     = index % (BLOCK_SIZE / sizeof(struct inode_t));

	// read block in inode table for this block group into memory
	u8 buff[BLOCK_SIZE];
	read_block(buff, bgd.inode_table + (index * sizeof(struct inode_t) / BLOCK_SIZE), 1);

	memcpy(&buff[offset_within_block * sizeof(struct inode_t)], inode, sizeof(struct inode_t));
	write_block(buff, bgd.inode_table + (index * sizeof(struct inode_t) / BLOCK_SIZE), 1);
}

/**
 * @brief creates a new, empty ext2 directory
 * 
 * This function will overwrite an existing file at path,
 * it is the caller's responsibility to ensure the directory to
 * make is a valid one.
 * 
 * @param path absolute path of directory to create
 * @return inode id of the created directory, or error
 */
int ext2_mkdir(const char *path)
{
	int inode_idx = alloc_inode();
	int block_idx = alloc_block();

	// unsuccessful at finding a free block or free inode
	if (inode_idx == EXT2_ALLOC_ERROR || block_idx == EXT2_ALLOC_ERROR)
	{
		kprintf("ext2_mkdir: no free inode or block\n");
		return EXT2_MKDIR_ERROR;
	}

	// inode representing the new directory
	struct inode_t dir;
	memset(&dir, 0, sizeof(dir));

	dir.mode |= INODE_MODE_DIR;
	dir.block_ptr[0] = block_idx;
	dir.links_count  = 1;
	dir.size         = BLOCK_SIZE;

	write_inode(&dir, inode_idx);

	// get the inode of the parent (the inode we are placing our new directory in)
    int parent_idx = parent_inode_from_path(path);
    struct inode_t parent = read_inode(parent_idx);

	// buffer to parent's dir entries
	u8 buff[BLOCK_SIZE];
	read_block(buff, parent.block_ptr[0], 1);

    // create . and .. entries for new directory
    char dotbuff[BLOCK_SIZE];
    struct ext2_dir_entry dot = {
        .inode    = inode_idx,
        .rec_len  = 12,
        .name_len = 1,
        .type     = DIR_TYPE_DIR,
    };

    struct ext2_dir_entry dotdot = {
        .inode    = parent_idx,
        .rec_len  = 1024 - 12,
        .name_len = 2,
        .type     = DIR_TYPE_DIR,
    };
    
    // copy . and .. entries to buffer
    memcpy(&dotbuff[0], &dot, sizeof(dot));
    memcpy(&dotbuff[12], &dotdot, sizeof(dotdot));

    // copy . and .. names to buffer
    strcpy((char*)  &dotbuff[0  + EXT2_DIRENT_NAME_OFFSET], ".");
    strcpy((char *) &dotbuff[12 + EXT2_DIRENT_NAME_OFFSET], "..");

    // write . and .. entries to disk
    write_block(dotbuff, block_idx, 1);

    // insert dir into its parent's entries

    // create entry for new directory
    char *dirname = strrchr(path, '/') + 1; // relative path from parent's perspective
    struct ext2_dir_entry new_dir_entry = {
        .inode    = inode_idx,
        .rec_len  = round(EXT2_DIRENT_NAME_OFFSET + strlen(dirname), 4),
        .name_len = strlen(dirname),
        .type     = DIR_TYPE_DIR,
    };

    // TODO - handle when directory entries are larger than a single block
    if (parent.size > BLOCK_SIZE)
    {
        kprintf("ext2_mkdir: parent size is %d\n bytes", parent.size);
        return EXT2_MKDIR_ERROR;
    }

	struct ext2_dir_entry *entry = (struct ext2_dir_entry *) buff;
	uint bytes_read = 0;

	while (1)
	{
        // entry now points to the final entry in the block
		if (bytes_read + entry->rec_len == BLOCK_SIZE)
		{
			// adjust previously last entry's length
			entry->rec_len = round(EXT2_DIRENT_NAME_OFFSET + entry->name_len, 4);

            // new entry can fit in this block
            if (bytes_read + entry->rec_len + new_dir_entry.rec_len <= BLOCK_SIZE)
            {
                // adjust new entry's length so it fills up entire block
                new_dir_entry.rec_len = BLOCK_SIZE - bytes_read - entry->rec_len;

                // copy our updated records to buffer
                memcpy(&buff[bytes_read + entry->rec_len], &new_dir_entry, sizeof(new_dir_entry));
                strcpy((char *) &buff[bytes_read + entry->rec_len + 8], dirname);
                break;
            }

            // TODO - handle the case when we'd need to put our new entry in a different block
            else
            {
                kprintf("ext2_mkdir: %s needs to go into another parent block\n", dirname);
                return EXT2_MKDIR_ERROR;
            }
		}

		bytes_read += entry->rec_len;
		entry = (struct ext2_dir_entry *) ((u8 *) entry + entry->rec_len);
	}

    // write parent's new entries to disk
	write_block(buff, parent.block_ptr[0], 1);

    return inode_idx;
}

/**
 * @brief creates a new, empty ext2 file
 * 
 * This function will overwrite an existing file at path,
 * it is the caller's responsibility to ensure the file to
 * create is a valid one.
 * 
 * @param path absolute path of file to create
 * @return inode id of the created file, or error
 */
int ext2_touch(const char *path)
{
	int inode_idx = alloc_inode();

	// unsuccessful at finding a free block or free inode
	if (inode_idx == EXT2_ALLOC_ERROR)
	{
		kprintf("ext2_touch: no free inode\n");
		return EXT2_TOUCH_ERROR;
	}

	// inode representing the new file
	struct inode_t file;
	memset(&file, 0, sizeof(file));

	file.mode |= INODE_MODE_REG;
	file.links_count  = 1;
	file.size         = 0;

	write_inode(&file, inode_idx);

	// get the inode of the parent (the inode we are placing our new file in)
    int parent_idx = parent_inode_from_path(path);
    struct inode_t parent = read_inode(parent_idx);

	// buffer to parent's dir entries
	u8 buff[BLOCK_SIZE];
	read_block(buff, parent.block_ptr[0], 1);

    // insert file into its parent's entries

    // create entry for new directory
    char *filename = strrchr(path, '/') + 1; // relative path from parent's perspective
    struct ext2_dir_entry new_dir_entry = {
        .inode    = inode_idx,
        .rec_len  = round(EXT2_DIRENT_NAME_OFFSET + strlen(filename), 4),
        .name_len = strlen(filename),
        .type     = DIR_TYPE_DIR,
    };

    // TODO - handle when directory entries are larger than a single block
    if (parent.size > BLOCK_SIZE)
    {
        kprintf("ext2_touch: parent size is %d\n bytes", parent.size);
        return EXT2_TOUCH_ERROR;
    }

	struct ext2_dir_entry *entry = (struct ext2_dir_entry *) buff;
	uint bytes_read = 0;

	while (1)
	{
        // entry now points to the final entry in the block
		if (bytes_read + entry->rec_len == BLOCK_SIZE)
		{
			// adjust previously last entry's length
			entry->rec_len = round(EXT2_DIRENT_NAME_OFFSET + entry->name_len, 4);

            // new entry can fit in this block
            if (bytes_read + entry->rec_len + new_dir_entry.rec_len <= BLOCK_SIZE)
            {
                // adjust new entry's length so it fills up entire block
                new_dir_entry.rec_len = BLOCK_SIZE - bytes_read - entry->rec_len;

                // copy our updated records to buffer
                memcpy(&buff[bytes_read + entry->rec_len], &new_dir_entry, sizeof(new_dir_entry));
                strcpy((char *) &buff[bytes_read + entry->rec_len + 8], filename);
                break;
            }

            // TODO - handle the case when we'd need to put our new entry in a different block
            else
            {
                kprintf("ext2_touch: %s needs to go into another parent block\n", filename);
                return EXT2_TOUCH_ERROR;
            }
		}

		bytes_read += entry->rec_len;
		entry = (struct ext2_dir_entry *) ((u8 *) entry + entry->rec_len);
	}

    // write parent's new entries to disk
	write_block(buff, parent.block_ptr[0], 1);

    return inode_idx;
}

/**
 * @brief finds the inode number associated with a given path
 * @param path absolute path of file to find
 * @return inode index of the file or error if it doesn't exist
 */
static int inode_from_path(char *path)
{
    // tokenize path into its segments
    // e.x. given the path "/home/user/bin/a.out"
    // segment will eventually be "home", "user", "bin", "a.out"
    char *segment = strtok(path, "/");
    int ino;

    // current directory we're looking in
    struct inode_t dir = read_inode(ROOT_INODE);

    // block to hold directory entries
    u8 buff[BLOCK_SIZE];

    do
    {
        // read entries of current directory
        read_block(buff, dir.block_ptr[0], 1);

        // search for tok within directory entries
        uint bytes_read = 0;
        struct ext2_dir_entry *entry = (struct ext2_dir_entry *) buff;

        do
        {
            if (strcmp(segment, DIRENT_NAME(entry)) == 0)
            {
                ino = entry->inode;
                
                // when the segment was found, skip to label 
                // so we know we have found each segment
                goto found_segment;
            }

            bytes_read += entry->rec_len;

            // offset to next entry
            entry = (struct ext2_dir_entry *) (buff + bytes_read);
        } while (bytes_read < BLOCK_SIZE);

        // if the loop breaks normally, that means we didn't find the 
        // segment we were looking for. so, short circuit
        return EXT2_INODE_NOTFOUND;

        found_segment:
        // load the entries of the next segment
        dir = read_inode(ino);

    } while ((segment = strtok(NULL, "/")) != NULL);
    
    return ino;
}

/**
 * @brief gets the inode of the parent of a given file
 * @param path absolute path of the file to get the parent of
 * @return inode index of the file's parent
 * 
 * e.x. parent_inode_from_path("/path/to/a/nested/file");
 * will return the inode index of the directory "nested"
 */
static int parent_inode_from_path(char *path)
{
    // pointer to final occurrance of / in path
    char *last_slash = strrchr(path, '/');

    // if the final occurance of / is also the first, the parent is the root directory
    if (last_slash == path)
        return ROOT_INODE;

    // temporarily null terminate path after the parent we are looking for
    *last_slash = '\0';
    int ino = inode_from_path(path);
    *last_slash = '/';

    return ino;
}

static void print_superblock()
{
	kprintf("Inode count:             %d\n", superblock.inode_count);
	kprintf("Block count:             %d\n", superblock.block_count);
	kprintf("Reserved blocks:         %d\n", superblock.reserved_block_count);
	kprintf("Free blocks:             %d\n", superblock.free_block_count);
	kprintf("Free inodes:             %d\n", superblock.free_inode_count);
	kprintf("First data block:        %d\n", superblock.first_data_block);
	kprintf("Block size:              %d\n", 1024 << superblock.log_block_size);
	kprintf("Fragment size:           %d\n", 1024 << superblock.log_frag_size);
	kprintf("Blocks per group:        %d\n", superblock.blocks_per_group);
	kprintf("Fragments per group:     %d\n", superblock.frags_per_group);
	kprintf("First inode:             %d\n", superblock.first_ino);
	kprintf("Inode size:              %d\n", superblock.inode_size);
	kprintf("Filesystem magic number: %x\n", superblock.magic);
}
