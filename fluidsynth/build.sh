#!/bin/sh

VERSION=1.0

mkdir -p repo

buildPackage() {
   docker build --build-arg="ARCH=${ARCH}" --build-arg="VERSION=${VERSION}" -t build-fluidsynth-${ARCH} .
   docker create --name build-fluidsynth-${ARCH} build-fluidsynth-${ARCH}
   docker cp build-fluidsynth-${ARCH}:soundqube-fluidsynth_${VERSION}-1_${ARCH}.deb repo
   docker rm build-fluidsynth-${ARCH}
}

ARCH=arm64
buildPackage

ARCH=amd64
buildPackage

cd repo;dpkg-scanpackages -m . | gzip -c > Packages.gz
