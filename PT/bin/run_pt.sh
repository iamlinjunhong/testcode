#!/bin/bash

# source shflags from current directory
mydir="${BASH_SOURCE%/*}"
if [[ ! -d "$mydir" ]]; then mydir="$PWD"; fi
. $mydir/shflags

# must to configure
DEFINE_string ip http://192.168.100.194:9200 "elasticsearch ip"
DEFINE_string date 2020-12-09 "mxosrvr node and date"
DEFINE_string ODBC_source traf "odbc source"
DEFINE_string userName db__root "user name"
DEFINE_integer node_nums '1' 'number of db nodes'
DEFINE_string password traf123 "user password"

#default configure
DEFINE_integer lo 1 "Port of the first server"
DEFINE_integer pCount 2 'start 2 threads to connect db for getting statistics'

# parse the command-line
FLAGS "$@" || exit 1
eval set -- "${FLAGS_ARGV}"

for ((i=0; i<$FLAGS_node_nums; ++i)); do
     nohup ./pickTrash \
        -ip=${FLAGS_ip}\
        -date=${FLAGS_date} \
        -ODBC_source=${FLAGS_ODBC_source} \
        -userName=${FLAGS_userName} \
        -password=${FLAGS_password} \
        -lo=$((${FLAGS_lo}+i)) \
        -pCount=${FLAGS_pCount} > std.log 2>&1 &
done
