/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: user/ls_rust/src/main.rs
 * DATE: December 17th, 2025
 * DESCRIPTION: ls - list directory contents (Rust implementation)
 */

#![no_std]
#![no_main]

mod syscall;

use core::slice;

// dirent structure matching C version
#[repr(C)]
struct Dirent {
    d_ino: i32,
    d_reclen: i32,
    // d_name is a flexible array member - we'll access it via pointer arithmetic
}

impl Dirent {
    fn name(&self) -> &str {
        unsafe {
            // d_name starts right after d_reclen
            let name_ptr = (self as *const Self).add(1) as *const u8;

            // Find length of null-terminated string
            let mut len = 0;
            while *name_ptr.add(len) != 0 {
                len += 1;
            }

            // Create string slice
            let name_bytes = slice::from_raw_parts(name_ptr, len);
            core::str::from_utf8_unchecked(name_bytes)
        }
    }
}

// Simple print function
fn print(s: &str) {
    syscall::write(1, s.as_bytes());
}

fn println(s: &str) {
    print(s);
    print("\n");
}

#[panic_handler]
fn panic(_info: &core::panic::PanicInfo) -> ! {
    print("ls: panic!\n");
    unsafe {
        extern "C" {
            fn exit(code: i32) -> !;
        }
        exit(1)
    }
}

#[no_mangle]
pub extern "C" fn main() -> i32 {
    // Note: argc, argv, envp are on the stack but we don't need them for ls

    // Open root directory - need null-terminated C string
    let path = b"/\0";
    let fd = syscall::open_cstr(path.as_ptr());
    if fd < 0 {
        print("ls: cannot open directory /\n");
        return 1;
    }

    // Buffer for getdents
    let mut buf = [0u8; 1024];

    loop {
        let nbytes = syscall::getdents(fd, &mut buf);

        if nbytes <= 0 {
            break;
        }

        // Parse directory entries
        let mut pos = 0;
        while pos < nbytes as usize {
            unsafe {
                let dirent = &*(buf.as_ptr().add(pos) as *const Dirent);
                let name = dirent.name();
                println(name);

                pos += dirent.d_reclen as usize;
            }
        }

        // Check if we've read all entries
        if nbytes < buf.len() as i32 {
            break;
        }
    }

    syscall::close(fd);
    0
}
