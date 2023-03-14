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

#include <string.h>

struct superblock_t superblock;

// block group descriptor table
struct block_group_desc *bgdt;

// number of block groups in volume
static int block_groups;

static int alloc_inode();
static int alloc_block();
static struct inode_t read_inode(u32);
static void write_inode(struct inode_t *, u32);
static void print_inode(u32);
static void print_superblock();

static u32 *get_data_blocks(struct inode_t *, u32, u32);

static bool insert_dirent(struct inode_t *, struct ext2_dir_entry *, char *);

#define BITMAP_SET(bitmap, bit)   (bitmap[bit / 8] |=  (1 << (bit % 8)))
#define BITMAP_CLEAR(bitmap, bit) (bitmap[bit / 8] &= ~(1 << (bit % 8)))

static int BITMAP_TEST(u8 *bitmap, int bit)
{
	return bitmap[bit / 8] & (1 << (bit % 8)) ? 1 : 0;
}

/**
 * @brief finds the first set bit of a bitmap
 * @param bitmap a pointer to the bitmap
 * @param max_bits the maximum bit index this bitmap keeps track of
 * @return index of first set bit or -1 if all are clear
 */
static inline int BITMAP_FIRST_SET(u8 *bitmap, int max_bits)
{
	for (int i = 0; i < max_bits / 8; ++i)
	{
		// quickly check if all 8 bits of this u8 are clear to save some work
		if (bitmap[i] == 0)
			continue;

		// at least one of the bits in this u8 are set so find it
		for (int j = 0; j < 8; ++j)
		{
			int bit = i * 8 + j;
			if (BITMAP_TEST(bitmap, bit))
				return bit;
		}
	}

	return -1;
}

/**
 * @brief finds the first clear bit of a bitmap
 * @param bitmap a pointer to the bitmap
 * @param max_bits the maximum bit index this bitmap keeps track of
 * @return index of first clear bit or -1 if all are set
 */
static inline int BITMAP_FIRST_CLEAR(u8 *bitmap, int max_bits)
{
	for (int i = 0; i < max_bits / 8; ++i)
	{
		// quickly check if all 8 bits of this u8 are set to save us some work
		if (bitmap[i] == 0xff)
			continue;

		// at least one of the bits in this u8 are clear so find it
		for (int j = 0; j < 8; ++j)
		{
			int bit = i * 8 + j;
			if (!BITMAP_TEST(bitmap, bit))
				return bit;
		}
	}

	return -1;
}

/**
 * reads ext2 filesystem block(s) from disk
 * @param buff buffer of at least EXT2_BLOCK_SIZE bytes to read into
 * @param blk index of block in filesystem to read
 * @param n number of blocks to read
 */
static inline void read_block(void *buff, uint blk, int n)
{
	ata_read(buff, blk * EXT2_SECTORS_PER_BLOCK, n * EXT2_SECTORS_PER_BLOCK);
}

/**
 * reads ext2 filesystem block(s) from disk
 * @param buff buffer of at least EXT2_BLOCK_SIZE bytes to read into
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
static inline void write_bgdt()
{
	write_block(bgdt, EXT2_BLOCK_DESCRIPTOR, get_num_blocks(block_groups * sizeof(struct block_group_desc)));
}

/**
 * writes the superblock to disk
 */
static inline void write_superblock()
{
	write_block(&superblock, EXT2_SUPERBLOCK, get_num_blocks(sizeof(superblock)));
}

void ext2_init()
{
	read_block(&superblock, EXT2_SUPERBLOCK, get_num_blocks(sizeof(superblock)));

	int inodes     = superblock.inode_count;
	int blocks     = superblock.block_count;
	int block_size = 1024 << superblock.log_block_size;

	// make sure block_size in the superblock is what we expect it to be
	if (block_size != EXT2_BLOCK_SIZE)
		kprintf("ext2_init: block size doesn't match superblock\n");

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
	block_groups = block_groups_inode;

	/*
     * read the block group descriptor table into memory
     * the size of the bgdt is not constant, and does not have to be a multiple of BLOCK_SIZE.
	 * allocate the exact amount of bytes the bgdt requires and then
	 * keep a sepparate buffer which is the closest multiple of BLOCK_SIZE.
	 * then, read an entire BLOCK_SIZE multiple into the buffer and copy only
	 * what is needed to the bgdt
	 */
	size_t bgdt_size = sizeof(struct block_group_desc) * block_groups;
	bgdt = kmalloc(bgdt_size);
	u8 bgdt_blocks = get_num_blocks(bgdt_size);
	if (bgdt_size % EXT2_BLOCK_SIZE != 0)
		bgdt_blocks++;
	
	u8 buff[bgdt_blocks * EXT2_BLOCK_SIZE];
	read_block(buff, EXT2_BLOCK_DESCRIPTOR, bgdt_blocks);
	memcpy(bgdt, buff, bgdt_size);

	(void) print_inode;
	(void) print_superblock;
}

