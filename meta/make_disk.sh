#! /bin/bash
# maestro
# License: GPLv2
# See LICENSE.txt for full license text
# Author: Sam Kravitz
#
# FILE: meta/make_disk.sh
# DATE: August 31, 2021
# DESCRIPTION: Creates and partitions a bootable FAT32 hard drive
# USAGE: Run from maestro's root directory before compiling: meta/make_disk.sh
# NOTE: Requires root access to mount drive to /dev/loopX
# NOTE: Requires sfdisk utility
# REOURCE: https://thestarman.pcministry.com/asm/mbr/PartTables.htm

# get name of directory that script is in
SCRIPT_DIR=`dirname "$0"`

dd if=/dev/zero of=disk.img bs=512 count=1048576
sudo losetup /dev/loop0 disk.img
sudo sfdisk /dev/loop0 < $SCRIPT_DIR/fat32.sfdisk
sudo losetup -o 1048576 /dev/loop1 /dev/loop0
sudo mkfs.fat -F 32 /dev/loop1
mkdir tmp
sudo mount /dev/loop1 ./tmp
sudo grub-install --target=i386-pc --boot-directory=./tmp/boot --modules="biosdisk fat part_msdos" /dev/loop0
sudo umount ./tmp
sudo losetup -d /dev/loop1
sudo losetup -d /dev/loop0
rmdir ./tmp

### create a FAT12 partition using mtools ###
# dd if=/dev/zero of=disk.img count=2048 bs=512
# echo "drive c: file=\"disk.img\" partition=1" > ~/.mtoolsrc
# mpartition -I c:
# mpartition -c -b 63 -l 1985 c:
# mpartition -a c:
# mformat -R 63 c:
# sudo grub-install --target=i386-pc --modules="biosdisk fat part_msdos" disk.img
# mcopy grub.cfg c:/boot/grub
# mcopy maestro.bin c:/boot