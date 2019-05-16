#!/bin/sh
set -e
cd /app/bxextensions
echo "building binaries"
python3 build_extensions.py --src-dir /app/bxextensions --package-installation
echo "copying binaries"
cp -r ./*.so release/
