"""SCons.Tool.sunlink

Tool-specific initialization for the Sun Solaris (Forte) linker.

There normally shouldn't be any need to import this module directly.
It will usually be imported through the generic SCons.Tool.Tool()
selection method.
"""

#
# Copyright (c) 2001, 2002, 2003, 2004 The SCons Foundation
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
# KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#

__revision__ = "/home/scons/scons/branch.0/baseline/src/engine/SCons/Tool/sunlink.py 0.96.1.D001 2004/08/23 09:55:29 knight"

import os
import os.path

import SCons.Util

import link

ccLinker = None

# search for the acc compiler and linker front end

try:
    dirs = os.listdir('/opt')
except OSError:
    dirs = []

for d in dirs:
    linker = '/opt/' + d + '/bin/CC'
    if os.path.exists(linker):
        ccLinker = linker
        break

def generate(env):
    """Add Builders and construction variables for Forte to an Environment."""
    link.generate(env)
    
    env['SHLINKFLAGS'] = SCons.Util.CLVar('$LINKFLAGS -G')

def exists(env):
    return ccLinker
