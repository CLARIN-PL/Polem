FROM ubuntu:18.04 as polem-base

RUN apt-get update && apt-get install -y locales && locale-gen en_US.UTF-8

#### installing required packages
RUN apt-get update && apt-get -y upgrade
RUN DEBIAN_FRONTEND="noninteractive" apt-get -y install vim libboost-all-dev libicu-dev git-core wget cmake \
                                                libantlr-dev libloki-dev python-dev swig \
                                                libxml2-dev libsigc++-2.0-dev libglibmm-2.4-dev libxml++2.6-dev

#### downloading prerequisities...
RUN mkdir /build
WORKDIR /build
RUN git clone http://nlp.pwr.wroc.pl/corpus2.git
RUN git clone http://nlp.pwr.edu.pl/wccl.git
RUN wget -O morfeusz2-2.0.0-Linux-amd64.deb https://nextcloud.clarin-pl.eu/index.php/s/VVIvx4w20azcWbp/download
RUN dpkg -i morfeusz2-2.0.0-Linux-amd64.deb
#### ... and building them

# corpus2
RUN cd corpus2
RUN mkdir bin
WORKDIR /build/corpus2/bin
RUN cmake ..
RUN make -j
RUN make -j
RUN make install
RUN ldconfig

# wccl
RUN mkdir /build/wccl/bin
WORKDIR /build/wccl/bin
RUN cmake ..
RUN make -j
RUN make -j
RUN make install
RUN ldconfig

RUN apt-get update && apt-get install -y openjdk-8-jdk

# Install cmake 3.16 to use FindPython2 and FindPython3
RUN apt remove -y cmake && \
    wget https://github.com/Kitware/CMake/releases/download/v3.16.0-rc2/cmake-3.16.0-rc2-Linux-x86_64.tar.gz && \
    tar -xzf cmake*tar.gz && \
    ln -s $(pwd)/cmake*/bin/cmake /usr/bin/cmake


#----------------------------
# Now build image with Polem
#----------------------------

FROM polem-base

ENV LANG=C.UTF-8

# Compile Polem from sources
WORKDIR /build/polem
COPY . /build/polem

RUN mkdir -p build && \
    cd build && \
    cmake .. && \
    make -j && \
    make install && \
    ldconfig && \
    cd / && rm -r /tmp/*

WORKDIR /build/polem
#CMD ["python3.6", "polem.py"]