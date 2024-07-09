#!/bin/bash

##echo 'Updating /etc/hosts file...'
##HOSTNAME=$(hostname)
##echo "127.0.1.1\t$HOSTNAME" >> /etc/hosts



mkdir /root/.vnc 
echo "soundqube" | vncpasswd -f > /root/.vnc/passwd 
chmod 600 /root/.vnc/passwd

touch /root/.Xauthority

echo "Starting VNC server at $RESOLUTION..."
vncserver -kill :1 || true
vncserver -geometry $RESOLUTION &

echo "VNC server started at $RESOLUTION! ^-^"

echo "Starting tail -f /dev/null..."
tail -f /dev/null
