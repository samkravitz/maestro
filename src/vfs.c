/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: vfs.c
 * DATE: March 14, 2022
 * DESCRIPTION: virtual filesystem abstraction
 */
#include <vfs.h>

#include <ext2.h>
#include <kmalloc.h>
#include <kprintf.h>

#include "string.h"

static void build_tree(struct vfs_node *);
static void print_tree(struct vfs_node *, int);

static struct vfs_node *root = NULL;

/**
 * @brief builds the node graph structure of the filesystem
 */
void vfs_init()
{
	// allocate root node
	root                 = (struct vfs_node *) kmalloc(sizeof(struct vfs_node));
	root->inode          = ROOT_INODE;
	root->type           = DIR_TYPE_DIR;
	root->num_children   = 0;
	root->name           = strdup("/");
	root->leftmost_child = NULL;
	root->right_sibling  = NULL;

	build_tree(root);

	(void) print_tree;
}

/**
 * @brief recursively build the vfs tree from the filesystem
 * @param node node in the tree to start on
 */
static void build_tree(struct vfs_node *node)
{
	// only continue to build if this node is a directory
	if (node->type != DIR_TYPE_DIR)
		return;

	// don't build a directory's . and .. entries, it will infinite loop
	if (strcmp(node->name, ".") == 0 || strcmp(node->name, "..") == 0)
		return;

	u8 buff[EXT2_BLOCK_SIZE];
	ext2_readdir(buff, node->inode);

	// allocate memory for each of node's dir entries
	struct ext2_dir_entry *entry = (struct ext2_dir_entry *) buff;
	uint bytes_read = 0;
	while (bytes_read < EXT2_BLOCK_SIZE)
	{
		// allocate memory for this node
		struct vfs_node *child = (struct vfs_node *) kmalloc(sizeof(struct vfs_node));
		child->inode           = entry->inode;
		child->type            = entry->type;
		child->num_children    = 0;
		child->name            = strndup(DIRENT_NAME(entry), entry->name_len);
		child->leftmost_child  = NULL;
		child->right_sibling   = NULL;

		node->num_children++;

		bytes_read += entry->rec_len;
		entry = (struct ext2_dir_entry *) (buff + bytes_read);

		// insert child into node's children list
		if (!node->leftmost_child)
			node->leftmost_child = child;

		else
		{
			struct vfs_node *tmp = node->leftmost_child;

			while (tmp->right_sibling)
				tmp = tmp->right_sibling;

			tmp->right_sibling = child;
		}
	}

	struct vfs_node *child = node->leftmost_child;
	while (child)
	{
		build_tree(child);
		child = child->right_sibling;
	}
}

static void print_tree(struct vfs_node *node, int depth)
{
	for (int i = 0; i < depth * 4; i++)
		kprintf(" ");

	kprintf("%s %d %d %d\n", node->name, node->inode, node->num_children, node->type);

	struct vfs_node *tmp = node->leftmost_child;
	while (tmp)
	{
		print_tree(tmp, depth + 1);
		tmp = tmp->right_sibling;
	}
}
