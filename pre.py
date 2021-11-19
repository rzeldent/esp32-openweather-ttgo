import subprocess

subprocess.call('python3 ./convert/images_to_cpp.py ./images ./src/images.c ./include/images.h')