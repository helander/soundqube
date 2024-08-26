FROM debian as base
ARG ARCH

RUN apt update -y

RUN apt install -y build-essential cmake
RUN apt install -y libreadline-dev libpipewire-0.3-dev
RUN apt install -y git

RUN git clone https://github.com/FluidSynth/fluidsynth.git

RUN apt install -y libglib2.0-dev
RUN apt install -y libjack-jackd2-dev
WORKDIR fluidsynth
RUN mkdir build
WORKDIR build
RUN cmake -Denable-pipewire=1 -DLIB_SUFFIX="" -DCMAKE_INSTALL_PREFIX=/usr ..
RUN make
RUN make install  DESTDIR=/package
WORKDIR /

ENV VERSION 1.0.1

ADD DEBIAN package/DEBIAN
RUN sed -i s/ARCH/${ARCH}/g package/DEBIAN/control
RUN sed -i s/VERSION/${VERSION}/g package/DEBIAN/control

RUN dpkg-deb --build --root-owner-group package
RUN mv package.deb soundqube-pwfluidsynth_${VERSION}-1_${ARCH}.deb


FROM scratch AS package

COPY --from=base /*.deb /
