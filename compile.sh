#!/bin/bash

cd Desktop/CPProject

rm -rf build

mkdir build

cd build

cmake ..

make -j 4

cd project2

./test2