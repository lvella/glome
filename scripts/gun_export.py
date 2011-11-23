#!BPY
# -*- coding: utf-8 -*-
#

"""
Name: 'Glome Mesh Exporter'
Blender: 259
Group: 'Export'
Tooltip: 'Export Guns positions to Binary file format for Glome Game Renega Desruga'
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

# FILE FORMAT:
#: ([n]name: amount of items - integer)
#: (< list >: list of values (coordenates) - float)
#[BEGIN]
# nguns
# nguns * <guns Matrix4d>
#[END]

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

fsize = struct.calcsize('f')
isize = struct.calcsize('H')

# Object scale
scale = 0.0005
gscale = 0.001

##############
# Math Utils #
##############

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

###########
# Objects #
###########

class Guns:
  def __init__(self, objMesh, lguns):
    self.lguns = lguns
    self.objMesh = objMesh
    self.name = objMesh.name + '.gun'

  def export(self):
    bfile = open(self.name, 'wb')
    bfile.write(struct.pack('<H', len(self.lguns)))
    for g in self.lguns:
      t = matrix_gen(g.location - self.objMesh.location)
      for i in range(0, t.row_size):
        for j in range(0, t.col_size):
          bfile.write(struct.pack('<f', t[i][j]))
    bfile.close()

  def read(self):
    print('Guns of ' + self.name + ':')
    bfile = open(self.name, 'rb')
    ng = bfile.read(isize)
    print(struct.unpack('<H', ng)[0])
    for g in self.lguns:
      for i in range(0, 4):
        for j in range(0, 4):
          a = bfile.read(fsize)
          print(struct.unpack('<f', a)[0])
    bfile.close()

#########
# Begin #
#########

def ExportNMesh():
  listAllScenes = bpy.data.scenes
  currentScene = listAllScenes.get('gun')
  lguns = currentScene.objects.values()
  for o in listAllScenes.get('mesh').objects.values():
    #FIXME: (o.type == 'MESH') this comparison may be a problem in future
    if(o.type == 'MESH'):
      objMesh = o
  g = Guns(objMesh,lguns)
  g.export()
#  g.read()

if __name__ == "__main__":
  ExportNMesh()
