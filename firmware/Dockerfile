FROM gcc:14.2.0-bookworm

RUN apt-get update && \
    apt-get install -y software-properties-common python3-launchpadlib && \
    apt-get install -y git-all git python3 wget

 
RUN wget https://github.com/Kitware/CMake/archive/refs/tags/v3.31.5.tar.gz
RUN tar -zxvf v3.31.5.tar.gz && \
    cd CMake-3.31.5 && \
    ./bootstrap && \
    make install

WORKDIR /app
COPY  ../../ .
RUN git submodule update --init --recursive

ENTRYPOINT [ "make" ]
CMD ["PROJECT=Demo/Blink", "PLATFORM=cli", "build"]
