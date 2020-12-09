#!/bin/bash

DIR="${BASH_SOURCE%/*}"
if [[ ! -d "$DIR" ]]; then DIR="$PWD"; fi

export THIRD_PART_DIR=$DIR/thidpart

#json-0.10.5
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DIR/jsoncpp-0.10.5/libs/linux-gcc-4.8.5
export LIBRARY_PATH=$LIBRARY_PATH:$DIR/jsoncpp-0.10.5/libs/linux-gcc-4.8.5

# log4cplus-1.2.1
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DIR/log4cplus-1.2.1/lib
export LIBRARY_PATH=$LIBRARY_PATH:$DIR/log4cplus-1.2.1/lib

# gflags-2.2.1
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DIR/gflags-2.2.1/lib
export LIBRARY_PATH=$LIBRARY_PATH:$DIR/gflags-2.2.1/lib

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DIR/lib
export LIBRARY_PATH=$LIBRARY_PATH:$DIR/lib
