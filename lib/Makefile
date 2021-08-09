all: libs

libs:
	$(MAKE) -C libc
	$(MAKE) -C liballoc

.PHONY: clean
clean:
	$(MAKE) -C libc clean
	$(MAKE) -C liballoc clean