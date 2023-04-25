import glob
import shutil
import os

f = None
if not os.path.exists('.gitignore'):
    f = open('.gitignore', 'w')


bin_file_exts = [
    'dll',
    'exp',
    'obj',
    'o',
    'exe',
    'zip',
    'node',
    'raw',
    'lib'
]

for ext in bin_file_exts:
    if not f is None:
        f.write('*.%s\n' % ext)
    globbed_files = glob.glob('**/*.%s' % ext, recursive=True)
    for file in globbed_files:
        os.remove(file)

if not f is None:
    f.write('node_modules/\n')
    f.write('glfw/\n')
    f.write('freeglut/\n')
    f.write('libuv/\n')


for node_module in glob.glob('**/*/node_modules', recursive=True):
    shutil.rmtree(node_module)

shutil.rmtree('glfw')
shutil.rmtree('libuv')
shutil.rmtree('freeglut')


if not f is None:
    f.close()
