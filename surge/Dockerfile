FROM debian AS base
ARG ARCH
ARG VERSION

RUN apt update -y

RUN apt install -y build-essential libcairo-dev libxkbcommon-x11-dev libxkbcommon-dev libxcb-cursor-dev libxcb-keysyms1-dev libxcb-util-dev libxrandr-dev libxinerama-dev libxcursor-dev libasound2-dev libjack-jackd2-dev
RUN apt install -y git
RUN git clone https://github.com/surge-synthesizer/surge.git
RUN apt install -y cmake

WORKDIR /surge
RUN git submodule update --init  --recursive
RUN cmake -Bignore/sxt -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
RUN cmake --build ignore/sxt --config Release --parallel 8
RUN cmake --prefix /package --install ignore/sxt

WORKDIR /
#RUN mkdir -p package/usr/bin
#RUN make
#RUN mv unison package/usr/bin

ADD DEBIAN package/DEBIAN
RUN sed -i s/ARCH/${ARCH}/g package/DEBIAN/control
RUN sed -i s/VERSION/${VERSION}/g package/DEBIAN/control

RUN mkdir -p package/etc/systemd/system
ADD soundqube-surge.service package/etc/systemd/system
RUN dpkg-deb --build --root-owner-group package
RUN mv package.deb soundqube-surge_${VERSION}-1_${ARCH}.deb


FROM scratch AS package
ARG ARCH
ARG VERSION

COPY --from=base /soundqube-surge_${VERSION}-1_${ARCH}.deb /
