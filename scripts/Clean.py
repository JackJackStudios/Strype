import os
import shutil

os.chdir("./../")

FILE_EXTENSIONS = {".sln", ".csproj", ".vcxproj", ".vcxproj.filters", ".vcxproj.user"}
DIRS_TO_REMOVE = {"bin", "bin-int", "obj", "Build"} 

def remove_files(root_dir):
    for folder, _, files in os.walk(root_dir):
        for file in files:
            if any(file.endswith(ext) for ext in FILE_EXTENSIONS):
                file_path = os.path.join(folder, file)
                os.remove(file_path)
                print(f"Removed: {file}")

def remove_dirs(root_dir):
    for folder, subdirs, _ in os.walk(root_dir):
        for subdir in subdirs:
            if subdir in DIRS_TO_REMOVE:
                dir_path = os.path.join(folder, subdir)
                shutil.rmtree(dir_path, ignore_errors=True)

                relative_path = os.path.relpath(dir_path, root_dir)
                print(f"Removed: {relative_path}")

workspace_dir = os.path.abspath(os.getcwd())
remove_files(workspace_dir)
remove_dirs(workspace_dir)
