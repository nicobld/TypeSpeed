#!/bin/bash
set -e

mkdir -p lib
mkdir -p include
mkdir -p json-c-build/

cd json-c-build
cmake ../json-c/ -DCMAKE_INSTALL_PREFIX=install/ -DBUILD_SHARED_LIBS=OFF
make -j
make -j install
cd ..

mv json-c-build/install/include/json-c include/
mv -f json-c-build/install/lib/libjson-c.a lib/