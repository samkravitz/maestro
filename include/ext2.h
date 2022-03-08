/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: ext2.h
 * DATE: September 8, 2021
 * DESCRIPTION: ext2 filesystem driver
 * RESOURCES: http://www.nongnu.org/ext2-doc/ext2.html
 */
#ifndef EXT2_H
#define EXT2_H

#include <maestro.h>

// block that contains superblock
#define EXT2_SUPERBLOCK        1

// block that contains block group descriptor table
#define EXT2_BLOCK_DESCRIPTOR  (EXT2_SUPERBLOCK + 1)

// inode of root directory
#define ROOT_INODE             2

// size of ext2 block in bytes
// this corresponds to -b argument to mkfs.ext2 in meta/make_disk.sh
// ext2_init will perform a sanity check just to make sure
#define BLOCK_SIZE             1024

// given a number of bytes, convert to how many ext2 blocks that is
#define get_num_blocks(bytes)      (bytes / BLOCK_SIZE)

// number of 512 byte disk sectors in an ext2 block
#define EXT2_SECTORS_PER_BLOCK (BLOCK_SIZE / 512)

#define EXT2_ALLOC_ERROR       0xffffffff
#define EXT2_INODE_INVALID     0xffffffff

// rounds an integer x up to the nearest multiple of to
#define round(x, to)           ((x + to - 1) & (-to))

struct superblock_t
{
	// base superblock fields
	u32 inode_count;
	u32 block_count;
	u32 reserved_block_count;
	u32 free_block_count;
	u32 free_inode_count;
	u32 first_data_block;    // block number of block containing the superblock
	u32 log_block_size;      // log2 (block size) -10 (the number to shift 1,024 to the left by to obtain the block size)
	u32 log_frag_size;       // log2 (fragment size) -10
	u32 blocks_per_group;    // blocks per block group
	u32 frags_per_group;
	u32 inodes_per_group;
	u32 mtime;               // last mount time (POSIX time)
	u32 wtime;               // last write time
	u16 mnt_count;           // how many times volume was mounted before it was verified
	u16 max_mount_count;
	u16 magic;               // ext2 signature - 0xef53
	u16 state;
	u16 errors;
	u16 version_minor;
	u32 lastcheck;
	u32 checkinterval;       // maximum UNIX time interval allowed between checks
	u32 creator_os;
	u32 version_major;
	u16 uid;                 // user id that can use reserved blocks
	u16 gid;                 // group id that can use reserved blocks

	// extended superblock fields
	u32 first_ino;           // first non-reserved inode
	u16 inode_size;          // inode size in bytes
	u16 block_group_no;      // block group that this superblock is part of
	u32 optional_features;
	u32 required_features;
	u32 ro_features;         // features that if not supported, volume must be read-only
	u8 fsid[16];             // file system id (what is output but blkid)
	char vname[16];          // volume name
	char pname[64];          // path volume was last mounted to
	u32 algo_bitmap;
	u8 prealloc_blocks;
	u8 prealloc_dir_blocks;
	u16 unused;
	u8 jid[16];              // journal ID
	u32 jinum;               // journal inode
	u32 jdev;                // journal device
	u32 last_orphan;
	u8 rsvd[1024 - 236];
} __attribute__((packed));

struct block_group_desc
{
	u32 block_bitmap;    // block id of the first block of the block bitmap for group represented
	u32 inode_bitmap;    // block id of the first block of the inode bitmap for group represented
	u32 inode_table;     // block id of the first block of the inode table for group represented
	u16 free_block_count;
	u16 free_inode_count;
	u16 used_dir_count;
	u16 pad;
	u8 rsvd[12];
} __attribute__((packed));