/**
 * @brief prints an inode to the screen
 * @param ino index of which inode to print
 */
void print_inode(u32 ino)
{
	struct inode_t inode = read_inode(ino);

	// buffer to hold block
	u8 buff[EXT2_BLOCK_SIZE];
	read_block(buff, inode.block_ptr[0], 1);

	// inode is a directory
	if (inode.mode & INODE_MODE_DIR)
	{
		struct ext2_dir_entry *entry = (struct ext2_dir_entry *) buff;
		uint bytes_read = 0;
		do
		{
			char *name = strndup(DIRENT_NAME(entry), entry->name_len);
			kprintf("{ .inode: %d, .rec_len: %d, .name_len: %d, .name: %s }\n",
			        entry->inode,
			        entry->rec_len,
			        entry->name_len,
			        name);
			bytes_read += entry->rec_len;
			entry = (struct ext2_dir_entry *) (buff + bytes_read);
		} while (bytes_read < EXT2_BLOCK_SIZE);    // directory entries must fit in size of 1 block
	}

	// inode is not a directory
	else
		kprintf("Reading inode of regular file %d\n", inode.size);
}

/**
 * @brief allocates an unused inode and updates the bgdt's inode bitmap
 * @return id of the free inode or error if none is found
 */
static int alloc_inode()
{
	if (superblock.free_inode_count == 0)
		return EXT2_ALLOC_ERROR;

	for (int i = 0; i < block_groups; ++i)
	{
		struct block_group_desc *bgd = &bgdt[i];

		// no free inodes in this block group
		if (bgd->free_inode_count <= 0)
			continue;

		// get this group's inode bitmap
		u8 buff[EXT2_BLOCK_SIZE];
		read_block(buff, bgd->inode_bitmap, 1);

		int index = BITMAP_FIRST_CLEAR(buff, superblock.inodes_per_group);

		// no free inode could be found
		if (index == -1)
			return EXT2_ALLOC_ERROR;

		bgd->free_inode_count--;
		superblock.free_inode_count--;
		BITMAP_SET(buff, index);
		write_block(buff, bgd->inode_bitmap, 1);
		write_bgdt();
		write_superblock();

		// inode indeces start at 1, so add 1 bc bitmaps start at 0
		return index + 1;
	}

	// should never get here
	kprintf("EXT2 alloc_inode: something has gone terribly wrong!\n");
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

		// no free blocks in this block group
		if (bgd->free_block_count <= 0)
			continue;

		// get this group's block bitmap
		u8 buff[EXT2_BLOCK_SIZE];
		read_block(buff, bgd->block_bitmap, 1);

		int index = BITMAP_FIRST_CLEAR(buff, superblock.blocks_per_group);

		// no free inode could be found
		if (index == -1)
			return EXT2_ALLOC_ERROR;

		bgd->free_block_count--;
		superblock.free_block_count--;
		BITMAP_SET(buff, index);
		write_block(buff, bgd->block_bitmap, 1);
		write_bgdt();
		write_superblock();

		return index;
	}

	// should never get here
	kprintf("EXT2 alloc_block: something has gone terribly wrong!\n");
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
	int bg = (idx - 1) / superblock.inodes_per_group;

	// block group descriptor corresponding to the group the inodes belongs to
	struct block_group_desc bgd = bgdt[bg];

	// index of inode in inode table (NOTE - inode index starts at 1)
	int index = (idx - 1) % superblock.inodes_per_group;
	int offset_within_block = index % (EXT2_BLOCK_SIZE / sizeof(struct inode_t));

	// read block in inode table for this block group into memory
	u8 buff[EXT2_BLOCK_SIZE];
	read_block(buff, bgd.inode_table + (index * sizeof(struct inode_t) / EXT2_BLOCK_SIZE), 1);

	struct inode_t inode;
	memcpy(&inode, &buff[offset_within_block * sizeof(struct inode_t)], sizeof(inode));
	return inode;
}

