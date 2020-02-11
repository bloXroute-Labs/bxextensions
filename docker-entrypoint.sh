#!/bin/sh
set -e
cd /app/bxextensions

echo "building binaries...${PYTHONS}"
for python_cmd in ${PYTHONS}
do
  echo "*******************"
  echo "*******************"
  echo "... ${python_cmd}" 
  echo "*******************"
  echo "*******************"
  ${python_cmd} build_extensions.py --build-type Debug --src-dir /app/bxextensions --package-installation --no-cache
done
echo "copying binaries"
cp -r ./*.so release/lib/
