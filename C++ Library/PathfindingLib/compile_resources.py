import io
import os

files = []
res_dir = os.path.join(os.getcwd(),"res")

for filename in os.listdir(res_dir):
    files.append(filename)

for filename in files:
    resource_name = filename.split(".")[0]
    compiled_filename = resource_name + ".c"

    resource_file = open(os.path.join(res_dir,filename),"r")
    compiled_file = open(compiled_filename,"w+")

    
    compiled_file.write("#include \"stdafx.h\"\n")
    compiled_file.write("#include <stdlib.h>\n")
    compiled_file.write("const char "+resource_name+"[] = {\n")

    for line in resource_file:
        for character in list(line):
            compiled_file.write(hex(ord(character))+",")

    compiled_file.write(hex(ord("\0")))
    compiled_file.write("};\n")
    compiled_file.write("const size_t "+resource_name+"_len = sizeof("+resource_name+");\n\n")

    resource_file.close()
    compiled_file.close()
