FROM debian AS base
ARG ARCH
ARG VERSION

RUN apt update -y

ENV LINUX_KERNEL_VERSION=6.6
ENV LINUX_KERNEL_BRANCH=rpi-${LINUX_KERNEL_VERSION}.y
ENV LINUX_KERNEL_RT_PATCH=patch-6.6.36-rt35

ENV TZ=Europe/Stockholm
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get update
RUN apt-get install -y git make gcc bison flex libssl-dev bc ncurses-dev kmod \
    wget zip unzip fdisk nano curl xz-utils jq

WORKDIR /rpi-kernel
RUN git clone https://github.com/raspberrypi/linux.git -b ${LINUX_KERNEL_BRANCH} --depth=1
WORKDIR /rpi-kernel/linux
RUN curl https://mirrors.edge.kernel.org/pub/linux/kernel/projects/rt/${LINUX_KERNEL_VERSION}/older/${LINUX_KERNEL_RT_PATCH}.patch.gz --output ${LINUX_KERNEL_RT_PATCH}.patch.gz && \
    gzip -cd /rpi-kernel/linux/${LINUX_KERNEL_RT_PATCH}.patch.gz | patch -p1 --verbose

ENV KERNEL=kernel8

RUN  make bcm2711_defconfig
RUN ./scripts/config --disable CONFIG_VIRTUALIZATION
RUN ./scripts/config --enable CONFIG_PREEMPT_RT
RUN ./scripts/config --disable CONFIG_RCU_EXPERT
RUN ./scripts/config --enable CONFIG_RCU_BOOST
RUN ./scripts/config --set-val CONFIG_RCU_BOOST_DELAY 500

RUN make -j4 Image modules dtbs


WORKDIR /rpi-kernel/linux/
RUN mkdir -p /package/usr/share/soundqube-kernel/boot/overlays
RUN make INSTALL_MOD_PATH=/package modules_install
RUN make INSTALL_DTBS_PATH=/package/usr/share/soundqube-kernel/boot dtbs_install

RUN cp /rpi-kernel/linux/arch/arm64/boot/dts/broadcom/*.dtb /package/usr/share/soundqube-kernel/boot/
RUN cp /rpi-kernel/linux/arch/arm64/boot/dts/overlays/*.dtb* /package/usr/share/soundqube-kernel/boot/overlays/
RUN cp /rpi-kernel/linux/arch/arm64/boot/dts/overlays/README /package/usr/share/soundqube-kernel/boot/overlays/
RUN cp /rpi-kernel/linux/arch/arm64/boot/Image /package/usr/share/soundqube-kernel/boot/$KERNEL\_rt.img

ADD config.txt /package/usr/share/soundqube-kernel/boot

WORKDIR /
ADD DEBIAN package/DEBIAN
RUN sed -i s/ARCH/${ARCH}/g package/DEBIAN/control
RUN sed -i s/VERSION/${VERSION}/g package/DEBIAN/control

RUN dpkg-deb --build --root-owner-group package
RUN mv package.deb soundqube-kernel_${VERSION}-1_${ARCH}.deb

FROM scratch AS package
ARG ARCH
ARG VERSION

COPY --from=base /soundqube-kernel_${VERSION}-1_${ARCH}.deb /
