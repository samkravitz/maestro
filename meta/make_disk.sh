#! /bin/sh
# maestro
# License: GPLv2
# See LICENSE.txt for full license text
# Author: Sam Kravitz
#
# FILE: meta/make_disk.sh
# DATE: August 31, 2021
# DESCRIPTION: Creates an ext2 hard drive image

dd if=/dev/zero of=disk.img bs=1K count=64K
mkfs.ext2 -I 128 -b 1024 -q disk.img
e2mkdir disk.img:/bin
