'''Functions related to compiling a shared library.'''
import os
import subprocess
import sys
import time
if os.name == 'nt':
    from .mingw64 import get_mingw_compiler
from ..config import configparams, isvalid
rootdir = os.path.dirname(os.path.dirname(__file__))
libdir = os.path.dirname(__file__) + '/lib'
if not os.path.isdir(libdir):
    os.mkdir(libdir)
def getcompiler():
    '''Determines if it is feasible to compile in the current working environment.
Throws an error if compilation is not possible, and otherwise returns the compiler.'''
    #Use mingw64 on Windows:
    if os.name == 'nt':
        return get_mingw_compiler()
    #Check that a c++ compiler is avaliable:
    try:
        compiler = subprocess.check_output(['/bin/bash', 'which', 'c++']).decode('utf-8').replace('\n', '')
    except subprocess.CalledProcessError:
        #Search on PATH:
        for x in sys.path:
            file = x + '/c++'
            if os.path.isfile(file):
                if os.access(file, os.X_OK):
                    return x + '/c++'
        raise OSError('''A C++ compiler does not appear to be installed on this system.
Try installing it with: one of the following:
1. sudo apt install g++
2. sudo apt install clang''')
    return compiler
def compilelibrary(rule, compilerargs = ['-std=c++17', '-O3', '-Os', '-Ofast']):
    '''Compiles a shared library for the given rule.'''
    if not isvalid(rule):
        raise ValueError('Rule '+str(rule)+' is not recognised as a non-B0 isotropic 2-state Moore rule.')
    #Edit the parameters header file:
    configparams(rule)
    #Preparations for compilation:
    compiler = getcompiler()
    infile = rootdir + '/main.cpp'
    outfile = libdir + '/' + rule + '.so'
    forcedflags = [infile, '-o', outfile, '-fPIC', '-shared']
    if os.name == 'nt':
        #Required to avoid DLL hell:
        #https://en.wikipedia.org/wiki/DLL_hell
        forcedflags += ['-static', '-static-libgcc', '-static-libstdc++']
    command = [compiler] + forcedflags + compilerargs
    joinedcommand = ''
    for x in command:
        joinedcommand += x + ' '
    joinedcommand = joinedcommand[:-1]
    #Time for the actual compilation:
    sys.stderr.write('Attempting compilation with command: \n' + joinedcommand + '\n')
    starttime = time.time()
    wd = os.path.dirname(compiler)
    cwd = os.getcwd()
    os.chdir(wd)
    with subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=wd) as proc:
        compiler_stdout, compiler_stderr = proc.communicate()
        status = proc.returncode
    os.chdir(cwd)
    if status == 0:
        sys.stderr.write('Compilation succeeded in '+str(round(time.time() - starttime, 3))+' seconds.\n')
    else:
        raise ValueError('Error occurred during compilation!\n\n' + compiler_stderr.decode('utf-8'))
