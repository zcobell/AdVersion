#!/bin/bash

INSTALL_DIR=/opt/libgit2

git submodule update --remote --init

cd libgit2
mkdir build
cd build
cmake .. -G"Unix Makefiles" -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR
make
sudo make install
