FROM debian AS base
ARG ARCH

RUN apt update -y

RUN apt install -y golang

RUN mkdir -p package/opt/bin && mkdir -p package/opt/lib/soundqube-webui/www

ADD server.go .
RUN go mod init soundqube-webui
RUN GOOS=linux GOARCH=${ARCH} go build 
RUN mv soundqube-webui package/opt/bin
ADD www package/opt/lib/soundqube-webui/www

ENV VERSION 1.0.12

ADD DEBIAN package/DEBIAN
RUN sed -i s/ARCH/${ARCH}/g package/DEBIAN/control
RUN sed -i s/VERSION/${VERSION}/g package/DEBIAN/control

RUN mkdir -p package/etc/systemd/user
ADD soundqube-webui.service package/etc/systemd/user
RUN dpkg-deb --build --root-owner-group package
RUN mv package.deb soundqube-webui_${VERSION}-1_${ARCH}.deb

FROM scratch AS package

COPY --from=base /*.deb /
