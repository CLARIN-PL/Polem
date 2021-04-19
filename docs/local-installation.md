Below is an instruction how to install the tool on a Linux.
Polem is provided as a source code package and must be compiled. The build system is based on CMake.
Built system provides executable that is installed to bin folder and shared library libpolem-dev.so that can be used in C/C++.

#### Requirements

Before running the build system, you need to ensure you have the following libraries installed with headers.

* Boost (tested with 1.58)
   ```bash
   sudo apt-get install libboost1.58-all-dev
   ```
* ICU library (tested with 55.1; Ubuntu package libicu-dev)
   ```bash
   sudo apt-get install libicu-dev
   ```
* CMake 3.2 or later
   ```bash
   sudo apt-get install cmake
   ```
* Corpus2
   ```bash
   git clone http://nlp.pwr.wroc.pl/corpus2.git
   cd corpus2
   mkdir bin
   cd bin
   cmake ..
   make -j
   make -j
   sudo make install
   sudo ldconfig
   cd ../..
   ```

* WCCL
  ```bash
  sudo apt-get install -y libantlr-dev
  git clone http://nlp.pwr.edu.pl/wccl.git
  cd wccl
  mkdir bin
  cd bin
  cmake ..
  make -j
  make -j
  sudo make install
  sudo ldconfig
  cd ../..
  ```   

* Morfeusz2 form generator
    ```bash
    wget -O morfeusz2-2.0.0-Linux-amd64.deb https://nextcloud.clarin-pl.eu/index.php/s/VVIvx4w20azcWbp/download
    sudo dpkg -i morfeusz2-2.0.0-Linux-amd64.deb
    ``` 


#### Polem

Having installed the dependencies, install Polem:

```bash
mkdir polem/cpp/build
cd polem/cpp/build
cmake ..
make
sudo make install
```