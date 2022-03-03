; maestro
; License: GPLv2
; See LICENSE.txt for full license text
; Author: Sam Kravitz
;
; FILE: stage1.s
; DATE: March 3rd, 2022
; DESCRIPTION:
; stage 1 bootloader for maestro. We are restricted to a 512 byte boot sector at the beginning
; of execution so we cannot do much. All this does is load the stage 2 bootloader
; from disk to memory and executes it.
;
; RESOURCES:
; https://github.com/lazear/ext2-boot
; and
; NanobyteOS - overall a great resource for osdev
; https://github.com/chibicitiberiu/nanobyte_os/blob/master/src/bootloader/stage1/boot.asm

[bits 16]
org 0x7c00

; lba 0

; initialize system state that BIOS may or may not have initialized
xor ax, ax                  ; ax = 0
mov ds, ax
mov es, ax

; setup stack
mov ss, ax
mov sp, 0x7c00

; read superblock from disk into memory we reserved for it
mov bx, superblock
mov [buffer], bx            ; memory buffer for superblock
mov ax, 2
mov [sector_count], ax      ; superblock is 1024 bytes, so we need to read 2 sectors
mov [lba], ax               ; superblock begins at lba 2 (byte-offset 1024) 
call read_disk

; confirm this is a valid ext2 disk image
mov ax, [superblock + 56]   ; ext2 magic number is at offset 56 in superblock
cmp ax, 0xef53              ; 0xef53 is ext2 magic number
jne error

; our next order of business is to load the stage2 bootloader (/stage2.bin) from disk into memory

; load block group descriptor table (bgdt) into memory
mov bx, superblock + 1024          ; bgdt begins at the block immediately following the superblock
mov [buffer], bx                   ; where bgdt will be loaded into memory
mov ax, 2
mov [sector_count], ax             ; bgdt is 1024 bytes, so we need to read 2 sectors
mov ax, 4
mov [lba], ax                      ; bgdt begins at lba 4
call read_disk

; now, load part of the inode table into memory
; (we are only concerned with inode 5), so we don't need to read that much

mov cx, [superblock + 1024 + 8]    ; cx = bgdt->inode_table

; get lba of inode table (block of inode table * 2)
shl cx, 1                          ; cx = lba of inode table
mov bx, 0x1000                     ; load inode table to address 0x1000
mov [buffer], bx                   ; where inode table will be loaded into memory
mov ax, 2
mov [sector_count], ax             ; read 2 sectors of inode table
mov [lba], cx                      ; starting lba of inode table
call read_disk

; inode table is now loaded into memory, so lets get the info for inode 5 (bootloader inode)
mov cx, [0x1000 + 4 * 128 + 40]    ; cx = inode_table[4]->block[0]
shl cx, 1                          ; cx = lba of inode 5
mov bx, 0x5000                     ; load stage2.bin to address 0x5000
mov [buffer], bx                   ; where stage2.bin will be loaded into memory
mov ax, 1
mov [sector_count], ax             ; read 1 sectors of inode 5
mov [lba], cx                      ; starting lba of inode 5
call read_disk

jmp 0x5000                         ; jump to stage 2

read_disk:
	mov si, packet		; address of disk address packet
	mov ah, 0x42
	mov dl, 0x80		; BIOS drive number
	int 0x13
	ret

error:
    jmp $

; disk address packet structure - allows us to use lba values for disk reads
align 4
packet:
    size:            db 0x10 ; packet size (16 bytes)
    zero:            db 0    ; always 0
    sector_count:    dw 0    ; # of sectors to transfer (a sector is 512 bytes)
    buffer:          dd 0    ; transfer destination address (0:7c00)
    lba:             dd 0    ; lba to read from
    unused:          dd 0    ; unused (upper 16 bits of 48-bit starting lba)

times 510 - ($ - $$) db 0    ; pad remaining 510 bytes with zeroes
dw 0xaa55                    ; magic number to indicate this is a bootable sector

; lba 1

times 1020 - ($ - $$) db 0 ; pad remaining bytes with zeroes
dd 0xcafebabe              ; my magic number for debugging purposes

; lba 2
superblock:
