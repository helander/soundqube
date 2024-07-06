#!/bin/sh

VERSION=1.0

mkdir -p repo

buildPackage() {
   docker build --build-arg="ARCH=${ARCH}" --build-arg="VERSION=${VERSION}" -t build-main-${ARCH} .
   docker create --name build-main-${ARCH} build-main-${ARCH}
   docker cp build-main-${ARCH}:soundqube_${VERSION}-1_${ARCH}.deb repo
   docker rm build-main-${ARCH}
}

ARCH=arm64
buildPackage

ARCH=amd64
buildPackage

cd repo;dpkg-scanpackages -m . | gzip -c > Packages.gz
