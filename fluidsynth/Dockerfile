FROM debian AS base
ARG ARCH
ARG VERSION

RUN apt update -y


ADD DEBIAN package/DEBIAN
RUN sed -i s/ARCH/${ARCH}/g package/DEBIAN/control
RUN sed -i s/VERSION/${VERSION}/g package/DEBIAN/control

RUN mkdir -p package/etc/systemd/system
ADD soundqube-fluidsynth.service package/etc/systemd/system
ADD init.fluidsynth package/etc/soundqube/fluidsynth/init
RUN dpkg-deb --build --root-owner-group package
RUN mv package.deb soundqube-fluidsynth_${VERSION}-1_${ARCH}.deb

FROM scratch AS package
ARG ARCH
ARG VERSION

COPY --from=base /soundqube-fluidsynth_${VERSION}-1_${ARCH}.deb /
