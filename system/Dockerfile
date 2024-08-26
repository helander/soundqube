FROM debian AS base
ARG ARCH

ENV VERSION 1.0.13
ADD DEBIAN package/DEBIAN
RUN sed -i s/ARCH/${ARCH}/g package/DEBIAN/control
RUN sed -i s/VERSION/${VERSION}/g package/DEBIAN/control

RUN mkdir -p package
ADD etc/ package/etc

RUN dpkg-deb --build --root-owner-group package
RUN mv package.deb soundqube-system_${VERSION}-1_${ARCH}.deb


FROM scratch AS package

COPY --from=base /*.deb /
