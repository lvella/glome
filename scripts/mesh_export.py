#!BPY
# -*- coding: utf-8 -*-
#

"""
Name: 'Glome Mesh Exporter'
Blender: 259
Group: 'Export'
Tooltip: 'Export meshes to Binary file format for Glome Game Renega Desruga'
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
# (The header elements - unsigned integer(I) (4 bytes))
# (The amount of items - unsigned short(H) (2 bytes))
# (< list > represents a list of values (coordenates) - float(f) (4 bytes))
#
#[BEGIN]
# ##########
# # HEADER #
# ##########
# mesh guns engines (uint ea)
# ########
# # BODY #
# ########
# nvertices (ushort)
# nvertices lines contaning vertex coordenates and colors attributes: <x, y, z, w> <r, g, b, a> (8*float ea)
# nedges (ushort)
# nedges lines contaning 2 vertex index per line: <v_in0 , v_in1> (2*ushort ea)
# nguns (ushort)
# nguns * <guns Matrix4d positions> (16*float ea)
# nengines (ushort)
# nengines * <engines Matrix4d positions> (16*float ea)
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
from struct import calcsize
import math
from math import *

fsize = struct.calcsize('f')
usize = struct.calcsize('H')
isize = struct.calcsize('I')

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

class SpaceShip:
  def __init__(self, objs, listAllScenes):
    self.data = objs.get(ssname).to_mesh(listAllScenes.get('mesh'),True,'PREVIEW')
    self.ssMesh = Mesh(self.data)
    #TODO: work for n guns and engines
    self.lguns = listAllScenes.get('gun').objects.values()
    self.ssGuns = Guns(objs.get(ssname),self.lguns)
    self.lengines = listAllScenes.get('engine').objects.values()
    self.ssEngines = Engines(objs.get(ssname),self.lengines)

  def export(self):
    self.ssMesh.export()
    self.ssGuns.export()
    self.ssEngines.export()

  def create_header(self):
    bfile = open(filename, 'ab')
    # calc initial positions
    self.mesh_pos = header_size = 3 * isize
    mesh_size = (len(self.data.vertices) * 8 * fsize) + (len(self.data.edges) * 2 * usize) + (2 * usize)
    self.gun_pos = header_size + mesh_size
    gun_size = (len(self.lguns) * 16 * fsize) + usize
    self.engine_pos = header_size + mesh_size + gun_size
    engine_size = (len(self.lengines) * 16 * fsize) + usize
    # write header
    bfile.write(struct.pack('<I', self.mesh_pos))
    bfile.write(struct.pack('<I', self.gun_pos))
    bfile.write(struct.pack('<I', self.engine_pos))
    bfile.close()

  def read(self):
    bfile = open(filename, 'rb')
    print('Reading mesh of ' + ssname)
    print('### HEADER ###')
    m = bfile.read(isize)
    g = bfile.read(isize)
    e = bfile.read(isize)
    print('Mesh position: ' + str(struct.unpack('<I', m)[0]) + ' Gun position: ' + str(struct.unpack('<I', g)[0]) + ' Engine position: ' + str(struct.unpack('<I', e)[0]))
    print('### BODY ###')
    nv = bfile.read(usize)
    print(struct.unpack('<H', nv)[0])
    for v in self.data.vertices:
      x = bfile.read(fsize)
      y = bfile.read(fsize)
      z = bfile.read(fsize)
      w = bfile.read(fsize)
      r = bfile.read(fsize)
      g = bfile.read(fsize)
      b = bfile.read(fsize)
      a = bfile.read(fsize)
      print(struct.unpack('<f', x)[0], struct.unpack('<f', y)[0], struct.unpack('<f', z)[0], struct.unpack('<f', w)[0], struct.unpack('<f', r)[0], struct.unpack('<f', g)[0], struct.unpack('<f', b)[0], struct.unpack('<f', a)[0])
    ne = bfile.read(usize)
    print(struct.unpack('<H', ne)[0])
    for e in self.data.edges:
      e0 = bfile.read(usize)
      e1 = bfile.read(usize)
      print(struct.unpack('<H', e0)[0], struct.unpack('<H', e1)[0])

    print('Guns of ' + ssname + ':')
    ng = bfile.read(usize)
    print(struct.unpack('<H', ng)[0])
    c = 1
    for g in self.lguns:
      print('gun' + str(c) + ':')
      for i in range(0, 4):
        for j in range(0, 4):
          a = bfile.read(fsize)
          print(struct.unpack('<f', a)[0])
      c = c + 1

    print('Engines of ' + ssname + ':')
    ne = bfile.read(usize)
    print(struct.unpack('<H', ne)[0])
    c = 1
    for e in self.lengines:
      print('engine' + str(c) + ':')
      for i in range(0, 4):
        for j in range(0, 4):
          a = bfile.read(fsize)
          print(struct.unpack('<f', a)[0])
      c = c + 1
    bfile.close()


#Export SpaceShip coordenates
class Mesh:
  def __init__(self, data):
    self.data = data
    # Material vertex colour data
    #TODO: work for more than one material
    self.diffuse_color = self.data.materials[0].diffuse_color
    self.alpha =  self.data.materials[0].alpha
    #specular_color = self.data.materials[0].specular_color

  def export(self):
    bfile = open(filename, 'ab')
    bfile.write(struct.pack('<H', len(self.data.vertices)))
    for v in self.data.vertices:
      out = vertex_conv(v.co, scale)
      bfile.write(struct.pack('<f', out[0]))
      bfile.write(struct.pack('<f', out[1]))
      bfile.write(struct.pack('<f', out[2]))
      bfile.write(struct.pack('<f', out[3]))
      bfile.write(struct.pack('<f', self.diffuse_color[0]))
      bfile.write(struct.pack('<f', self.diffuse_color[1]))
      bfile.write(struct.pack('<f', self.diffuse_color[2]))
      bfile.write(struct.pack('<f', self.alpha))
    bfile.write(struct.pack('<H', len(self.data.edges)))
    for e in self.data.edges.values():
      bfile.write(struct.pack('<H', e.key[0]))
      bfile.write(struct.pack('<H', e.key[1]))
    bfile.close()

#Export guns positions
class Guns:
  def __init__(self, objMesh, lguns):
    self.lguns = lguns
    self.objMesh = objMesh

  def export(self):
    bfile = open(filename, 'ab')
    bfile.write(struct.pack('<H', len(self.lguns)))
    for g in self.lguns:
      t = matrix_gen(g.location - self.objMesh.location)
      for i in range(0, t.row_size):
        for j in range(0, t.col_size):
          bfile.write(struct.pack('<f', t[i][j]))
    bfile.close()

#Export Engines positions
class Engines:
  def __init__(self, objMesh, lengines):
    self.lengines = lengines
    self.objMesh = objMesh

  def export(self):
    bfile = open(filename, 'ab')
    bfile.write(struct.pack('<H', len(self.lengines)))
    for e in self.lengines:
      t = matrix_gen(e.location - self.objMesh.location)
      for i in range(0, t.row_size):
        for j in range(0, t.col_size):
          bfile.write(struct.pack('<f', t[i][j]))
    bfile.close()

########
# Main #
########
filename = ''
ssname = ''
if __name__ == "__main__":
  # objects in blender need be separeted per scenes
  #FIXME: In scenes engine and gun, cameras and lamps objects should be deleted
  listAllScenes = bpy.data.scenes
  objs = bpy.data.objects
  for o in objs:
    if(o.type == 'MESH'):
      filename = o.name + '.wire'
      ssname = o.name
  #FIXME: spaceship name(ssname) is global
  ss = SpaceShip(objs, listAllScenes)
  ss.create_header()
  ss.export()
  ss.read()
