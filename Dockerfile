FROM ubuntu:22.04

# Set noninteractive frontend for tzdata
RUN DEBIAN_FRONTEND=noninteractive apt-get update && apt-get install -y tzdata

# Install development tools and pigpio library
RUN apt-get install -y \
    vim \
    build-essential \
    git \
    cmake \
    net-tools \
    gdb \
    clang \
    libpigpio-dev

# Set working directory
WORKDIR /work
