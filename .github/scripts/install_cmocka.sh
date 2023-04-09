#!/usr/bin/env sh

curl https://cmocka.org/files/1.1/cmocka-1.1.7.tar.xz -o cmocka-1.1.7.tar.xz &&
    tar Jxvf cmocka-1.1.7.tar.xz &&
    cd cmocka-1.1.7 &&
    mkdir build &&
    cd build &&
    cmake -DCMAKE_BUILD_TYPE=Release .. &&
    make &&
    sudo make install
