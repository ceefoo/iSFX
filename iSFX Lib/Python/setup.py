from distutils.core import setup, Extension
from sys import platform

if platform == 'darwin':
  module1 = Extension('iSFX',
                      include_dirs = ['../../FMOD/inc',
                                      '/usr/local/include',
                                      '../C++'],
                      libraries = ['fmodex'],
                      library_dirs = ['../../FMOD/lib/darwin'],
                      sources = ['pyiSFX.cpp'])
if platform == 'linux':
  module1 = Extension('iSFX',
                      include_dirs = ['../../FMOD/inc',
                                      '../C++'],
                      libraries = ['../../FMOD/lib/' + platform + '/libfmodex.so'],
                      sources = ['pyiSFX.cpp'])

setup (name = 'iSFX',
       version = '1.0',
       description = 'iSFX',
       ext_modules = [module1])