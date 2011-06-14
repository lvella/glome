#!BPY
# -*- coding: utf-8 -*-
#

"""
Name: 'Navigna Mesh Exporter'
Blender: 257
Group: 'Export'
Tooltip: 'Export meshes to Binary file format for Navigna Game Renega Desruga'
"""

# Copyright (C) 2011 Fractal Corp.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

try:
  import bpy
  from bpy import *
  import mathutils
except ImportError:
  sys.exit('Only works from within Blender!\n')

import os
import re
import struct
import math
from math import *

# Guns list
lguns = []
# Object scale
scale = 0.0005
gscale = 0.001

def xw_matrix(angle):
  c = cos(angle)
  s = sin(angle)
  return mathutils.Matrix(((c, 0, 0, -s),
    (0, 1, 0, 0),
    (0, 0, 1, 0),
    (s, 0, 0, c)))

def yw_matrix(angle):
  c = cos(angle)
  s = sin(angle)
  return mathutils.Matrix(((1, 0, 0, 0),
    (0, c, 0, -s),
    (0, 0, 1, 0),
    (0, s, 0, c)))

def zw_matrix(angle):
  c = cos(angle)
  s = sin(angle)
  return mathutils.Matrix(((1, 0, 0, 0),
    (0, 1, 0, 0),
    (0, 0, c, -s),
    (0, 0, s, c)))

def matrix_gen(v):
  return xw_matrix(v[0] * gscale) * yw_matrix(v[2] * gscale) *  zw_matrix(v[1] * gscale)

def vertex_conv(i, scale):
  'Convert 3-D to 4-D coordinates'
  x = i[0] * scale
  y = i[1] * scale
  z = i[2] * scale
  d = 1 + x*x + y*y + z*z
  return (2*x/d, 2*y/d, 2*z/d, (-1 + x*x + y*y + z*z)/d)

def export(obj, data):
  bfile = open(obj.name + '.wire', 'wb')
  #exporting guns
  bfile.write(struct.pack('<H', len(lguns)))
  for g in lguns:
    t = matrix_gen(g.location - obj.location)
    for i in range(0, t.row_size):
      for j in range(0, t.col_size):
        bfile.write(struct.pack('<f', t[i][j]))
  #exporting Navigna mesh
  export_mesh(bfile, data)
  bfile.close()
#  read_file(obj, data) #for testing only

def export_mesh(bfile, data):
  bfile.write(struct.pack('<H', len(data.vertices)))
  for v in data.vertices:
    out = vertex_conv(v.co, scale)# scale is hardcoded?
    bfile.write(struct.pack('<f', out[0]))
    bfile.write(struct.pack('<f', out[1]))
    bfile.write(struct.pack('<f', out[2]))
    bfile.write(struct.pack('<f', out[3]))
  bfile.write(struct.pack('<H', len(data.edges)))
  for e in data.edges.values():
    bfile.write(struct.pack('<H', e.key[0]))
    bfile.write(struct.pack('<H', e.key[1]))

# Only for testing
def read_file(obj, data):
  bfile = open(obj.name + '.wire', 'rb')
  fsize = struct.calcsize('f')
  isize = struct.calcsize('H')
  #Reading Header
  ng = bfile.read(isize)
  print(struct.unpack('<H', ng)[0])
  for g in lguns:
    for i in range(0, 4):
      for j in range(0, 4):
        a = bfile.read(fsize)
        print(struct.unpack('<f', a)[0])  #Reading Navigna
  nv = bfile.read(isize)
  print(struct.unpack('<H', nv)[0])
  for v in data.vertices:
    x = bfile.read(fsize)
    y = bfile.read(fsize)
    z = bfile.read(fsize)
    w = bfile.read(fsize)
    print(struct.unpack('<f', x)[0], struct.unpack('<f', y)[0], struct.unpack('<f', z)[0], struct.unpack('<f', w)[0])
  ne = bfile.read(isize)
  print(struct.unpack('<H', ne)[0])
  for e in data.edges:
    e0 = bfile.read(isize)
    e1 = bfile.read(isize)
    print(struct.unpack('<H', e0)[0], struct.unpack('<H', e1)[0])
  bfile.close()

#########
# Begin #
#########

def export_mesh_navigna():
  scene = bpy.context.scene
  objs = bpy.context.visible_objects
  for o in objs:
    if(re.search(('gun'), o.name) != None):
      lguns.append(o)
    if(o.type == 'MESH'):
      obj = o
  data = obj.to_mesh(scene,True,'PREVIEW')
  export(obj, data)
  #clean
  for o in objs:
    if(re.search(('gun'), o.name) != None):
      lguns.pop()

if __name__ == "__main__":
  export_mesh_navigna()
