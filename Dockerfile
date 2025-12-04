# Dockerfile — cross-build target: linux/arm (armhf)
FROM --platform=$BUILDPLATFORM ubuntu:22.04

# Avoid interactive tzdata prompts
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=UTC

# Install tools needed to build pigpio and your app
RUN apt-get update && apt-get install -y \
    build-essential \
    git \
    cmake \
    wget \
    vim \
    net-tools \
    gdb \
    clang \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Build pigpio from source (install to system)
RUN git clone https://github.com/joan2937/pigpio.git /tmp/pigpio 
RUN cd /tmp/pigpio
RUN make
RUN make install
RUN ldconfig
RUN rm -rf /tmp/pigpio

# Copy your project into the container
# Adjust the copy path if your sources are in a subfolder (e.g. copy src/)
COPY . /work

# Build your project — example using a simple Makefile or manual compile.
# Replace these commands to match your actual build process.
WORKDIR /work

# Example: if you have a Makefile target "main"
# RUN make main

# Example fallback: compile src/main.c directly (change if your project differs)
# Uncomment and adjust the next line if you build via gcc directly:
# RUN gcc -o /main src/main.c -lpigpio -lrt -pthread

# As a generic approach, run your build script (if present)
# If you use cmake:
# RUN mkdir -p build && cd build && cmake .. && make

# If you have a Makefile that outputs ./main in repo root:
RUN if [ -f Makefile ]; then make main || make; fi

# Final fallback: if /work/main already exists from build, move it to image root
RUN if [ -f /work/main ]; then cp /work/main /main && chmod +x /main; fi

# Image does not need to run anything
CMD ["/bin/true"]
