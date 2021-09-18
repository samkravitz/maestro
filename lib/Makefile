all: libs

libs:
	$(MAKE) -C libc
	$(MAKE) -C mxx

.PHONY: clean
clean:
	$(MAKE) -C libc clean
	$(MAKE) -C mxx clean
