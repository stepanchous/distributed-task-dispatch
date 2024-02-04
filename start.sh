#!/bin/bash

set -e

if [[ ! -d "build" ]]; then
    mkdir build
fi

cmake -B build -S ./
cmake --build build -j6

docker build -t worker -f ./worker/Dockerfile .
docker build -t broker -f ./broker/Dockerfile .

docker-compose up

wait
