/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: user/ls_rust/src/syscall.rs
 * DATE: December 17th, 2025
 * DESCRIPTION: Rust syscall wrappers for Maestro OS
 */

#![allow(dead_code)]

// Syscall numbers
const SYS_READ: u32 = 0;
const SYS_WRITE: u32 = 1;
const SYS_EXIT: u32 = 2;
const SYS_OPEN: u32 = 3;
const SYS_SBRK: u32 = 4;
const SYS_GETDENTS: u32 = 5;
const SYS_FORK: u32 = 6;
const SYS_EXECV: u32 = 7;
const SYS_CLOSE: u32 = 8;
const SYS_GETENV: u32 = 9;
const SYS_WAITPID: u32 = 10;

#[inline(always)]
unsafe fn syscall1(num: u32, arg1: u32) -> i32 {
    let ret: i32;
    core::arch::asm!(
        "int 0x80",
        in("eax") num,
        in("ebx") arg1,
        lateout("eax") ret,
        options(nostack)
    );
    ret
}

#[inline(always)]
unsafe fn syscall3(num: u32, arg1: u32, arg2: u32, arg3: u32) -> i32 {
    let ret: i32;
    core::arch::asm!(
        "int 0x80",
        in("eax") num,
        in("ebx") arg1,
        in("ecx") arg2,
        in("edx") arg3,
        lateout("eax") ret,
        options(nostack)
    );
    ret
}

pub fn write(fd: i32, buf: &[u8]) -> i32 {
    unsafe { syscall3(SYS_WRITE, fd as u32, buf.as_ptr() as u32, buf.len() as u32) }
}

// Export exit with C linkage for crt0.o
#[no_mangle]
pub extern "C" fn exit(code: i32) -> ! {
    unsafe { syscall1(SYS_EXIT, code as u32) };
    loop {}
}

// Open file with null-terminated C string
pub fn open_cstr(path: *const u8) -> i32 {
    unsafe { syscall1(SYS_OPEN, path as u32) }
}

pub fn close(fd: i32) -> i32 {
    unsafe { syscall1(SYS_CLOSE, fd as u32) }
}

pub fn getdents(fd: i32, buf: &mut [u8]) -> i32 {
    unsafe { syscall3(SYS_GETDENTS, fd as u32, buf.as_mut_ptr() as u32, buf.len() as u32) }
}
