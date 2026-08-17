#!/bin/bash

# Compile pwnit
mkdir -p build
cmake -S . -B build 
cd build
make
cd ..

# Copy default configs
mkdir -p ~/.config/pwnit
cp config/pwnit.toml ~/.config/pwnit

mkdir -p ~/.config/pwnit/templates/
cp config/default.py ~/.config/pwnit/templates/

# Make pwnit global
sudo cp pwnit /usr/bin/
