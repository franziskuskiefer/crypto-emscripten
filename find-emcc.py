#!/usr/bin/python2
import os
exec(open(os.path.expanduser("~/.emscripten")).read())
print(EMSCRIPTEN_ROOT)
