#!/bin/sh


buildPackage() {
   docker build --build-arg="ARCH=${ARCH}"  -t build-setbfree-${ARCH} --platform=$1 --output=../repo/${ARCH} --target=package  .
}

ARCH=arm64
buildPackage linux/arm64

ARCH=amd64
#buildPackage linux/amd64

