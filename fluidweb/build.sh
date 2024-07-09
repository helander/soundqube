#!/bin/sh

VERSION=1.0

buildPackage() {
   docker build --build-arg="ARCH=${ARCH}" --build-arg="VERSION=${VERSION}" -t build-fluidweb-${ARCH} --output=../repo/${ARCH} --target=package .
}

ARCH=arm64
buildPackage

ARCH=amd64
buildPackage

