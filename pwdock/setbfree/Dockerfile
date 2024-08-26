FROM debian:trixie 

RUN apt update -y
# For container run-time environment
RUN apt install -y pipewire-jack pipewire pipewire-alsa

# For component build environment
RUN apt install -y build-essential libasound2-dev libjack-jackd2-dev
RUN apt install -y git
#RUN apt install -y ttf-bitstream-vera
#RUN apt install -y libftgl-dev
#RUN apt install -y libglu1-mesa-dev
#RUN apt install -y libgl1
#RUN apt install -y lv2-dev

RUN git clone https://github.com/pantherb/setBfree.git

ADD common.mak setBfree/
ADD src/tonegen.c setBfree/src/
ADD src/program.c setBfree/src/
ADD src/program.h setBfree/src/

#WORKDIR setBfree
#RUN make clean
#RUN make PREFIX=/usr ENABLE_ALSA=yes


#WORKDIR /

ADD src/http_server.c setBfree/src/
ADD src/http_server.h setBfree/src/
ADD src/Makefile setBfree/src/
ADD src/main.c setBfree/src/

WORKDIR setBfree

RUN make PREFIX=/usr ENABLE_ALSA=yes                                                                                                                                                                   
#RUN make install ENABLE_ALSA=yes PREFIX=/usr DESTDIR=/package         
RUN make install ENABLE_ALSA=yes PREFIX=/usr         


