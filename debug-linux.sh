#!/bin/bash

source setldpath-linux.sh
export LD_PRELOAD=~/.tgd/libs/libNvidia_gfx_debugger.so:$LD_PRELOAD
gdb -i=mi dist/linux/game
