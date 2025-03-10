FROM ghcr.io/db7/dat3m

RUN apt update && apt install -y clang
RUN apt update && apt install -y make

# assumes demo-ccat will be mounted on /work
ENV CFLAGS="-I/work/local/include -I/work/src"
RUN cp $DAT3M_HOME/scripts/dartagnan /usr/local/bin
WORKDIR /work