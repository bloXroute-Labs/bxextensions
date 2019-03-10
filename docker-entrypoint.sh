#!/bin/sh
set -e

# this script will start either main_api_server.py or main_socket_server.py depending on the first arg.

PROGNAME=$(basename $0)
USER="bxextensions"
GROUP="bxextensions"

cd /app/bxextensions
cp *.so build/alpine-3.8.1/
