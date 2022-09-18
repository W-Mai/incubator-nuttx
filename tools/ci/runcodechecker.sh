#!/usr/bin/env bash
# tools/ci/runcodechecker.sh
#
# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.  The
# ASF licenses this file to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance with the
# License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations
# under the License.
#

CWD=$(cd "$(dirname "$0")" && pwd)
WS=$(cd "${CWD}"/../../../ && pwd -P)
CODECHECKERPID=${WS}/codechecker.pid.o

WORKSPACE=~/.codechecker

SERVERSCHM=http
SERVERHOST=0.0.0.0
SERVERPORT=8001
SERVERADDR=${SERVERHOST}:${SERVERPORT}
SERVERURL=${SERVERSCHM}://${SERVERADDR}

NEEDSTART=false
NEEDKILL=false
NEEDCOMPRESS=false
NEEDRM=false

function health_check {
  tries=1
  until wget --spider -q ${SERVERURL} ;  do
    sleep 2
    echo Waiting for CodeChecker Webserver start...\(tries ${tries} times\)
    let tries++
  done
}

function start_server {
  CodeChecker server --host ${SERVERHOST} -v ${SERVERPORT} -w ${WORKSPACE} > /dev/null  & echo $! > ${CODECHECKERPID}

  # Health check and block the code.
  health_check

  # If has product name, add new product to CodeChecker Server.
  if [ "${product_name}" != "" ]; then
    add_products ${product_name}
  fi 
}

function kill_server {
  pid_=`cat ${CODECHECKERPID}`
  kill "${pid_}"
  rm ${CODECHECKERPID}

  echo "CodeChecker Webserver has been killed."
}

function add_products {
  # Health check and block the code.
  health_check
  CodeChecker cmd products add $1 --url ${SERVERURL}
}

function compress_database {
  path=`dirname ${WORKSPACE}`
  pushd $path
  local name=${WORKSPACE#${path}/}

  tar zcf ${name}.tar.gz ${name}

  if $1; then
    rm -rf $WORKSPACE
  fi

  popd
}

while [ ! -z "$1" ]; do
  case $1 in
  -k )
    NEEDKILL=true
    ;;
  -n )
    shift
    product_name="${1}"
    ;;
  -s )
    NEEDSTART=true
    ;;
  -w )
    shift
    WORKSPACE="${1}"
    ;;
  -c )
    NEEDCOMPRESS=true
    ;;
  --rm )
    NEEDRM=true
    ;;
  * )
    shift
    break
    ;;
  esac
  shift
done

if $NEEDSTART; then
  start_server
fi

if $NEEDKILL; then
  kill_server
fi

if $NEEDCOMPRESS; then
  compress_database $NEEDRM
fi