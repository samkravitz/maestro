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
	int buf_pos;
	int buf_end;
	char buf[1024];
} DIR;

struct dirent
{
	int d_ino;
	int d_reclen;
	char d_name[];
};

DIR *opendir(const char *name);
struct dirent *readdir(DIR *);

#endif    // DIRENT_H
