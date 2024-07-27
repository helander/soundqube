#!/bin/bash

#export DISPLAY=:56

Xvfb $DISPLAY &

x11vnc -N -forever & 

sleep 5

#blackbox &

#wmdrawer &

startxfce4 &

sleep infinity
