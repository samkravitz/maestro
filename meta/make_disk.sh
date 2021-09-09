#! /bin/bash
# maestro
# License: GPLv2
# See LICENSE.txt for full license text
# Author: Sam Kravitz
#
# FILE: meta/make_disk.sh
# DATE: August 31, 2021
# DESCRIPTION: Creates and partitions a bootable ext2 hard drive
# USAGE: Run from maestro's root directory before compiling: sudo meta/make_disk.sh
# NOTE: Requires root access to mount drive to /dev/loopX
# REOURCE: https://thestarman.pcministry.com/asm/mbr/PartTables.htm

dd if=/dev/zero of=disk.img bs=512 count=1048576
losetup /dev/loop0 disk.img
fdisk /dev/loop0 << EOF > /dev/null
o
n
p
1
2048

w
EOF
losetup -o 1048576 /dev/loop1 /dev/loop0
mke2fs -I 128 -b 1024 -q /dev/loop1
mkdir tmp
mount /dev/loop1 ./tmp
grub-install --target=i386-pc --boot-directory=./tmp/boot --modules="biosdisk ext2 part_msdos" /dev/loop0
umount ./tmp
losetup -d /dev/loop1
losetup -d /dev/loop0
rmdir ./tmp
