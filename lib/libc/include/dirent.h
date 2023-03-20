/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: libc/dirent.h
 * DATE: March 20th, 2023
 * DESCRIPTION: directory entry struct
 */

#ifndef DIRENT_H
#define DIRENT_H

typedef struct
{
	int fd;
} DIR;

struct dirent
{
	int d_ino;
	char d_name[256];
};

DIR *opendir(const char *name);

#endif    // DIRENT_H
