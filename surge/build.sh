#!/bin/sh

VERSION=1.0

buildPackage() {
   docker build --build-arg="ARCH=${ARCH}" --build-arg="VERSION=${VERSION}" -t build-surge-${ARCH} --platform=$1 --output=../repo/${ARCH} --target=package  .
}

ARCH=arm64
buildPackage linux/arm64

#ARCH=amd64
#buildPackage linux/amd64

