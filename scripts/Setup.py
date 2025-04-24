import os
import subprocess
import shutil

lib_path = "Libraries/"

os.chdir('./../')

subprocess.call(["git", "submodule", "update", "--init", "--recursive"])
subprocess.call(["setx", "STRYPE_DIR", os.getcwd()])
os.environ['STRYPE_DIR'] = os.getcwd()

for file in os.listdir(lib_path):
    if file.endswith(".lua"):
        file_path = os.path.join(lib_path, file)
        copy_folder = file_path[:file_path.find("_")] + "/" 
        shutil.copy2(file_path, copy_folder + "premake5.lua")

subprocess.call([os.path.abspath("./" + lib_path + "premake/premake5.exe"), "--verbose", "vs2022"])

for file in os.listdir(lib_path):
    if file.endswith(".lua"):
        file_path = os.path.join(lib_path, file)
        copy_folder = file_path[:file_path.find("_")] + "/" 
        os.remove(copy_folder + "premake5.lua")