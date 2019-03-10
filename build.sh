#!/usr/bin/env bash
echo "Building container..."
docker build . -f Dockerfile -t bxextensions
echo "Cleaning container..."
docker container rm bxextensions
echo "Running container..."
### docker run -it bxextensions sh
## docker run -d --name bxextensions \
##	--volume ~/bloxroute-dev/bxextensions/build/alpine-3.8.1:/app/bxextensions/build/alpine-3.8.1 \
##	bxextensions 
## sudo chown $USER:$GROUP build/alpine-3.8.1/*.so