/**
 * @brief writes an inode to disk
 * @param inode pointer to inode to write
 * @param idx index of the inode to write
 */
static void write_inode(struct inode_t *inode, u32 idx)
{
	// find which block group the inode belongs to
	int bg = (idx - 1) / superblock.inodes_per_group;

	// block group descriptor corresponding to the group the inodes belongs to
	struct block_group_desc bgd = bgdt[bg];

	// index of inode in inode table (NOTE - inode index starts at 1)
	int index = (idx - 1) % superblock.inodes_per_group;
	int offset_within_block = index % (EXT2_BLOCK_SIZE / sizeof(struct inode_t));

	// read block in inode table for this block group into memory
	u8 buff[EXT2_BLOCK_SIZE];
	read_block(buff, bgd.inode_table + (index * sizeof(struct inode_t) / EXT2_BLOCK_SIZE), 1);

	memcpy(&buff[offset_within_block * sizeof(struct inode_t)], inode, sizeof(struct inode_t));
	write_block(buff, bgd.inode_table + (index * sizeof(struct inode_t) / EXT2_BLOCK_SIZE), 1);
}

/**
 * @brief creates a new, empty ext2 directory
 * 
 * This function will overwrite an existing file at path,
 * it is the caller's responsibility to ensure the directory to
 * make is a valid one.
 * 
 * @param pino inode of new directory's parent
 * @param name name of new directory
 * @return inode id of the created directory, or error
 */
int ext2_mkdir(u32 pino, char *name)
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
	dir.size         = EXT2_BLOCK_SIZE;

	write_inode(&dir, inode_idx);

	struct inode_t parent = read_inode(pino);

	// create . and .. entries for new directory
	char dotbuff[EXT2_BLOCK_SIZE];
	struct ext2_dir_entry dot = {
		.inode    = inode_idx,
		.rec_len  = 12,
		.name_len = 1,
		.type     = DIR_TYPE_DIR,
	};

	struct ext2_dir_entry dotdot = {
		.inode    = pino,
		.rec_len  = 1024 - 12,
		.name_len = 2,
		.type     = DIR_TYPE_DIR,
	};

	// copy . and .. entries to buffer
	memcpy(&dotbuff[0], &dot, sizeof(dot));
	memcpy(&dotbuff[12], &dotdot, sizeof(dotdot));

	// copy . and .. names to buffer
	strcpy((char *) &dotbuff[0 + EXT2_DIRENT_NAME_OFFSET], ".");
	strcpy((char *) &dotbuff[12 + EXT2_DIRENT_NAME_OFFSET], "..");

	// write . and .. entries to disk
	write_block(dotbuff, block_idx, 1);

	// create entry for new directory
	struct ext2_dir_entry new_dir_entry = {
		.inode    = inode_idx,
		.rec_len  = round(EXT2_DIRENT_NAME_OFFSET + strlen(name), 4),
		.name_len = strlen(name),
		.type     = DIR_TYPE_DIR,
	};

	// insert entry into its parent's entries
	if (!insert_dirent(&parent, &new_dir_entry, name))
	{
		kprintf("Error in insert_dirent\n");
		return EXT2_MKDIR_ERROR;
	}

	return inode_idx;
}

/**
 * @brief creates a new, empty ext2 file
 * 
 * This function will overwrite an existing file at path,
 * it is the caller's responsibility to ensure the file to
 * create is a valid one.
 * 
 * @param pino inode of new file's parent
 * @param name name of new file
 * @return inode id of the created file, or error
 */
