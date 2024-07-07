FROM debian AS base
ARG ARCH
ARG VERSION

RUN apt update -y

RUN apt install -y build-essential
RUN apt install -y libasound2-dev
ADD unison.c .
ADD Makefile .

RUN mkdir -p package/usr/bin
RUN make
RUN mv unison package/usr/bin

ADD DEBIAN package/DEBIAN
RUN sed -i s/ARCH/${ARCH}/g package/DEBIAN/control
RUN sed -i s/VERSION/${VERSION}/g package/DEBIAN/control

RUN mkdir -p package/etc/systemd/system
ADD soundqube-unison.service package/etc/systemd/system
RUN dpkg-deb --build --root-owner-group package
RUN mv package.deb soundqube-unison_${VERSION}-1_${ARCH}.deb


FROM scratch AS package
ARG ARCH
ARG VERSION

COPY --from=base /soundqube-unison_${VERSION}-1_${ARCH}.deb /
