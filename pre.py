Import('env')

#print(env.Dump())

print('convert_images')
env.Execute("$PYTHONEXE -m pip install Pillow")
env.Execute("$PYTHONEXE $PROJECT_DIR/convert/images_to_cpp.py $PROJECT_DIR/images $PROJECT_DIR/src/images.c $PROJECT_DIR/include/images.h")
print('convert_images done')