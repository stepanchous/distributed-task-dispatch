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
COPY ./database/setup_database.py /app/setup_database.py
COPY ./database/db_pb2.py /app/db_pb2.py

WORKDIR /app

RUN python setup_database.py

EXPOSE 5301

ENTRYPOINT ./db_server



