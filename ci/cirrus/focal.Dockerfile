FROM ubuntu:focal

RUN \
  apt-get update && \
  apt-get install -y wget software-properties-common

RUN \
  wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - && \
  echo "deb http://apt.llvm.org/focal/ llvm-toolchain-focal-10 main" | tee -a /etc/apt/sources.list >/dev/null && \
  echo "deb http://apt.llvm.org/focal/ llvm-toolchain-focal-11 main" | tee -a /etc/apt/sources.list >/dev/null && \
  apt-get update && \
  apt-get install -y cmake libc6-dev-i386 && \
  apt-get install -y g++-9-multilib g++-10-multilib && \
  apt-get install -y g++-9 g++-10 && \
  apt-get install -y g++-9-aarch64-linux-gnu g++-10-aarch64-linux-gnu && \
  apt-get install -y clang-10 clang-11
