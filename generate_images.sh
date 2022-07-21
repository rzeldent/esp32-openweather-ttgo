#!/bin/sh
python3 -m pip install --upgrade pip setuptools wheel
python3 -m pip install Pillow
 
python3 ./import/images_to_cpp.py ./images ./src/images.c ./include/images.h