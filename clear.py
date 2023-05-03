import glob
import shutil
import os

f = None
if os.path.exists('.gitignore'):
    os.remove('.gitignore')

f = open('.gitignore', 'w', encoding='utf-8')


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
    f.write('node_modules\n')
    f.write('glfw\n')
    f.write('freeglut\n')
    f.write('libuv\n')
    f.write('GDCM\n')
    f.write('opencv\n')
    f.write('sample_dicom\n')


for node_module in glob.glob('**/*/node_modules', recursive=True):
    shutil.rmtree(node_module)

shutil.rmtree('glfw')
shutil.rmtree('libuv')
shutil.rmtree('freeglut')
shutil.rmtree('dicom2raw/GDCM')
shutil.rmtree('dicom2raw/opencv')


if not f is None:
    f.close()
