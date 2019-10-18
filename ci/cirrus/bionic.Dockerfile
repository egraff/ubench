FROM ubuntu:bionic

RUN \
  apt-get update && \
  apt-get install -y wget software-properties-common

RUN \
  wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - && \
  echo "deb http://apt.llvm.org/bionic/ llvm-toolchain-bionic-5.0 main" | tee -a /etc/apt/sources.list >/dev/null && \
  echo "deb http://apt.llvm.org/bionic/ llvm-toolchain-bionic-6.0 main" | tee -a /etc/apt/sources.list >/dev/null && \
  echo "deb http://apt.llvm.org/bionic/ llvm-toolchain-bionic-7 main" | tee -a /etc/apt/sources.list >/dev/null && \
  echo "deb http://apt.llvm.org/bionic/ llvm-toolchain-bionic-8 main" | tee -a /etc/apt/sources.list >/dev/null && \
  echo "deb http://apt.llvm.org/bionic/ llvm-toolchain-bionic-9 main" | tee -a /etc/apt/sources.list >/dev/null && \
  apt-get update && \
  apt-get install -y cmake libc6-dev-i386 && \
  apt-get install -y clang-5.0 clang-6.0 clang-7 clang-8 clang-9
