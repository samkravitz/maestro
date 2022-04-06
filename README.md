# maestro

## My symphony in C.

### Requirements
- [qemu](https://www.qemu.org/) (specifically qemu-system-i386)
- [nasm](https://nasm.us/)
- [e2tools](https://github.com/e2tools/e2tools)
- At least 512M of space for the disk image

### Usage
build toolchain (this only needs to be done once): \
`make toolchain` \
create disk image (this only needs to be done once): \
`make disk` \
build: \
`make` \
launch: \
`make start`
