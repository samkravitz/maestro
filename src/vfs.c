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
static struct vfs_node *find(char *);
static struct vfs_node *find_parent(char *);
static struct vfs_node *find_helper(const struct vfs_node *, char *);
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
 * @brief finds the vfs_node associated with a given path
 * @param path absolute path of file to find
 * @return ptr to vfs_node to the file or NULL if it doesn't exist
 */
static struct vfs_node *find(char *path)
{
	// tokenize path into its segments
	// e.x. given the path "/home/user/bin/a.out"
	// segment will eventually be "home", "user", "bin", "a.out"
	char *segment = strtok(path, "/");

	if (!segment)
		return NULL;

	struct vfs_node *node = root;

	do
	{
		node = find_helper(node, segment);
		if (!node)
			return NULL;
	} while ((segment = strtok(NULL, "/")) != NULL);

	return node;
}

/**
 * @brief gets the vfs_node of the parent of a given file
 * @param path absolute path of the file to get the parent of
 * @return ptr to vfs_node of the file's parent or NULL if it doesn't exist
 * 
 * e.x. find_parent("/path/to/a/nested/file");
 * will return the vfs_node of the directory "/path/to/a/nested"
 */
static struct vfs_node *find_parent(char *path)
{
	// pointer to final occurrance of / in path
	char *last_slash = strrchr(path, '/');

	// if the final occurance of / is also the first, the parent is the root directory
	if (last_slash == path)
		return root;

	// temporarily null terminate path after the parent we are looking for
	*last_slash          = '\0';
	struct vfs_node *ret = find(path);
	*last_slash          = '/';

	return ret;
}

static struct vfs_node *find_helper(const struct vfs_node *node, char *name)
{
	struct vfs_node *child = node->leftmost_child;
	while (child)
	{
		if (strcmp(child->name, name) == 0)
			return child;

		child = child->right_sibling;
	}

	return NULL;
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
