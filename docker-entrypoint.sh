#!/bin/sh
set -e
cd /app/bxextensions
echo "copying binaries"
cp -rf *.so release/
