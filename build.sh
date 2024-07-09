#!/bin/sh

$(cd autoconnect;./build.sh;)
$(cd desktop;./build.sh;)
$(cd fluidsynth;./build.sh;)
$(cd fluidweb;./build.sh;)
$(cd jack;./build.sh;)
$(cd main;./build.sh;)
$(cd unison;./build.sh;)

$(cd repo;apt-ftparchive --arch arm64 packages arm64 > Packages)
$(cd repo;apt-ftparchive --arch arm64 packages arm64 >> Packages)

$(cd repo;apt-ftparchive release . > Release)

git add repo
git commit -m "Temp"
git push

