#!/bin/bash

##echo 'Updating /etc/hosts file...'
##HOSTNAME=$(hostname)
##echo "127.0.1.1\t$HOSTNAME" >> /etc/hosts

export HOME=/root

mkdir -p $HOME/.vnc 
echo "soundqube" | vncpasswd -f > $HOME/.vnc/passwd 
chmod 600 $HOME/.vnc/passwd

touch $HOME/.Xauthority

echo "Starting VNC server at $RESOLUTION..."
vncserver -kill :1 || true
vncserver -geometry $RESOLUTION &

echo "VNC server started at $RESOLUTION! ^-^"

echo "Starting tail -f /dev/null..."
tail -f /dev/null
