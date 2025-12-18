TARGET_JSON = i686-maestro.json
TARGET = i686-maestro
CARGO = cargo +nightly
CRT0 = ../../lib/libc/bin/crt0.o
LINKER_SCRIPT = linker.ld

all: ls

ls: src/main.rs src/syscall.rs $(CRT0)
	RUSTFLAGS="-C link-arg=$(CRT0) -C relocation-model=static" \
	$(CARGO) build --release --target $(TARGET_JSON) -Z build-std=core,compiler_builtins -Z build-std-features=compiler-builtins-mem
	cp target/$(TARGET)/release/ls_rust.elf ls
	e2cp ls ../../disk.img:/

clean:
	$(CARGO) clean
	rm -f ls

.PHONY: all clean
