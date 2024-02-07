FROM fedora:latest

RUN dnf update -y 
RUN dnf install -y \
    make \
    cmake \
    g++ \
    boost-devel \
    protobuf-compiler \
    grpc-devel \
    json-devel \
    zeromq-devel \
    spdlog-devel \
    sqlite-devel \
    python \
    git

RUN dnf install -y pip

RUN pip install protobuf

ADD ./build/database/database-server/db_server /app/db_server
ADD ./database/setup_database.py /app/setup_database.py
ADD ./database/db_pb2.py /app/db_pb2.py
ADD ./database/data.json /app/data.json

WORKDIR /app

RUN python setup_database.py

EXPOSE 5301

ENTRYPOINT ./db_server



