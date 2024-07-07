FROM debian AS base
ARG ARCH
ARG VERSION

RUN apt update -y

RUN apt install -y build-essential
RUN apt install -y libasound2-dev
ADD autoconnect.c .
ADD Makefile .

RUN mkdir -p package/usr/bin
RUN make
RUN mv autoconnect package/usr/bin

ADD DEBIAN package/DEBIAN
RUN sed -i s/ARCH/${ARCH}/g package/DEBIAN/control
RUN sed -i s/VERSION/${VERSION}/g package/DEBIAN/control

RUN mkdir -p package/etc/systemd/system
ADD soundqube-autoconnect.service package/etc/systemd/system
RUN dpkg-deb --build --root-owner-group package
RUN mv package.deb soundqube-autoconnect_${VERSION}-1_${ARCH}.deb


FROM scratch AS package
ARG ARCH
ARG VERSION

COPY --from=base /soundqube-autoconnect_${VERSION}-1_${ARCH}.deb /
