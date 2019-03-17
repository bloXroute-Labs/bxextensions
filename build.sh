#!/usr/bin/env bash
OS_RELEASE=${1:-alpine-3.8.1}
echo "Cleaning container..."
mkdir -p release/${OS_RELEASE}
rm -f release/${OS_RELEASE}/*
docker container rm bxextensions
echo "Building container..."
docker build . -f Dockerfile -t bxextensions
echo "Running container..."
### docker run -it bxextensions sh
ROOT_DIR=$(pwd)
docker run --name bxextensions \
	--volume ${ROOT_DIR}/release/:/app/bxextensions/release/ \
	bxextensions 
sudo chown $USER:$GROUP release/${OS_RELEASE}/*.so
