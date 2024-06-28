# soundqube
Meta repository for the soundqube project - a music studio in an Raspberry PI box

# referenced repositories
- Raspberry Pi image - contains a fully working setup
- Linux kernel with Realtime additions
- Fluidweb - a web interface to control Fluidsynth
- autoconnect - a tool to autocreate MIDI connections between programs
- unison - sends incoming MIDI note events to mutiple MIDI channels
- http2midi - generate MIDI events using an HTTP interface
- systemd services - various systemd service files

# Overview
The soundqube project aims at creating and using software for a music production studio.

The main hardware platform is currently the Rasberry Pi 4 computer but the project components referenced from this
repository are available for use on other hardware platforms.

Serious audio production on a Raspberry Pi in general requires an external USB audio interface. However if your
performance requirements are low, you can use a Pi with only the internal audio interface.

The Linux kernel used in the project is a kernel patched with the Realtime additions. These additions provide
significant performance improvements, so try to use them. One of the project components is a ready built Rasberry Pi image
which includes a kernel with these additions. Further, this kernel is also a project component, so in case you want to build
your own kernel, you could base that work on this component.

Currently, the only sound generating program is Fluidsynth, but more may come or you can add your own.

With the current standard setup you can have Fluidsynth play 8 different high quality instruments simultaneously.

With the unison component you could clone the MIDI notes from your attached keyboard, and have Fluidsynth play the same notes
on 8 different instruments simultaneously. 

Fluidsynth works with soundfonts and the project provides links to high quality soundfont files - your sound library may
become unlimited.
