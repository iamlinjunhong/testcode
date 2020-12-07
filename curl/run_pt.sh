#!/bin/bash

# Copyright (c) 2018 Baidu.com, Inc. All Rights Reserved
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# source shflags from current directory
mydir="${BASH_SOURCE%/*}"
if [[ ! -d "$mydir" ]]; then mydir="$PWD"; fi
. $mydir/shflags

# define command-line flags
DEFINE_integer port 9200 "Port of the first server"
DEFINE_string ip http://192.168.100.194:9200 "elasticsearch ip"
DEFINE_string node mxosrvrs_1_2020-12-07 "mxosrvr node and date"
DEFINE_string ODBC_source traf "odbc source"
DEFINE_string userName db__root "user name"
DEFINE_string password traf123 "user password"
DEFINE_integer node_nums '1' 'Number of servers'

# parse the command-line
FLAGS "$@" || exit 1
eval set -- "${FLAGS_ARGV}"

for ((i=0; i<$FLAGS_node_nums; ++i)); do
     ./pickTrash \
        -ip=${FLAGS_ip}\
        -node=${FLAGS_node} \
        -ODBC_source=${FLAGS_ODBC_source} \
        -userName=${FLAGS_userName} \
        -password=${FLAGS_password} \
        -port=$((${FLAGS_port}+i))
done