struct inode_t
{
	u16 mode;                // indicates the format of the file and access rights
	u16 uid;                 // user id associated with file
	u32 size;                // file size in bytes
	u32 atime;               // seconds since 1/1/1970 of last access
	u32 ctime;               // seconds since 1/1/1970 of creation
	u32 mtime;               // seconds since 1/1/1970 of modification
	u32 dtime;               // seconds since 1/1/1970 of deletion
	u16 gid;                 // group id having access to file
	u16 links_count;         // how many times this inode is linked
	u32 blocks;              // number of disk sectors reserved to contain data
	u32 flags;               // flags
	u32 osd1;                // OS specific value
	u32 block_ptr[12];       // pointers to the blocks containing data for this inode
	u32 singly_block_ptr;    // points to a block that is a list of block pointers to data
	u32 double_block_ptr;    // points to a block that is a list of block pointers to Singly Indirect Blocks
	u32 triply_block_ptr;    // points to a block that is a list of block pointers to Doubly Indirect Blocks
	u32 generation;          // indicates the file version (used by NFS)
	u32 file_acl;            // extended attributes (always 0 in revision 0)
	u32 dir_acl;             // extended attributes (always 0 in revision 0)
	u32 faddr;               // location of file fragment
	u8 osd2[12];             // OS specific value

    // inode mode values
    #define INODE_MODE_SOCK       0xc000        // socket
    #define INODE_MODE_LINK       0xa000        // symbolic link
    #define INODE_MODE_REG        0x8000        // regular file
    #define INODE_MODE_BLKDEV     0x6000        // block device
    #define INODE_MODE_DIR        0x4000        // directory
    #define INODE_MODE_CHRDEV     0x2000        // char device
    #define INODE_MODE_FIFO       0x1000        // fifo
    #define INODE_MODE_SUSR       0x0800        // set user id
    #define INODE_MODE_SGRP       0x0400        // set group id
    #define INODE_MODE_STCKY      0x0200        // sticky bit
    #define INODE_MODE_RUSR       0x0100        // usr read
    #define INODE_MODE_WUSR       0x0080        // usr write
    #define INODE_MODE_XUSR       0x0040        // usr execute
    #define INODE_MODE_RGRP       0x0020        // group read
    #define INODE_MODE_WGRP       0x0010        // group write
    #define INODE_MODE_XGRP       0x0008        // group execute
    #define INODE_MODE_ROTH       0x0004        // others read
    #define INODE_MODE_WOTH       0x0002        // others write
    #define INODE_MODE_XOTH       0x0001        // others execute

    // inode flag values
    #define INODE_FLAG_SECR       0x00000001    // secure deletion
    #define INODE_FLAG_UNRM       0x00000002    // record for undelete
    #define INODE_FLAG_COMPR      0x00000004    // compressed file
    #define INODE_FLAG_SYNC       0x00000008    // synchronous updates
    #define INODE_FLAG_IMMUTABLE  0x00000010    // immutable file
    #define INODE_FLAG_APPEND     0x00000020    // append only
    #define INODE_FLAG_NODUMP     0x00000040    // do not dump/delete file
    #define INODE_FLAG_NOATIME    0x00000080    // do not update atime
    #define INODE_FLAG_DIRTY      0x00000100    // dirty
    #define INODE_FLAG_COMPRBLOCK 0X00000200    // compressed blocks
    #define INODE_FLAG_NOCOMPR    0x00000400    // access raw compressed data
    #define INODE_FLAG_ECOMPR     0x00000800    // compression error
    #define INODE_FLAG_BTREE      0x00001000    // b-tree format directory
    #define INODE_FLAG_INDEX      0x00002000    // hash indexed directory
    #define INODE_FLAG_IMAGIC     0x00004000    // AFS directory
    #define INODE_FLAG_JNLDATA    0x00008000    // journal file data
    #define INODE_FLAG_RESERVED   0x80000000    // reserved for ext2 library
} __attribute__((packed));

struct ext2_dir_entry
{
	u32 inode;      // inode number
	u16 rec_len;    // displacement to the next directory entry
	u8 name_len;
	u8 type;

    // name goes here
    // char *name;

    // type values
    #define DIR_TYPE_UNKNOWN 0
    #define DIR_TYPE_REG     1
    #define DIR_TYPE_DIR     2
    #define DIR_TYPE_CHRDEV  3
    #define DIR_TYPE_BLKDEV  4
    #define DIR_TYPE_FIFO    5
    #define DIR_TYPE_SOCK    6
    #define DIR_TYPE_SYMLINK 7
} __attribute__((packed));

void ext2_init();
void ext2_mkdir(const char *);

#endif    // EXT2_H
