FROM debian AS base
ARG ARCH

RUN apt update -y

RUN apt install -y golang

RUN mkdir -p package/opt/bin && mkdir -p package/opt/lib/fluidweb/www

ADD server.go .
RUN go mod init fluidweb
RUN GOOS=linux GOARCH=${ARCH} go build 
RUN mv fluidweb package/opt/bin
ADD www package/opt/lib/fluidweb/www

ENV VERSION 1.0.2

ADD DEBIAN package/DEBIAN
RUN sed -i s/ARCH/${ARCH}/g package/DEBIAN/control
RUN sed -i s/VERSION/${VERSION}/g package/DEBIAN/control

RUN mkdir -p package/etc/systemd/system
ADD fluidweb.service package/etc/systemd/system
RUN dpkg-deb --build --root-owner-group package
RUN mv package.deb fluidweb_${VERSION}-1_${ARCH}.deb

FROM scratch AS package
ARG ARCH
ENV VERSION 1.0.2

COPY --from=base /fluidweb_${VERSION}-1_${ARCH}.deb /
