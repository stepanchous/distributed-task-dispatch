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

ADD ../build/broker/src/broker /app/broker

WORKDIR /app

EXPOSE 5559
EXPOSE 5560
EXPOSE 5561

ENTRYPOINT ./broker

