FROM debian AS base
ARG ARCH
ARG VERSION

RUN apt update -y

ADD DEBIAN package/DEBIAN
RUN sed -i s/ARCH/${ARCH}/g package/DEBIAN/control
RUN sed -i s/VERSION/${VERSION}/g package/DEBIAN/control

RUN mkdir -p package/etc/systemd/system
ADD soundqube-jack.service package/etc/systemd/system
RUN dpkg-deb --build --root-owner-group package
RUN mv package.deb soundqube-jack_${VERSION}-1_${ARCH}.deb

FROM scratch AS package
ARG ARCH
ARG VERSION

COPY --from=base /soundqube-jack_${VERSION}-1_${ARCH}.deb /
