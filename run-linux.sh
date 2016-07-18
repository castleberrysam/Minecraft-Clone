#!/bin/bash

cd "$(dirname "$(readlink -f "$0")")"
source setldpath-linux.sh
dist/linux/game
