import os
import subprocess
import shutil
import urllib.request


shutil.unpack_archive('Dependencies.zip', './', 'zip')

sb = subprocess.Popen(['npm', 'i', '--ignore-scripts'],
                      shell=True, cwd=os.path.realpath('./node'))
sb.wait()

sb = subprocess.Popen(['npm', 'run', 'build-native'],
                      shell=True, cwd=os.path.realpath('./node'))
sb.wait()

sb = subprocess.Popen(['npm', 'i', '--ignore-scripts'],
                      shell=True, cwd=os.path.realpath('./dicom2raw/node'))
sb.wait()

sb = subprocess.Popen(['npm', 'run', 'build-native'],
                      shell=True, cwd=os.path.realpath('./dicom2raw/node'))
sb.wait()

shutil.copy('freeglut/bin/freeglut.dll', 'Node/freeglut.dll')
shutil.copy('glfw/bin/glfw3.dll', 'Node/glfw3.dll')
shutil.copy('libuv/bin/uv.dll', 'uv.dll')

urllib.request.urlretrieve(
    "https://www.dl.dropboxusercontent.com/s/7d87jcsh0qodk78/fuel_64x64x64_uint8.raw?dl=1", "ABC_64_64_64.raw")

sb.wait()
