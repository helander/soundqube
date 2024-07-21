FROM debian AS base
ARG ARCH

RUN apt update -y

ENV VERSION 1.0.2

ADD DEBIAN package/DEBIAN
RUN sed -i s/ARCH/${ARCH}/g package/DEBIAN/control
RUN sed -i s/VERSION/${VERSION}/g package/DEBIAN/control

RUN dpkg-deb --build --root-owner-group package
RUN mv package.deb soundqube_${VERSION}-1_${ARCH}.deb

FROM scratch AS package
ARG ARCH
ENV VERSION 1.0.2

COPY --from=base /soundqube_${VERSION}-1_${ARCH}.deb /
