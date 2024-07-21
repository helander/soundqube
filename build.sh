#!/bin/sh

docker run --privileged --rm tonistiigi/binfmt --install all

$(cd autoconnect;../pkgbuild.sh;)
$(cd aeolus;../pkgbuild.sh;)
$(cd desktop;../pkgbuild.sh;)
$(cd fluidsynth;../pkgbuild.sh;)
$(cd fluidweb;../pkgbuild.sh;)
$(cd jack;../pkgbuild.sh;)
$(cd main;../pkgbuild.sh;)
#$(cd kernel;../pkgbuild.sh;)
$(cd unison;../pkgbuild.sh;)
$(cd setbfree;../pkgbuild.sh;)

$(cd repo;apt-ftparchive --arch arm64 packages arm64 > Packages)
$(cd repo;apt-ftparchive --arch arm64 packages arm64 >> Packages)

$(cd repo;apt-ftparchive release . > Release)

git add repo
git commit -m "Temp"
git push

