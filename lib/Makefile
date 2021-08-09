all: libs

libs:
	$(MAKE) -C libc
	$(MAKE) -C libdlmalloc

.PHONY: clean
clean:
	$(MAKE) -C libc clean
	$(MAKE) -C libdlmalloc clean