FROM fedora:latest

RUN dnf update -y 
RUN dnf install -y \
    make \
    cmake \
    g++ \
    boost-devel \
    protobuf-compiler \
    grpc-devel

ADD ./src /app/src

WORKDIR /app/src/build

RUN cmake .. -DCMAKE_BUILD_TYPE=Release && \
    cmake --build .

ENTRYPOINT ./client /app/src/ASTs/example2.json
