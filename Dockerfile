FROM ubuntu:xenial
#### installing required packages
RUN apt-get update && apt-get -y upgrade
RUN apt-get -y install libboost-all-dev libicu-dev git-core wget cmake libantlr-dev libloki-dev python-dev swig libxml2-dev libsigc++-2.0-dev libglibmm-2.4-dev libxml++2.6-dev
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
# installing polem
RUN apt-get -y install default-jdk
RUN mkdir -p /build/polem/build
COPY . /build/polem
WORKDIR /build/polem/build
RUN cmake ..
RUN make
RUN make install
RUN mkdir /data
RUN mv /build/polem/corpus /data
RUN rm -rf /build
WORKDIR /data
ENV LANG=pl_PL.utf-8
CMD ["polem", "-p", "corpus/kpwr-1.1-fixed-keywords-test.txt"]