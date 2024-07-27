#!/bin/sh

audiousb=$(aplay -l|grep 'USB Audio'|awk '{print $2}'|awk -F ':' '{print $1}')
ALSADEV="hw:${audiousb}"
echo ALSADEV $ALSADEV
exec /usr/bin/jackd -R -P99 -d alsa -d $ALSADEV -P -S -p 128 -n 2 -r 48000 -i 0 -o 2
