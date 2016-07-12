#!/bin/bash

PATH=$PATH:$(pwd)/lib/win/
for MOD in $(find modules/ -maxdepth 1 -mindepth 1); do
    PATH=$PATH:$(pwd)/$MOD/dist/win/
done
