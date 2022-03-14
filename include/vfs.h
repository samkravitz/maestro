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

void vfs_init();

#endif    // VFS_H
