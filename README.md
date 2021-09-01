# maestro

## My symphony in C.

### Requirements
- [qemu](https://www.qemu.org/) (specifically qemu-system-i386)
- [nasm](https://nasm.us/)
- [grub](https://www.gnu.org/software/grub/)
- An i686 cross-compiler (on x64 Linux machines, this should be achievable with some gcc flags)
- At least 512M of space for the disk image

### Usage
create disk image: (this only needs to be done once) \
(root access is required) \
`meta/make_disk.sh` \
build: \
`make` \
launch: \
`make start`

### Notes
I have ported the Doug Lea's memory allocator [dlmalloc](gee.cs.oswego.edu/dl/html/malloc.html) (public domain) to focus on different things in the OS. I plan on coming back and implementing my own memory allocator in due time.