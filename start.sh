#!/bin/bash

set -e

if [[ ! -d "build" ]]; then
    mkdir build
fi

cmake -B build -S ./
cmake --build build -j6

if [[ -e "database/database.db" ]]; then
    rm database/database.db
fi

protoc --proto_path=proto --python_out=database db.proto

python database/setup_database.py

# docker build -t worker -f ./worker/Dockerfile .
# docker build -t broker -f ./broker/Dockerfile .
# docker build -t manager -f ./manager/Dockerfile .
# docker build -t client -f ./client/Dockerfile .
#
# docker-compose up

wait
