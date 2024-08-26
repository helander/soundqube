FROM debian
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update -y
RUN apt-get install -y xvfb x11vnc blackbox menu
RUN apt-get install -y qpwgraph
RUN apt-get install -y qtractor
RUN apt-get install -y vmpk
RUN apt-get install -y fluidsynth
RUN apt-get install -y pipewire-jack pipewire-alsa
RUN apt-get install -y net-tools
RUN apt-get install -y x11-xserver-utils

ADD entrypoint.sh /
RUN chmod a+x /entrypoint.sh

ENV DISPLAY :57

ENTRYPOINT ["/entrypoint.sh"]