int ext2_touch(u32 pino, char *name)
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
	file.links_count = 1;
	file.size        = 0;

	write_inode(&file, inode_idx);

    struct inode_t parent = read_inode(pino);

	// create entry for new directory
	struct ext2_dir_entry new_dir_entry = {
		.inode    = inode_idx,
		.rec_len  = round(EXT2_DIRENT_NAME_OFFSET + strlen(name), 4),
		.name_len = strlen(name),
		.type     = DIR_TYPE_DIR,
	};

	// insert file into its parent's entries
	if (!insert_dirent(&parent, &new_dir_entry, name))
	{
		kprintf("Error in insert_dirent\n");
		return EXT2_TOUCH_ERROR;
	}

	return inode_idx;
}

/**
 * @brief reads directory entries of a directory into buff
 * @param buff buffer to read into
 * @param ino inode index of directory to read from
 */
void ext2_readdir(u8 *buff, u32 ino)
{
	struct inode_t inode = read_inode(ino);
	read_block(buff, inode.block_ptr[0], 1);
}

/**
 * @brief get a file's size
 * @param inum inode number
 * @return file size in bytes
 */
size_t ext2_filesize(u32 inum)
{
	struct inode_t inode = read_inode(inum);
	return inode.size;
}

/**
 * @brief read from a file's data blocks
 * @param buff buffer to read data into
 * @param inum inode number to read from
 * @param off  byte offset in file to begin reading
 * @param count number of bytes to read
 * @return number of bytes 
 */
int ext2_read_data(void *buff, u32 inum, size_t off, size_t count)
{
	struct inode_t inode = read_inode(inum);

	// inode's starting data block to read from
	u32 start_block = off / EXT2_BLOCK_SIZE;

	// offset within that starting block
	u32 start_offset = off % EXT2_BLOCK_SIZE;

	// number of data blocks to read in total
	u32 num_blocks = (count / EXT2_BLOCK_SIZE) + 1; // always want to read at least 1 block

	// remaining bytes to read
	u32 remaining = count;

	// if not starting at offset 0, we need to take into account that we must read an entire block
	if (start_offset != 0)
		num_blocks++;

	// list of data blocks to read
	u32 *data_blocks = get_data_blocks(&inode, start_block, num_blocks);

	// temporary buffer if reading from a non block-aligned offset
	u8 tmp[EXT2_BLOCK_SIZE];

	// handle case where we're not reading from a block aligned offset
	if (start_offset != 0)
	{
		read_block(tmp, *data_blocks, 1);
		u32 start_count = EXT2_BLOCK_SIZE - start_offset;

		if (start_count > count)
			start_count = count;
		memcpy(buff, tmp + start_offset, start_count);
		data_blocks++;
		buff += start_count;
		remaining -= start_count;
	}

	// read all intermediate, block aligned data blocks
	while (remaining > EXT2_BLOCK_SIZE)
	{
		read_block(buff, *data_blocks, 1);
		remaining -= EXT2_BLOCK_SIZE;
		data_blocks++;
		buff += EXT2_BLOCK_SIZE;
	}

	// finish case where we're not reading from a block aligned offset	
	if (remaining != 0)
	{
		read_block(tmp, *data_blocks, 1);
		memcpy(buff, tmp, remaining);
	}
	
	return count;
}
/**
 * @brief write into a file's data blocks
 * @param buff buffer to write data into
 * @param inum inode number to write from
 * @param off  byte offset in file to begin writing
 * @param count number of bytes to write
 * @return number of bytes 
 */
int ext2_write_data(void *buff, u32 inum, size_t off, size_t count)
{
	return 1;
}

/**
 * @brief calculates the indeces of data blocks of a file
 * 
 * this is a non trivial calculation because of the
 * singly, doubly, and triply indirect block pointers
 * 
 * @param in pointer to inode to get data blocks of
 * @param start nth data block of file to start from
 * @param count number of data blocks to get
 * @return dynamically allocated array where each element n is the start+nth data block of in
 */
