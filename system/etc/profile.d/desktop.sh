
if [ "$TERM" == "linux" ];then
  touch linuxterm
  export DISPLAY=:56
  Xvfb :56 &
  x11vnc -N -forever &
  startxfce4 &

  sleep 3

  pw-jack -p 128 setBfree &

  pw-jack -p 128 aeolus -J &

  pw-jack -p 128 fluidsynth -p "fluid0" --server --no-shell -r48000  -a jack --connect-jack-outputs -m alsa_seq -o synth.default-soundfont="" -f /media/soundqube/init &

  sleep infinity
fi
