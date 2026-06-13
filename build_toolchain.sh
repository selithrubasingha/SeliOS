#!/bin/bash
set -e  # Exit immediately if any command fails

echo "Starting cross-compiler build..."

# 1. Set up the environment variables and directories
export PREFIX="$HOME/src/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
mkdir -p $HOME/src/cross-build
cd $HOME/src/cross-build

# 2. Download Binutils and GCC source code
echo "Downloading source code..."
wget -nc https://ftp.gnu.org/gnu/binutils/binutils-2.42.tar.gz
wget -nc https://ftp.gnu.org/gnu/gcc/gcc-13.2.0/gcc-13.2.0.tar.gz
tar -xf binutils-2.42.tar.gz
tar -xf gcc-13.2.0.tar.gz

# 3. Compile and install Binutils (Linker, Assembler tools)
echo "Building Binutils..."
mkdir -p build-binutils
cd build-binutils
../binutils-2.42/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make -j$(nproc)
make install
cd ..

# 4. Compile and install GCC (The actual C Compiler)
echo "Building GCC..."
mkdir -p build-gcc
cd build-gcc
../gcc-13.2.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
make -j$(nproc) all-gcc
make -j$(nproc) all-target-libgcc
make install-gcc
make install-target-libgcc
cd ..

echo "====================================="
echo "✅ CROSS COMPILER BUILD COMPLETE!"
echo "====================================="
