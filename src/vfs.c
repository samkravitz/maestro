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
#include <proc.h>

#include "string.h"

static struct vnode *root = NULL;

static void build_tree(struct vnode *);
static struct vnode *find(char *);
static struct vnode *find_parent(char *);
static struct vnode *find_helper(const struct vnode *, char *);
static void print_tree(struct vnode *, int);

extern struct proc *curr;

static inline bool is_open(int fd)
{
	return curr->ofile[fd] != NULL;
}

static inline void insert_child(struct vnode *parent, struct vnode *child)
{
	if (!parent->leftmost_child)
	{
		parent->leftmost_child = child;
		return;
	}

	struct vnode *tmp = parent->leftmost_child;

	while (tmp->right_sibling)
		tmp = tmp->right_sibling;

	tmp->right_sibling = child;
}

/**
 * @brief builds the node graph structure of the filesystem
 */
void vfs_init()
{
	// allocate root node
	root                 = (struct vnode *) kmalloc(sizeof(struct vnode));
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
 * @brief creates a new, empty filesystem directory
 * 
 * this function calls the ext2 driver to write the created
 * directory to disk
 * 
 * @param path absolute path of the directory to create
 * @return inode id of created directory, or -1 on error
 */
struct vnode *vfs_mkdir(char *path)
{
	struct vnode *parent = find_parent(path);
	if (!parent)
	{
		kprintf("[%s]: parent does not exist!\n", __FUNCTION__);
		return NULL;
	}

	// relative name of the directory
	char *name = strrchr(path, '/') + 1;

	int inode = ext2_mkdir(parent->inode, name);
	if (inode < 0)
	{
		kprintf("Error in ext2_mkdir!\n");
		return NULL;
	}

	// allocate memory for the new directory in the tree
	struct vnode *node = (struct vnode *) kmalloc(sizeof(struct vnode));
	node->inode           = inode;
	node->type            = DIR_TYPE_DIR;
	node->num_children    = 0;
	node->name            = strdup(name);
	node->leftmost_child  = NULL;
	node->right_sibling   = NULL;

	insert_child(parent, node);

	// call this to add . and .. entries to the tree
	build_tree(node);
	return node;
}

/**
 * @brief creates a new, empty filesystem file
 * 
 * this function calls the ext2 driver to write the created
 * file to disk
 * 
 * @param path absolute path of the file to create
 * @return inode id of created file, or -1 on error
 */
struct vnode *vfs_touch(char *path)
{
	struct vnode *parent = find_parent(path);
	if (!parent)
	{
		kprintf("[%s]: parent does not exist!\n", __FUNCTION__);
		return NULL;
	}

	// relative name of the file
	char *name = strrchr(path, '/') + 1;

	int inode = ext2_touch(parent->inode, name);
	if (inode < 0)
	{
		kprintf("Error in ext2_touch!\n");
		return NULL;
	}

	// allocate memory for the new file in the tree
	struct vnode *node = (struct vnode *) kmalloc(sizeof(struct vnode));
	node->inode           = inode;
	node->type            = DIR_TYPE_REG;
	node->num_children    = 0;
	node->name            = strdup(name);
	node->leftmost_child  = NULL;
	node->right_sibling   = NULL;

	insert_child(parent, node);
	return node;
}

/**
 * @brief opens a file in the context of the running process
 * @param path absolute path of the file to open
 * @return fd of opened file or -1 on error
 */
int vfs_open(char *path)
{
	struct vnode *node = find(path);

	if (!node)
	{
		kprintf("vfs_open: %s does not exist\n", path);
		return -1;
	}

	// loop through process's open file table to see if it is able to open more files
	int fd = -1;

	// start at 3 because 0, 1, and 2 are resevered for stdin, stdout, and stderr
	for (int i = 3; i < NOFILE; i++)
	{
		// an entry of NULL means there is a spot for a new open file
		if (!curr->ofile[i])
		{
			fd = i;
			break;
		}
	}

	if (fd == -1)
	{
		kprintf("%s has a full file table!\n", curr->name);
		return -1;
	}

	struct file *f = kmalloc(sizeof(struct file));
	
	f->size = ext2_filesize(node->inode);
	f->pos = 0;
	f->n = node;

	curr->ofile[fd] = f;
	return fd;
}

int vfs_close(int fd)
{
	if (!is_open(fd))
	{
		kprintf("vfs_close: fd %d is not open!\n", fd);
		return -1;
	}

	kfree(curr->ofile[fd]);
	curr->ofile[fd] = NULL;
	return 0;
}

int vfs_seek(int fd, int amt)
{
	if (!is_open(fd))
	{
		kprintf("vfs_seek: fd %d is not open!\n", fd);
		return -1;
	}

	struct file *f = curr->ofile[fd];

	int newpos = (int) f->pos + amt;

	// NOTE - this doesn't handle unsized underflow
	// won't be an issue until pos + amt >= 0x80000000 (2G)

	if (newpos < 0 || newpos > (int) f->size)
	{
		kprintf("vfs_seek: invalid seek amount: pos: %d size: %d amt: %d\n", f->pos, f->size, amt);
		return -1;
	}

	f->pos = newpos;
	return 0;
}

int vfs_read(int fd, void *buff, size_t count)
{
	if (!is_open(fd))
	{
		kprintf("vfs_read: fd %d is not open!\n", fd);
		return -1;
	}

	struct file *f = curr->ofile[fd];

	// TODO - delegate ext2 specific work to a generic fs driver to keep
	// vfs isolated from ext2, in case support for other filesystems is added
	ext2_read_data(buff, f->n->inode, f->pos, count);

	f->pos += count;

	return 0;
}

int vfs_write(int fd, void *buff, size_t count)
{
	if (!is_open(fd))
	{
		kprintf("vfs_write: fd %d is not open!\n", fd);
		return -1;
	}

	struct file *f = curr->ofile[fd];

	// TODO - delegate ext2 specific work to a generic fs driver to keep
	// vfs isolated from ext2, in case support for other filesystems is added
	ext2_write_data(buff, f->n->inode, f->pos, count);

	f->pos += count;

	return 0;
}


/**
 * @brief finds the vfs_node associated with a given path
 * @param path absolute path of file to find
 * @return ptr to vfs_node to the file or NULL if it doesn't exist
 */
static struct vnode *find(char *path)
{
	// tokenize path into its segments
	// e.x. given the path "/home/user/bin/a.out"
	// segment will eventually be "home", "user", "bin", "a.out"
	char *segment = strtok(path, "/");

	if (!segment)
		return NULL;

	struct vnode *node = root;

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
static struct vnode *find_parent(char *path)
{
	// pointer to final occurrance of / in path
	char *last_slash = strrchr(path, '/');

	// if the final occurance of / is also the first, the parent is the root directory
	if (last_slash == path)
	{
		kprintf("parent is root %s\n", path);
		return root;
	}

	// temporarily null terminate path after the parent we are looking for
	*last_slash          = '\0';
	struct vnode *ret = find(path);
	*last_slash          = '/';

	return ret;
}

static struct vnode *find_helper(const struct vnode *node, char *name)
{
	struct vnode *child = node->leftmost_child;
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
static void build_tree(struct vnode *node)
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
		struct vnode *child = (struct vnode *) kmalloc(sizeof(struct vnode));
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
			struct vnode *tmp = node->leftmost_child;

			while (tmp->right_sibling)
				tmp = tmp->right_sibling;

			tmp->right_sibling = child;
		}
	}

	struct vnode *child = node->leftmost_child;
	while (child)
	{
		build_tree(child);
		child = child->right_sibling;
	}
}

static void print_tree(struct vnode *node, int depth)
{
	for (int i = 0; i < depth * 4; i++)
		kprintf(" ");

	kprintf("%s %d %d %d\n", node->name, node->inode, node->num_children, node->type);

	struct vnode *tmp = node->leftmost_child;
	while (tmp)
	{
		print_tree(tmp, depth + 1);
		tmp = tmp->right_sibling;
	}
}
