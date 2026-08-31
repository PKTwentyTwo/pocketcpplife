'''Python bindings for the C++ patterns.'''
from .config import configparams
from .pythlib.pattern import download_soups, download_synthesis, hashsoup, Pattern
from .pythlib.payosha256 import upload_results
def reset():
    '''Clears all compiled shared libraries.'''
    import glob
    import os
    libdir = os.path.dirname(__file__)+'/pythlib/lib'
    files = glob.glob(libdir + '/*.so')
    for x in files:
        os.remove(x)
__all__ = ['configparams', 'download_soups', 'download_synthesis', 'hashsoup', 'Pattern', 'reset', 'upload_results']
