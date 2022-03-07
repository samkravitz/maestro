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
xor ax, ax                         ; ax = 0
mov ds, ax
mov es, ax

; setup stack
mov ss, ax
mov sp, 0x7c00

; read stage1 extended from disk into memory at lba 1
mov ebx, extended_bootsector
mov ax, 1                          ; read 1 sector
mov cx, 1                          ; starting lba
call read_disk

; read superblock from disk into memory we reserved for it
mov ebx, superblock                ; memory buffer for superblock
mov ax, 2                          ; superblock is 1024 bytes, so we need to read 2 sectors
mov cx, 2                          ; superblock begins at lba 2 (byte-offset 1024) 
call read_disk

; confirm this is a valid ext2 disk image
mov ax, [superblock + 56]          ; ext2 magic number is at offset 56 in superblock
cmp ax, 0xef53                     ; 0xef53 is ext2 magic number
jne invalid_ext2

; our next order of business is to load the stage2 bootloader (/stage2.bin) from disk into memory

; load block group descriptor table (bgdt) into memory
lea ebx, [superblock + 1024]       ; bgdt begins at the block immediately following the superblock
mov ax, 2                          ; bgdt is 1024 bytes, so we need to read 2 sectors
mov cx, 4                          ; bgdt begins at lba 4
call read_disk

; now, load the inode table into memory
; we are only concerned with inode 2 (root inode)
; we will use 0x8000 bytes of memory to hold the inode table
; this means we have access to 0x8000 / 128 = 256 inodes
; a check will be performed to make sure we don't try to access higher than that

; get lba of inode table (block of inode table * 2)
mov ebx, 0x8000                    ; load inode table to address 0x8000
mov ax, 64                         ; read 64 sectors (0x8000 bytes) of inode table
mov cx, [superblock + 1024 + 8]    ; cx = bgdt->inode_table
shl cx, 1                          ; cx = lba of inode table
call read_disk

; inode table is now loaded into memory, so lets get the info for inode 2 (root dir inode)
mov ebx, 0x2000                    ; load root dir entries to address 0x2000
mov ax, 2                          ; read 2 sectors of inode 2
mov cx, [0x8000 + 128 + 40]        ; cx = inode_table[1]->block[0]
shl cx, 1                          ; cx = lba of inode 2
call read_disk

; now that root dir entries have been loaded into memory, we can search for the file "stage2.bin"
mov eax, 0                         ; i = 0

stage2_search:
mov si, stage2_filename 
cmp ax, 1024                       ; if i > 1024, quit searching
je stage2_not_found
mov cx, 10                         ; cx = 10 strlen("stage2.bin")
lea di, [eax + 0x2000]             ; di = &(rootdir_inode + i)
push di
repe cmpsb                         ; strcmp(si, di)
pop di
je .done                           ; if strcmp(si, di) == 0, we found stage2.bin
inc ax                             ; i++
jmp stage2_search

; stage2.bin was found
; ax points to the offset in root dir that points to the first character in the name field
; so we want to subtract past file_type, name_len, and rec_len fields to get to inode field
.done:
sub ax, 8                          ; eax = &stage2->inode
mov eax, dword [0x2000 + eax]      ; eax = stage2->inode
cmp eax, 256                       ; if (inode > 256)
jg inode_out_of_bounds             ; throw err
dec eax                            ; inode indeces start at 1
mov ecx, 128                       ; ecx = sizeof(inode)
mul ecx                            ; eax = eax * ecx

; now we know the inode of stage2.bin, so let's load it into memory
mov cx, [0x8000 + eax + 40]        ; cx = inode_table[stage2]->block[0]
shl cx, 1                          ; cx = lba of stage2
mov ebx, 0x5000                    ; load stage2.bin 0x5000
mov ax, 2                          ; read 2 sectors of stage2.bin
call read_disk

; stage2 is loaded, so let's load the kernel
jmp load_kernel

; routine to read from disk into memory
; params:
;   - ax:  number of sectors to read
;   - ebx: absolute memory address to store data
;   - cx:  starting lba to read from
read_disk:
    pusha
    mov [sector_count], ax
    mov [lba], cx

    ; convert absolute address in ebx to segment:offset
    mov edx, 0
    mov eax, ebx                   ; eax contains absolute address
    mov ecx, 0x10                  ; absolute = 16 * segment + offset
    div ecx                        ; eax = eax / ecx, eax is the segment
    mov [dest_offset], dx          ; edx contains the remainder of the division
    mov [dest_segment], ax

	mov si, packet		           ; address of disk address packet
	mov ah, 0x42                   ; required so BIOS knows we gave it an lba and not chs
	mov dl, 0x80		           ; BIOS drive number
	int 0x13
    popa
	ret

