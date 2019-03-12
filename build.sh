#!/usr/bin/env bash
echo "Cleaning container..."
docker container rm bxextensions
echo "Building container..."
docker build . -f Dockerfile -t bxextensions
echo "Running container..."
### docker run -it bxextensions sh
mkdir -p output/x86_64-linux-gnu
docker run -d --name bxextensions \
	--volume ~/bloxroute-dev/bxextensions/output/x86_64-linux-gnu:/app/bxextensions/output/ \
	bxextensions 
sudo chown $USER:$GROUP output/x86_64-linux-gnu/*.so
