'''Includes functions for dealing with MinGW for compilation on Windows.'''
import os
import subprocess
from urllib.request import urlretrieve
import zipfile
rootdir = os.path.dirname(os.path.dirname(__file__))
mingwdir = rootdir + '/mingw'
if not os.path.isdir(mingwdir):
    os.mkdir(mingwdir)
def install_7z():
    '''Installs a command line version of 7Zip used to decompress the MinGW archive.'''
    archive = mingwdir + '/7za920.zip'
    if os.path.isfile(archive):
        os.remove(archive)
    #Download the zip file:
    print('Downloading 7z...')
    urlretrieve('https://www.7-zip.org/a/7za920.zip', archive)
    #Unpack the zip file:
    print('Unpacking 7z...')
    targetdir = mingwdir + '/7za'
    if os.path.isdir(targetdir):
        os.rmdir(targetdir)
    with zipfile.ZipFile(archive, 'r') as f:
        f.extractall(targetdir)
    print('Successfully installed 7z.')
def get_7z():
    '''Returns the path to the 7za executable.'''
    executable = os.path.realpath(mingwdir + '/7za/7za.exe')
    if os.path.isfile(executable):
        return executable
    print('7za.exe not found, installing...')
    install_7z()
    return executable
def install_mingw():
    '''Installs and unpacks MinGW.'''
    print('Downloading MinGW archive.\nThis may take a few minutes...')
    archive = mingwdir + '/mingw.zip'
    if os.path.exists(archive):
        os.remove(archive)
    urlretrieve('https://gcc-mcf.lhmouse.com/mingw-w64-gcc-mcf_20260407_16.0.1_x64-ucrt_be68aa557541c73183fbd0f8895e3330ceb9455e.7z', archive)
    print('Unpacking MinGW archive...')
    command = [get_7z(), 'x', '-o' + mingwdir + '/mingw', archive]
    subprocess.check_call(command)
    print('Successfully installed MinGW.')
def get_mingw_compiler():
    '''Returns the location of the g++ compiler used.'''
    compilerloc = mingwdir + '/mingw/ucrt64/bin/g++.exe'
    if os.path.isfile(compilerloc):
        return os.path.realpath(compilerloc)
    raise FileNotFoundError('mingw does not appear to be installed.\nTry pocketpylife.install_mingw()')
