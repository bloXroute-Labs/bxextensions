#!/usr/bin/env bash
OS_RELEASE=${1:-alpine-3.8.1}
echo "Cleaning container..."
mkdir -p release/${OS_RELEASE}
rm -f release/${OS_RELEASE}/*
docker container rm bxextensions
ROOT_DIR=$(pwd)
rm -f ${ROOT_DIR}/*.so
rm -f ${ROOT_DIR}/*.dylib
echo "Building container..."
docker build . -f Dockerfile -t bxextensions
### docker run -it bxextensions sh
echo "running docker container"
docker run --name bxextensions \
	--volume ${ROOT_DIR}/release/${OS_RELEASE}:/app/bxextensions/release \
	bxextensions 
echo "changing release binaries permissions"
chown $USER:$GROUP release/${OS_RELEASE}/*.so