static u32 *get_data_blocks(struct inode_t *in, u32 start, u32 count)
{
	u32 *blocks = kmalloc(sizeof(u32) * count);

	// buffers to hold the indirect block pointers, if necessary
	u32 singly[EXT2_BLOCK_SIZE / sizeof(u32)];
	u32 doubly[EXT2_BLOCK_SIZE / sizeof(u32)];
	// u32 triply[EXT2_BLOCK_SIZE / sizeof(u32)];

	// number of blocks the direct, singly, doubly, and triply block pointers manage, respectively
	#define DIRECT_BLOCKS                12
	#define INDIRECT_BLOCKS             256
	#define DOUBLY_INDIRECT_BLOCKS    65536
	#define TRIPLY_INDIRECT_BLOCKS 16777216

	// number of pointers each level (singly, doubly, triply) contains
	#define BLOCKS_IN_INDIRECT_BLOCK (EXT2_BLOCK_SIZE / sizeof(u32))

	for (u32 i = 0; i < count; i++)
	{
		u32 block = start + i;

		// block is in a direct block
		if (block < DIRECT_BLOCKS)
		{
			blocks[i] = in->block_ptr[block];
		}

		// block is in the singly indrect block
		else if (block < DIRECT_BLOCKS + INDIRECT_BLOCKS)
		{
			// read singly blocks into buffer
			read_block(singly, in->singly_block_ptr, 1);

			// get offset into data block
			u32 data_offset = (block - DIRECT_BLOCKS) % block;
			blocks[i] = singly[data_offset];
		}

		// block is in the doubly indirect block
		else if (block < DIRECT_BLOCKS + INDIRECT_BLOCKS + DOUBLY_INDIRECT_BLOCKS)
		{
			// read doubly blocks into buffer
			read_block(doubly, in->double_block_ptr, 1);

			// what index in the doubly block holds the singly block?
			u32 singly_offset = (block - DIRECT_BLOCKS - INDIRECT_BLOCKS) / BLOCKS_IN_INDIRECT_BLOCK;
			
			// read singly blocks into buffer
			read_block(singly, doubly[singly_offset], 1);

			// get index into data block
			u32 data_offset = (block - DIRECT_BLOCKS - INDIRECT_BLOCKS) % BLOCKS_IN_INDIRECT_BLOCK;
			blocks[i] = singly[data_offset];
		}

		// block is in the triply indirect block
		else
		{
			// TODO - implement triply indirect block
			kprintf("get_data_blocks: in a triply indirect block!\n");

			while (1)
				;
		}
	}

	return blocks;
}

/**
 * @brief utility function to insert a new entry into a directory
 * @param parent ptr to inode of the directory we are adding an entry to
 * @param new_ent ptr to new entry to add
 * @param name new_ent's name
 */
static bool insert_dirent(struct inode_t *parent, struct ext2_dir_entry *new_ent, char *name)
{
	// buffer to parent's dir entries
	u8 buff[EXT2_BLOCK_SIZE];
	read_block(buff, parent->block_ptr[0], 1);

	// TODO - handle when directory entries are larger than a single block
	if (parent->size > EXT2_BLOCK_SIZE)
	{
		kprintf("insert_dirent: parent size is %d\n bytes", parent->size);
		return false;
	}

	struct ext2_dir_entry *entry = (struct ext2_dir_entry *) buff;
	uint bytes_read = 0;

	while (1)
	{
		// entry now points to the final entry in the block
		if (bytes_read + entry->rec_len == EXT2_BLOCK_SIZE)
		{
			// adjust previously last entry's length
			entry->rec_len = round(EXT2_DIRENT_NAME_OFFSET + entry->name_len, 4);

			// new entry can fit in this block
			if (bytes_read + entry->rec_len + new_ent->rec_len <= EXT2_BLOCK_SIZE)
			{
				// adjust new entry's length so it fills up entire block
				new_ent->rec_len = EXT2_BLOCK_SIZE - bytes_read - entry->rec_len;

				// copy our updated records to buffer
				memcpy(&buff[bytes_read + entry->rec_len], new_ent, sizeof(*new_ent));
				strncpy((char *) &buff[bytes_read + entry->rec_len + EXT2_DIRENT_NAME_OFFSET], name, new_ent->name_len);
				break;
			}

			// TODO - handle the case when we'd need to put our new entry in a different block
			else
			{
				kprintf("insert_dirent: %s needs to go into another parent block\n", name);
				return false;
			}
		}

		bytes_read += entry->rec_len;
		entry = (struct ext2_dir_entry *) (buff + bytes_read);
	}

	// write parent's new entries to disk
	write_block(buff, parent->block_ptr[0], 1);
	return true;
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
