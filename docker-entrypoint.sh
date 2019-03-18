#!/bin/sh
set -e
cd /app/bxextensions
echo "copying binaries"
cp -r ./*.so release/
