#!/usr/bin/env bash

OS_LIST=${1:-alpine-3.8}
RETURN_CODE=0
PIDS=""

for os in $OS_LIST
do
  if (( $(docker container ls -a | grep -c "bxextensions_${os} ") > 0 )); then
      echo "removing bxextensions_${os} container"
      docker container rm bxextensions_${os}
  else
      echo "building bxextensions_${os} container for the first time"
  fi
  mkdir -p release/${os}
  mkdir -p build/${os}
  rm -f release/${os}/*
  ROOT_DIR=$(pwd)
  rm -f ${ROOT_DIR}/*.so
  rm -f ${ROOT_DIR}/*.dylib

  DOCKER_FILE="Dockerfile-$os"

  echo "Building container from $DOCKER_FILE"
  bash_cmd="docker build -f ${DOCKER_FILE} -t bxextensions_${os} --build-arg UID=$(id -u) --build-arg GID=$(id -g) . ;  \
            docker run --name bxextensions_${os} \
               --volume ${ROOT_DIR}/release/${os}:/app/bxextensions/release \
               --volume ${ROOT_DIR}/build/${os}:/app/bxextensions/build \
               --user $(id -u):$(id -g) \
               bxextensions_${os}" 
  echo ${bash_cmd}
  sh -c "${bash_cmd}" &
  PID=$!
  echo "docker run of $os process id ${PID}"
  PIDS="$PIDS $!"
done

for pid in $PIDS
do
  wait $pid
  status="$?" || true
  if [ "$status" != "0" ]; then
    echo "process $pid ended with status $status"
    RETURN_CODE=$status
  fi
done

exit $RETURN_CODE
