#!/bin/sh
Xvfb $DISPLAY -listen tcp &
sleep 5
x11vnc -N -forever &
sleep 5
qpwgraph -ax /media/soundqube/soundqube.qpwgraph &
xhost +
touch /ready
blackbox
