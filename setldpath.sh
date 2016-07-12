#!/bin/bash

LD_LIBRARY_PATH=$LD_LIBRARY_PATH:lib/linux/
for MOD in $(find modules/ -maxdepth 1 -mindepth 1); do
    LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MOD/dist/linux/
done
