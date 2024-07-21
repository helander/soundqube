FROM debian as base
ARG ARCH

RUN apt update -y

RUN apt install -y build-essential libasound2-dev libjack-jackd2-dev
RUN apt install -y libzita-alsa-pcmi-dev libclthreads-dev libclxclient-dev libreadline-dev
RUN apt install -y git

RUN git clone https://github.com/Organnery/aeolus-organnery.git

WORKDIR aeolus-organnery/source
ADD Makefile .
RUN make clean                                                                                                                                                                                         
RUN make PREFIX=/usr                                                                                                                                                                   
RUN make install  PREFIX=/usr DESTDIR=/package         

WORKDIR /

ENV VERSION 1.0.0

ADD DEBIAN package/DEBIAN
RUN sed -i s/ARCH/${ARCH}/g package/DEBIAN/control
RUN sed -i s/VERSION/${VERSION}/g package/DEBIAN/control

RUN mkdir -p package/etc/systemd/system
ADD soundqube-aeolus.service package/etc/systemd/system
RUN dpkg-deb --build --root-owner-group package
RUN mv package.deb soundqube-aeolus_${VERSION}-1_${ARCH}.deb


FROM scratch AS package
ARG ARCH
ENV VERSION 1.0.0

COPY --from=base /soundqube-aeolus_${VERSION}-1_${ARCH}.deb /
