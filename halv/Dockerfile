FROM debian:bookworm AS base
ARG ARCH

RUN apt update -y

RUN apt install -y build-essential git
RUN apt install -y meson
RUN touch x
RUN git clone https://github.com/helander/jalv-http.git

RUN apt install -y libjack-jackd2-dev
#RUN apt install -y libserd-dev
#RUN apt install -y libsord-dev
#RUN apt install -y libsratom-dev
#RUN apt install -y libsuil-dev
#RUN apt install -y liblilv-dev
RUN apt install -y libgtk-3-dev
RUN apt install -y qtbase5-dev
RUN apt install -y qt6-base-dev

#RUN apt install -y pkg-config
#RUN apt install -y cmake
#RUN apt install -y libgtk2.0-dev
#RUN apt install -y libgtkmm-2.4-dev
#RUN apt install -y libgraphviz-dev
#RUN apt install -y libgirepository1.0-dev

#WORKDIR /ganv
#RUN meson setup build
#WORKDIR /ganv/build
#RUN meson configure -Dprefix="/usr"
#RUN meson install


#RUN apt install -y libboost-dev

#WORKDIR /
#RUN git clone https://github.com/drobilla/raul.git

#WORKDIR /raul
#RUN meson setup build
#WORKDIR /raul/build
#RUN meson configure -Dprefix="/usr"
#RUN meson install




WORKDIR /jalv-http
RUN meson setup build
WORKDIR /jalv-http/build
RUN meson configure -Dprefix="/opt/soundqube/halv" -Dgtk3="enabled" -Dqt5="enabled" -Dqt6="enabled" -Dsuil="enabled" -Djack="enabled"
RUN meson compile
RUN meson install  --destdir=/package


ENV VERSION=1.0.0

WORKDIR /
ADD DEBIAN package/DEBIAN
ADD bin package/bin

RUN sed -i s/ARCH/${ARCH}/g package/DEBIAN/control
RUN sed -i s/VERSION/${VERSION}/g package/DEBIAN/control

RUN dpkg-deb --build --root-owner-group package
RUN mv package.deb soundqube-halv_${VERSION}-1_${ARCH}.deb


FROM scratch AS package

COPY --from=base /*.deb /
