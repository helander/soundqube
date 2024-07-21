FROM debian AS base
ARG ARCH

RUN apt update -y

ENV VERSION 1.0.1
ADD DEBIAN package/DEBIAN
RUN sed -i s/ARCH/${ARCH}/g package/DEBIAN/control
RUN sed -i s/VERSION/${VERSION}/g package/DEBIAN/control

RUN mkdir -p package/etc/systemd/system
ADD soundqube-desktop.service package/etc/systemd/system
RUN mkdir -p package/usr/bin
ADD soundqube-desktop.sh package/usr/bin
RUN dpkg-deb --build --root-owner-group package
RUN mv package.deb soundqube-desktop_${VERSION}-1_${ARCH}.deb

FROM scratch AS package
ARG ARCH
ENV VERSION 1.0.1

COPY --from=base /soundqube-desktop_${VERSION}-1_${ARCH}.deb /
