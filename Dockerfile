# stage 1: build
FROM ubuntu:24.04 AS builder

# install dependencies
COPY packages.txt /tmp/packages.txt
RUN apt-get update \
    && xargs -a /tmp/packages.txt apt-get install -y \
    && rm -rf /var/lib/apt/lists/*

# set workdir and copy project
WORKDIR /app
COPY . .

# move to build and run the build script
WORKDIR /app/build
RUN chmod +x build.sh \
    && ./build.sh

# stage 2: output binary
FROM ubuntu:24.04
WORKDIR /workspace

# copy the binary from the builder stage
COPY --from=builder /app/build/lounge .

# set default command just in case
CMD ["./lounge"]
