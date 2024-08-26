FROM debian as base
ARG ARCH

RUN apt update -y

RUN apt install -y build-essential libasound2-dev libjack-jackd2-dev
RUN apt install -y git
#RUN apt install -y ttf-bitstream-vera
#RUN apt install -y libftgl-dev
#RUN apt install -y libglu1-mesa-dev
#RUN apt install -y libgl1
#RUN apt install -y lv2-dev

RUN git clone https://github.com/pantherb/setBfree.git

ADD common.mak setBfree/
ADD src/tonegen.c setBfree/src/
ADD src/program.c setBfree/src/
ADD src/program.h setBfree/src/

#WORKDIR setBfree
#RUN make clean
#RUN make PREFIX=/usr ENABLE_ALSA=yes


#WORKDIR /

ADD src/http_server.c setBfree/src/
ADD src/http_server.h setBfree/src/
ADD src/Makefile setBfree/src/
ADD src/main.c setBfree/src/

WORKDIR setBfree

RUN make PREFIX=/usr ENABLE_ALSA=yes                                                                                                                                                                   
RUN make install ENABLE_ALSA=yes PREFIX=/usr DESTDIR=/package         

WORKDIR /

ENV VERSION 1.3.53
ADD DEBIAN package/DEBIAN
RUN sed -i s/ARCH/${ARCH}/g package/DEBIAN/control
RUN sed -i s/VERSION/${VERSION}/g package/DEBIAN/control

RUN mkdir -p package/etc/systemd/user
ADD soundqube-setbfree.service package/etc/systemd/user
RUN dpkg-deb --build --root-owner-group package
RUN mv package.deb soundqube-setbfree_${VERSION}-1_${ARCH}.deb


FROM scratch AS package

COPY --from=base /*.deb /
