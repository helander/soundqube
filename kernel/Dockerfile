FROM debian AS base
ARG ARCH
ARG VERSION

RUN apt update -y

##########################

ENV RASPIOS_IMAGE_NAME=raspios_lite_arm64
ENV LINUX_KERNEL_VERSION=6.6
ENV LINUX_KERNEL_BRANCH=rpi-${LINUX_KERNEL_VERSION}.y
ENV LINUX_KERNEL_RT_PATCH=patch-6.6.36-rt35

ENV TZ=Europe/Copenhagen
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


# print the above env variables
RUN echo ${KERNEL} ${ARCH} 

RUN  make bcm2711_defconfig
RUN ./scripts/config --disable CONFIG_VIRTUALIZATION
RUN ./scripts/config --enable CONFIG_PREEMPT_RT
RUN ./scripts/config --disable CONFIG_RCU_EXPERT
RUN ./scripts/config --enable CONFIG_RCU_BOOST
#RUN [ "$ARCH" = "arm" ] && ./scripts/config --enable CONFIG_SMP || true
#RUN [ "$ARCH" = "arm" ] && ./scripts/config --disable CONFIG_BROKEN_ON_SMP || true
RUN ./scripts/config --set-val CONFIG_RCU_BOOST_DELAY 500

RUN make -j4 Image modules dtbs

RUN echo "using raspberry pi image ${RASPIOS_IMAGE_NAME}"

#WORKDIR /raspios

#RUN export DATE=$(curl -s https://downloads.raspberrypi.org/${RASPIOS_IMAGE_NAME}/images/ | sed -n "s:.*${RASPIOS_IMAGE_NAME}-\(.*\)/</a>.*:\1:p" | tail -1) && \
#    export RASPIOS=$(curl -s https://downloads.raspberrypi.org/${RASPIOS_IMAGE_NAME}/images/${RASPIOS_IMAGE_NAME}-${DATE}/ | sed -n "s:.*<a href=\"\(.*\).xz\">.*:\1:p" | tail -1) && \
#    echo "Downloading ${RASPIOS}.xz" && \
#    curl https://downloads.raspberrypi.org/${RASPIOS_IMAGE_NAME}/images/${RASPIOS_IMAGE_NAME}-${DATE}/${RASPIOS}.xz --output ${RASPIOS}.xz && \
#    xz -d ${RASPIOS}.xz

#RUN mkdir /raspios/mnt && mkdir /raspios/mnt/disk && mkdir /raspios/mnt/boot
#ADD xbuild.sh ./xbuild.sh
#ADD config.txt ./

##########################

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
