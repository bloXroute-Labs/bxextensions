#!/usr/bin/env bash

set -e

OS_RELEASE=${1:-alpine-3.8}

if (( $(docker container ls -a | grep -c "bxextensions_${OS_RELEASE} ") > 0 )); then
    echo "removing bxextensions_${OS_RELEASE} container"
    docker container rm bxextensions_${OS_RELEASE}
else
    echo "building bxextensions_${OS_RELEASE} container for the first time"
fi
mkdir -p release/${OS_RELEASE}
mkdir -p build/${OS_RELEASE}
rm -f release/${OS_RELEASE}/*
ROOT_DIR=$(pwd)
rm -f ${ROOT_DIR}/*.so
rm -f ${ROOT_DIR}/*.dylib

DOCKER_FILE="Dockerfile-$OS_RELEASE"

echo "Building container from $DOCKER_FILE"
docker build -f ${DOCKER_FILE} -t bxextensions_${OS_RELEASE} --build-arg UID=$(id -u) --build-arg GID=$(id -g) .

echo "running docker container"
docker run --name bxextensions_${OS_RELEASE} \
	--volume ${ROOT_DIR}/release/${OS_RELEASE}:/app/bxextensions/release \
	--volume ${ROOT_DIR}/build/${OS_RELEASE}:/app/bxextensions/build \
	--user $(id -u):$(id -g) \
	bxextensions_${OS_RELEASE}
