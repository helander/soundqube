#!/bin/sh

docker run --privileged --rm tonistiigi/binfmt --install all

$(cd aeolus;../pkgbuild.sh;)
$(cd fluidsynth;../pkgbuild.sh;)
$(cd webui;../pkgbuild.sh;)
#$(cd kernel;../pkgbuild.sh;)
$(cd unison;../pkgbuild.sh;)
$(cd setbfree;../pkgbuild.sh;)
$(cd system;../pkgbuild.sh;)

$(cd repo;apt-ftparchive --arch arm64 packages arm64 > Packages)
$(cd repo;apt-ftparchive --arch arm64 packages arm64 >> Packages)

$(cd repo;apt-ftparchive release . > Release)

git add repo
git commit -m "Temp"
git push

