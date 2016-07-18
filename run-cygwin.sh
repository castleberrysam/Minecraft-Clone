#!/bin/bash

cd "$(dirname "$(readlink -f "$0")")"
source setldpath-cygwin.sh
dist/win/game.exe
