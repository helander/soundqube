FROM debian
ARG ARCH
ARG VERSION

RUN apt update -y

#RUN apt install -y dpkg-dev
#RUN apt install -y debhelper

ADD DEBIAN package/DEBIAN
RUN sed -i s/ARCH/${ARCH}/g package/DEBIAN/control
RUN sed -i s/VERSION/${VERSION}/g package/DEBIAN/control

RUN dpkg-deb --build --root-owner-group package
RUN mv package.deb soundqube_${VERSION}-1_${ARCH}.deb
