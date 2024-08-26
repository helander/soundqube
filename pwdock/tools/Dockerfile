#Problem running qpwgraph in debian:trixie => do not use trixie (2024-08-25)
FROM debian
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update -y
RUN apt-get install -y qpwgraph
RUN apt-get install -y qtractor
RUN apt-get install -y vmpk
RUN apt-get install -y pipewire-jack
RUN apt-get install -y net-tools
RUN apt-get install -y xterm

ADD entrypoint.sh /
RUN chmod a+x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]
