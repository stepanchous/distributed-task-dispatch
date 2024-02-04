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
    spdlog-devel

ADD ../build/manager/src/manager /app/manager

WORKDIR /app

EXPOSE 5300
EXPOSE 5559

ENTRYPOINT ./manager

