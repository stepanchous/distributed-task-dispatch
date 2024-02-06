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

ADD ../build/client/src/client /app/client
ADD ../ASTs /app/ASTs

WORKDIR /app

EXPOSE 5300

ENTRYPOINT ./client /app/ASTs/example3.json
