#!/bin/sh
set -e
cd /app/bxextensions
if [[ "${bxextensions_os}" == "centos-7" ]]; then
  cd lib/third_party
  # autoreconf --force --install
  cd ../..
fi

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
cp *.so release
