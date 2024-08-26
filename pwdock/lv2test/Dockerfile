FROM debian
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update -y
RUN apt-get install -y jalv
RUN apt-get install -y pipewire-jack
RUN apt-get install -y lsp-plugins-lv2
RUN apt-get install -y lilv-utils

ADD entrypoint.sh /
RUN chmod a+x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]
