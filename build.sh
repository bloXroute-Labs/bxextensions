#!/usr/bin/env bash

# ACTION -
#
# build - build locally (DEFAULT)
# deploy - build and push to s3
# pull - pull specific TAG or commit hash from S3
# copy - copy prebuilt extensions release TAG into release folder
# push - push prebuilt extensions release TAG into S3
# push_and_tag - copy current release folder to S3 (use by set release)
#
# TAG - use to specify a specific TAG, DEFAULT current commit hash

ACTION="${ACTION:-build}" # build / deploy / pull / copy / push
OS_LIST=${1:-alpine-3.11}
PARALLELISM=${2:-True}
RETURN_CODE=0
PIDS=""
BUILD_CONTEXT="."
TAG="${TAG:-$(git rev-parse HEAD)}"

echo "running build extensions with: ACTION: $ACTION TAG: ${TAG} SYSTEMS: $OS_LIST"
if [ "${ACTION}" == "build" ] || [ "${ACTION}" == "deploy" ]; then
  echo "creating release tag folder in release_tag/${TAG}"
  mkdir -p release_tag/${TAG}
  for os in $OS_LIST
    do
      DOCKER_FILE="Dockerfile-${os}"
      mkdir -p build/${os}
      mkdir -p release_tag/${TAG}/${os}
      echo "Building container from $DOCKER_FILE"
      bash_cmd="docker stop bxextensions_${os}; \\
            docker rm -f bxextensions_${os}; \\
            DOCKER_BUILDKIT=1 docker build -f ${DOCKER_FILE} -t bxextensions_${os} --build-arg UID=$(id -u) --build-arg GID=$(id -g) . ;  \\
            docker run --name bxextensions_${os} \\
               --volume $(pwd)/release_tag/${TAG}/${os}:/app/bxextensions/release \\
               --volume $(pwd)/build/${os}:/app/bxextensions/build/local \\
               --volume $(pwd)/interface:/app/bxextensions/interface \\
               --volume $(pwd)/lib:/app/bxextensions/lib \\
               --volume $(pwd)/include:/app/bxextensions/include \\
               --user $(id -u):$(id -g) \\
               --env bxextensions_os=${os} \\
               bxextensions_${os}"
      echo ${bash_cmd}
      if [ ${PARALLELISM} == "True" ]; then
        sh -c "${bash_cmd}" &
        PID=$!
        echo "docker run of ${os} process id ${PID}"
        PIDS="$PIDS $!"
      else
        sh -c "${bash_cmd}"
      fi
    done
    for pid in $PIDS
    do
      wait $pid
      status="$?" || true
      if [ "$status" != "0" ]; then
        echo "process $pid ended with status $status"
        RETURN_CODE=$status
      fi
    done
    echo "$(git rev-parse HEAD)" > release_tag/${TAG}/COMMIT_HASH
    cp release/MANIFEST.MF release_tag/${TAG}
elif [ "${ACTION}" == "pull" ]; then
  aws s3 cp s3://files.bloxroute.com/bxextensions/"${TAG}" release_tag/"${TAG}" --recursive
  if [ ! -d release_tag/"${TAG}" ] || [[ -z `ls -A release_tag/"${TAG}"` ]]; then
    echo "tag not found in S3 exists."
    exit 1
  else
    rm -rf release
    cp -rf release_tag/"${TAG}" release
  fi
fi

if [ "${ACTION}" == "push" ] || [ "${ACTION}" == "deploy" ]; then
  aws s3 cp release_tag/"${TAG}" s3://files.bloxroute.com/bxextensions/"${TAG}" --recursive
fi

if [ "${ACTION}" == "push_as_tag" ] ; then
  aws s3 cp release/ s3://files.bloxroute.com/bxextensions/"${TAG}" --recursive
fi

if [ "${ACTION}" == "copy" ] || [ "${ACTION}" == "deploy" ] || [ "${ACTION}" == "build" ]; then
  rm -rf release
  cp -rf release_tag/"${TAG}" release
fi

exit $RETURN_CODE