; disk address packet structure - allows us to use lba values for disk reads
align 4
packet:
    size:            db 0x10       ; packet size (16 bytes)
    zero:            db 0          ; always 0
    sector_count:    dw 0          ; # of sectors to transfer (a sector is 512 bytes)
    dest_offset:     dw 0          ; transfer destination offset
    dest_segment:    dw 0          ; transfer destination segment
    lba:             dd 0          ; lba to read from
    unused:          dd 0          ; unused (upper 16 bits of 48-bit starting lba)

times 510 - ($ - $$) db 0          ; pad remaining 510 bytes with zeroes
dw 0xaa55                          ; magic number to indicate this is a bootable sector

; lba 1

extended_bootsector:
load_kernel:

; now that stage2 is loaded, we will load our kernel into memory
mov eax, 0                         ; i = 0

kernel_search:
mov si, kernel_filename 
cmp ax, 1024                       ; if i > 1024, quit searching
je kernel_not_found
mov cx, 11                         ; cx = strlen("maestro.bin")
lea di, [eax + 0x2000]             ; di = &(rootdir_inode + i)
push di
repe cmpsb                         ; strcmp(si, di)
pop di
je .done                           ; if strcmp(si, di) == 0, we found stage2.bin
inc ax                             ; i++
jmp kernel_search

; maestro.bin was found
; ax points to the offset in root dir that points to the first character in the name field
; so we want to subtract past file_type, name_len, and rec_len fields to get to inode field
.done:
sub ax, 8                          ; eax = &kernel->inode
mov eax, dword [0x2000 + eax]      ; eax = kernel->inode
cmp eax, 256                       ; if (inode > 256)
jg inode_out_of_bounds             ; throw err
dec eax                            ; inode indeces start at 1
mov ecx, 128                       ; ecx = sizeof(inode)
mul ecx                            ; eax = eax * ecx
mov ebp, eax                       ; ebp = offset into the inode table for kernel entry

; now we know the inode of stage2.bin, so let's load it into memory
mov ebx, dword [0x8000 + eax + 4]  ; ebx = size of kernel in bytes
add ebx, 1024                      ; add 1 block size to ebx to make sure we read entire kernel

; read first 12 blocks (direct block ptrs) of kernel into memory
mov esi, 0                         ; i = 0

push ebx                           ; save kernel_size on stack

direct_blocks:
cmp esi, 12
je .done
mov cx, [0x8000 + ebp + 40 + 4 * esi]  ; cx = inode_table[kernel]->block[i]
shl cx, 1                              ; cx = lba of kernel->block[i]
mov edx, esi                           ; edx = i
shl edx, 10                            ; edx = i * 1024 (i * sizeof(block))
lea ebx, [0x10000 + edx]               ; load kernel to 0x10000
mov ax, 2                              ; read 2 sectors of kernel
call read_disk
inc esi                                ; i++
jmp direct_blocks

.done:

mov eax, ebp                           ; eax = offset into the inode table for kernel entry 
pop ebp                                ; ebp = kernel_size
sub ebp, 12 * 1024                     ; subtract the size of 12 direct blocks we just loaded

; now, read indirect blocks of kernel into memory
mov cx, [0x8000 + eax + 40 + 48]       ; ecx = inode_table[kernel]->indirect_block
shl cx, 1                              ; cx = lba of inode_table[kernel]->indirect_block
mov ebx, 0x4000                        ; load indirect block to address 0x4000
mov ax, 2                              ; read 2 sectors of inderect block
call read_disk

mov esi, 0                             ; i = 0

indirect_block:
cmp ebp, 1024                          ; while (size_left <= sizeof(block))
jle .done
mov ecx, dword [0x4000 + esi * 4]      ; ecx = kernel->indirect_block[i]
shl ecx, 1                             ; ecx = starting lba of kernel->indirect_block[i]
mov edx, esi                           ; edx = i
shl edx, 10                            ; edx *= 1024 (sizeof block)
lea ebx, [0x10000 + 12 * 1024 + edx]
mov ax, 2                              ; read 2 sectors of kernel
call read_disk
sub ebp, 1024                          ; ebp -= sizeof(block)
inc esi                                ; i++
jmp indirect_block

.done:

jmp 0x5000                             ; jump to stage 2

invalid_ext2:
    mov si, invalid_ext2_msg
    call puts
    jmp $

stage2_not_found:
    mov si, stage2_notfound_msg
    call puts
    jmp $

kernel_not_found:
    mov si, kernel_notfound_msg
    call puts
    jmp $

inode_out_of_bounds:
    mov si, inode_out_of_bounds_msg
    call puts
    jmp $

%include 'puts.inc'

invalid_ext2_msg:        db 'disk is not a valid ext2 image', 0
stage2_filename:         db 'stage2.bin', 0
kernel_filename:         db 'maestro.bin', 0
stage2_notfound_msg:     db 'stage2.bin was not found', 0
kernel_notfound_msg:     db 'maestro.bin was not found', 0
inode_out_of_bounds_msg: db 'inode number out of bounds', 0

times 1020 - ($ - $$) db 0 ; pad remaining bytes with zeroes
dd 0xcafebabe              ; my magic number for debugging purposes

; lba 2
superblock:
