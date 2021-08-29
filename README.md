# maestro

## My symphony in C.

### Requirements
- [qemu](https://www.qemu.org/) (specifically qemu-system-i386)
- [nasm](https://nasm.us/)
- [grub](https://www.gnu.org/software/grub/)
- An i686 cross-compiler (on x64 Linux machines, this should be achievable with some gcc flags)

### Usage
build: \
`make` \
create disk image: (this only needs to be done once) \
`make disk` \
launch: \
`make start`

### Notes
I have ported the Doug Lea's memory allocator [dlmalloc](gee.cs.oswego.edu/dl/html/malloc.html) (public domain) to focus on different things in the OS. I plan on coming back and implementing my own memory allocator in due time.