#!/bin/bash

/root/dolly/bin/runwifi.sh
sleep 4
/root/dolly/bin/streamer/run.sh &> /dev/null &
sleep 3
#/root/dolly/bin/dollycam/dollycam &
/root/dolly/bin/dollycam/dollycam &> /dev/null &
echo "DONE"

