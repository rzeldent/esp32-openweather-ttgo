#!/bin/sh
python3 -m pip install --upgrade pip setuptools wheel
python3 -m pip install Pillow
 
python3 ./import/images_to_cpp.py ./images ./include/images.h
python3 ./import/images_to_cpp.py ./images/modern ./include/weather_icons.h 75 75
#python3 ./import/images_to_cpp.py ./images/crystal ./include/weather_icons.h 75 75