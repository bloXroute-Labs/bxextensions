#!/usr/bin/env bash

OS_LIST=${1:-alpine-3.11}
PARALLELISM=${2:-Yes}
DOCKER_SO_FILES=${3:-Upload}
RETURN_CODE=0
PIDS=""


if [ ${DOCKER_SO_FILES} == "Get" ]; then
  docker rm bxextensions_release;
  docker pull 033969152235.dkr.ecr.us-east-1.amazonaws.com/bxextensions:release
  docker run  --name bxextensions_release \
      --volume $(pwd):/app/bxextensions/out_release \
      --user $(id -u):$(id -g) \
      033969152235.dkr.ecr.us-east-1.amazonaws.com/bxextensions:release
else
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
    bash_cmd="docker build --rm=false -f ${DOCKER_FILE} -t bxextensions_${os} --build-arg UID=$(id -u) --build-arg GID=$(id -g) . ;  \
              docker run --name bxextensions_${os} \
                 --volume ${ROOT_DIR}/release/${os}:/app/bxextensions/release/lib \
                 --volume ${ROOT_DIR}/release/MANIFEST.MF:/app/bxextensions/release/MANIFEST.MF:ro \
                 --user $(id -u):$(id -g) \
                 bxextensions_${os}"
    echo ${bash_cmd}
    if [ ${PARALLELISM} == "Yes" ]; then
      sh -c "${bash_cmd}" &
      PID=$!
      echo "docker run of $os process id ${PID}"
      PIDS="$PIDS $!"
    else
      sh -c "${bash_cmd}"
    fi
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

  if [ ${DOCKER_SO_FILES} == "Upload" ]; then
    docker build --rm=false -f Dockerfile-extensions-release -t 033969152235.dkr.ecr.us-east-1.amazonaws.com/bxextensions:release --build-arg UID=$(id -u) --build-arg GID=$(id -g) .
    docker push 033969152235.dkr.ecr.us-east-1.amazonaws.com/bxextensions:release
  fi
fi
exit $RETURN_CODE
