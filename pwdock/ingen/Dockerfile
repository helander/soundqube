FROM debian:trixie
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update -y
#RUN apt-get install -y jalv
RUN apt-get install -y pipewire-jack
RUN apt-get install -y lsp-plugins-lv2
RUN apt-get install -y mda-lv2
RUN apt-get install -y lilv-utils
RUN apt-get install -y setbfree

ADD entrypoint.sh /
RUN chmod a+x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]


RUN apt install -y build-essential git
RUN apt install -y meson
RUN git clone https://github.com/drobilla/ganv.git
RUN git clone https://github.com/drobilla/ingen.git

RUN apt install -y meson
RUN apt install -y pkg-config
RUN apt install -y cmake
RUN apt install -y libgtk2.0-dev
RUN apt install -y libgtkmm-2.4-dev
RUN apt install -y libgraphviz-dev
RUN apt install -y libgirepository1.0-dev

WORKDIR /ganv
RUN meson setup build
WORKDIR /ganv/build
RUN meson configure -Dprefix="/usr"
RUN meson install


RUN apt install -y libboost-dev
RUN apt install -y libserd-dev
RUN apt install -y libsord-dev
RUN apt install -y libsratom-dev
RUN apt install -y libsuil-dev
RUN apt install -y liblilv-dev

WORKDIR /
RUN git clone https://github.com/drobilla/raul.git

WORKDIR /raul
RUN meson setup build
WORKDIR /raul/build
RUN meson configure -Dprefix="/usr"
RUN meson install

RUN apt install -y libjack-jackd2-dev



WORKDIR /ingen
RUN meson setup build
WORKDIR /ingen/build
RUN meson configure -Dprefix="/usr" -Dgui="enabled" -Djack="enabled"
RUN meson compile
RUN meson install 

