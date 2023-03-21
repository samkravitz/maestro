#! /bin/bash

BINUTILS_VERSION=2.38
GCC_VERSION=11.2.0

mkdir toolchain
cd toolchain
wget https://ftp.gnu.org/gnu/binutils/binutils-$BINUTILS_VERSION.tar.xz
wget https://ftp.gnu.org/gnu/gcc/gcc-$GCC_VERSION/gcc-$GCC_VERSION.tar.xz
tar xvf binutils-$BINUTILS_VERSION.tar.xz
tar xvf gcc-$GCC_VERSION.tar.xz

export PREFIX="$HOME/projects/maestro/toolchain/x86_64-elf"
export TARGET=x86_64-elf
export PATH="$PREFIX/bin:$PATH"

mkdir binutils-build
cd binutils-build
../binutils-$BINUTILS_VERSION/configure \
	--target=$TARGET \
	--prefix="$PREFIX" \
	--with-sysroot \
	--disable-nls \
	--disable-werror
make -j8 && make install
cd ..

mkdir gcc-build
cd gcc-build
../gcc-$GCC_VERSION/configure \
	--target=$TARGET \
	--prefix="$PREFIX" \
	--disable-nls \
	--enable-languages=c,c++ \
	--without-headers
make -j8 all-gcc all-target-libgcc
make -j8 install-gcc install-target-libgcc
cd ..

# clean up toolchain directory
#rm -rf binutils-$BINUTILS_VERSION
#rm -rf gcc-$GCC_VERSION
#rm binutils-$BINUTILS_VERSION.tar.xz
#rm gcc-$GCC_VERSION.tar.xz