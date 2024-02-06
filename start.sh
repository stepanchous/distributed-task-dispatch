#!/bin/bash

set -e

if [[ ! -d "build" ]]; then
    mkdir build
fi

cmake -B build -S ./ -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j6

if [[ -e "database/database.db" ]]; then
    rm database/database.db
fi

protoc --proto_path=proto --python_out=database db.proto

docker build -t worker -f ./worker/Dockerfile .
docker build -t broker -f ./broker/Dockerfile .
docker build -t manager -f ./manager/Dockerfile .
docker build -t client -f ./client/Dockerfile .
docker build -t db_server -f ./database/Dockerfile .

docker-compose up --scale client=20

wait
