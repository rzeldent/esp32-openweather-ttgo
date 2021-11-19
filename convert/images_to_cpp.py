# pip install Pillow

import os
import sys
from PIL import Image

if (len(sys.argv) < 3):
    print('Usage: images_to_cpp.py <input_dir> <output_dir> <header path - optional>')
    sys.exit(1)

input_dir = sys.argv[1]
output_dir = sys.argv[2]
header_path = sys.argv[3] if len(sys.argv) > 3 else None

converted = []

file_names = os.listdir(input_dir)
for file_name in file_names:
    print('Processing: ' + file_name)

    image_path = os.path.join(input_dir, file_name)
    base_name = os.path.splitext(file_name)[0]
    output_file = os.path.join(output_dir, base_name + '.c')

    output_file = open(output_file, 'w')

    image_data_name = 'image_data_' + base_name
    image = Image.open(image_path)
    image = image.convert('RGB')

    width, height = image.size
    pixels = list(image.getdata())

    output_file.write('//*******************************************************************************\n')
    output_file.write('// Original file: ' + file_name + '\n')
    output_file.write('// Type: R5G6B5\n')
    output_file.write('// Scan: top_to_bottom then forward\n')
    output_file.write('// ******************************************************************************\n')
    output_file.write('\n')

    output_file.write('#include <image.h>\n')
    output_file.write('\n')
    output_file.write('static const uint16_t ' + image_data_name + '[' + str(height * width) + '] = {\n')

    convert_R8G8B8_to_R5G6B5 = lambda rgb: (rgb[0] >> 3) << 11 | (rgb[1] >> 2) << 5 | (rgb[2] >> 3)

    for y in range(0, height):
        for x in range(width):
            value = pixels[y * width + x]
            output_file.write('0x' + hex(convert_R8G8B8_to_R5G6B5(value))[2:].zfill(4))
            if (y*width+x < width*height - 1):
                output_file.write(',')
        output_file.write('\n')

    output_file.write('};\n')
    output_file.write('\n')

    output_file.write('const image_t ' + base_name + ' = { ' + image_data_name + ', ' + str(width) + ', ' + str(height) + ' };\n')

    output_file.close()
    image.close()

    converted.append(base_name)

if (header_path):
    output_file = open(header_path, 'w')

    output_file.write('#pragma once\n')
    output_file.write('\n')
    output_file.write('#include <image.h>\n')
    output_file.write('\n')

    for base_name in converted:
        output_file.write('extern const image_t ' + base_name + ';\n')


    output_file.close()

print('Done.')