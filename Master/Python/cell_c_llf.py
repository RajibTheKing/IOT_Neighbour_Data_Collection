#!/usr/local/bin/python3

# "data structure" to store necessary values for C-LLF (tuples are to immutable for me :D)
class Cell_C_LLF(object):
  def __init__(self, flow, cell):
    self.flow = flow
    self.cell = cell
    self.laxity = -1