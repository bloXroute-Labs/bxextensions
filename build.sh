#!/usr/bin/env bash

OS_LIST=${1:-alpine-3.8}
PIDS=""
STATUS=""
OS_COUNT=1
RETURN_CODE=0

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
  bash -c "docker build -f ${DOCKER_FILE} -t bxextensions_${os} --build-arg UID=$(id -u) --build-arg GID=$(id -g) . ;\
           docker run --name bxextensions_${os} \
              --volume ${ROOT_DIR}/release/${os}:/app/bxextensions/release \
              --volume ${ROOT_DIR}/build/${os}:/app/bxextensions/build \
              --user $(id -u):$(id -g) \
              bxextensions_${os}" &

  PIDS[$OS_COUNT]=$!
  echo "docker run of $os process id; ${PIDS[$OS_COUNT]}"
  ((OS_COUNT+=1))
done
((OS_COUNT-=1))

for os_seq in $(seq 1 $OS_COUNT)
do
  wait ${PIDS[$os_seq]}
  STATUS[$os_seq]="$?"
done

for os_seq in $(seq 1 $OS_COUNT)
do
  if (( ${STATUS[$os_seq]} > 0 )); then
    echo "process ${PIDS[$os_seq]} failed with exit code: ${STATUS[$os_seq]}"
    RETURN_CODE=${STATUS[$os_seq]}
  fi
done

exit $RETURN_CODE
