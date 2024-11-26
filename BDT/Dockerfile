# Base image
FROM nvidia/cuda:12.4.0-devel-ubuntu22.04

# Set the working directory inside the container
WORKDIR /usr/src/app

# Install necessary dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    autoconf \
    cmake \
    git \
    g++ \
    make \
    libboost-all-dev

# Clone the accelerated OpenFHE from UNIMAN
WORKDIR /usr/src/app
RUN git clone https://github.com/beehive-lab/openfhe-uniman.git
WORKDIR openfhe-uniman
RUN git switch gpu-cuda
RUN which nvcc

# Build and install OpenFHE
#WORKDIR /usr/src/app/openfhe-development
RUN mkdir build && cd build && cmake \
-DWITH_CUDA=ON -DCUDA_PATH=/usr/local/cuda -DCUDA_TOOLKIT_ROOT_DIR=/usr/local/cuda -DCMAKE_CUDA_COMPILER=/usr/local/cuda/bin/nvcc .. \
&& make \
&& make install 

#&& git checkout tags/v1.1.2

# Set the library path
ENV LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

# Command to run the compiled binary (replace 'your_binary_name' with the actual binary name)
ENV TERM xterm-256color
RUN echo 'export PS1="\[\e[36m\](docker@\h) \[\e[1;33m\][\[\e[38;5;130m\]\$(date +%H:%M:%S)\[\e[38;5;167m\] \u:\[\e[38;5;228m\] \w\[\e[1;33m\]]\[\e[m\]\n$ "' > /root/.bashrc

# Get and update the CMakeLists file from the library before compilation
#RUN cp /usr/src/app/openfhe-development/CMakeLists.User.txt ./CMakeLists.txt
#RUN echo "add_executable(bdt-hom-eval bdt-hom-eval.cpp)" >> CMakeLists.txt

# Add all context files and directories before compilation
WORKDIR /bdt
ADD . .

# Get and update the CMakeLists file from the library before compilation
WORKDIR /bdt/BDTClient
RUN cp /usr/src/app/openfhe-uniman/CMakeLists.User.txt ./CMakeLists.txt
RUN echo "find_package(Threads REQUIRED)" >> CMakeLists.txt
RUN echo "add_executable(bdt_client_prefinal bdt_client_prefinal.cpp)" >> CMakeLists.txt

WORKDIR /bdt/BDTServer
RUN cp /usr/src/app/openfhe-uniman/CMakeLists.User.txt ./CMakeLists.txt
RUN echo "find_package(Threads REQUIRED)" >> CMakeLists.txt
RUN echo "add_executable(bdt-hom-eval bdt-hom-eval.cpp)" >> CMakeLists.txt

WORKDIR /bdt/BDTResult
RUN cp /usr/src/app/openfhe-uniman/CMakeLists.User.txt ./CMakeLists.txt
RUN echo "find_package(Threads REQUIRED)" >> CMakeLists.txt
RUN echo "add_executable(bdt_result_prefinal bdt_result_prefinal.cpp)" >> CMakeLists.txt

#Build
WORKDIR /bdt/BDTClient/build
RUN cmake .. && make && mkdir demoData
WORKDIR /bdt/BDTServer/build
RUN cmake .. && make && mkdir demoData
WORKDIR /bdt/BDTResult/build
RUN cmake .. && make && mkdir demoData

#WORKDIR /bdt/build
#RUN cmake .. && make
#RUN ./bdt-hom-eval 3

#Working directory
WORKDIR /bdt

CMD ["bash", "-l"] 





