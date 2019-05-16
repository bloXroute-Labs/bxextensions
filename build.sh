#!/usr/bin/env bash
docker container rm bxextensions
set -e
OS_RELEASE=${1:-alpine-3.8}
echo "Cleaning container..."
mkdir -p release/${OS_RELEASE}
rm -f release/${OS_RELEASE}/*
ROOT_DIR=$(pwd)
rm -f ${ROOT_DIR}/*.so
rm -f ${ROOT_DIR}/*.dylib

DOCKER_FILE="Dockerfile-$OS_RELEASE"

echo "Building container from $DOCKER_FILE"
docker build -f ${DOCKER_FILE} -t bxextensions --build-arg UID=$(id -u) --build-arg GID=$(id -g) .

echo "running docker container"
docker run --name bxextensions \
	--volume ${ROOT_DIR}/release/${OS_RELEASE}:/app/bxextensions/release \
	--user $(id -u):$(id -g) \
	bxextensions
