/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: vfs.h
 * DATE: March 14, 2022
 * DESCRIPTION: virtual filesystem abstraction
 */
#ifndef VFS_H
#define VFS_H

#include <maestro.h>

struct vfs_node
{
	u32 inode;
	u8 type;
	char *name;
	size_t num_children;
	struct vfs_node *leftmost_child;
	struct vfs_node *right_sibling;
};

// structure representing an open file from a process's point of view
struct file
{
	size_t size;           // size in bytes
	size_t pos;            // seek offset

	struct vfs_node *n;    // reference to vfs node this open file represents
};

void vfs_init();
struct vfs_node *vfs_mkdir(char *);
struct vfs_node *vfs_touch(char *);

#endif    // VFS_H
