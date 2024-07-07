#!/bin/sh

VERSION=1.0

mkdir -p repo

buildPackage() {
   docker build --build-arg="ARCH=${ARCH}" --build-arg="VERSION=${VERSION}" -t build-jack-${ARCH} --output=repo --target=package .
#   docker create --name build-jack-${ARCH} build-jack-${ARCH}
#   docker cp build-jack-${ARCH}:soundqube-jack_${VERSION}-1_${ARCH}.deb repo
#   docker rm build-jack-${ARCH}
}

ARCH=arm64
buildPackage

ARCH=amd64
buildPackage

cd repo;dpkg-scanpackages -m . | gzip -c > Packages.gz
