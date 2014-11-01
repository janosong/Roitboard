#!/bin/bash
PWD="$(dirname $0)"
echo $PWD
$PWD/mjpg_streamer -i "$PWD/input_file.so -d 160 -p $PWD/default.jpg" -o "$PWD/output_http.so -w $PWD/www" &
