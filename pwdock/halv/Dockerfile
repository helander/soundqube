#FROM debian:trixie
FROM debian
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update -y
RUN echo "deb [trusted=yes] https://raw.githubusercontent.com/helander/soundqube/main/repo ./" > /etc/apt/sources.list.d/soundqube.list
RUN apt install -y ca-certificates

RUN apt-get update -y 
RUN apt-get install -y pipewire-jack
RUN apt-get install -y lilv-utils
RUN apt-get install -y soundqube-lv2unison

#RUN apt-get install -y soundqube-halv
RUN apt-get install -y jalv


CMD sleep infinity
