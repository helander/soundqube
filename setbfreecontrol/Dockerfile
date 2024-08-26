FROM debian AS base
ARG ARCH

RUN apt update -y

RUN apt install -y build-essential
RUN apt install -y lv2-dev
ADD src src/
ADD ttl ttl/
ADD Makefile .

RUN mkdir -p package/usr/lib/lv2
RUN make
RUN make install DESTDIR=package

ENV VERSION=1.0.0

ADD DEBIAN package/DEBIAN
RUN sed -i s/ARCH/${ARCH}/g package/DEBIAN/control
RUN sed -i s/VERSION/${VERSION}/g package/DEBIAN/control

RUN dpkg-deb --build --root-owner-group package
RUN mv package.deb soundqube-setbfreecontrol_${VERSION}-1_${ARCH}.deb


FROM scratch AS package

COPY --from=base /*.deb /
