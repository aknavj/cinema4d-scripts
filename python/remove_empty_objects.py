#!/bin/c4d - Welcome to the world of Python
import c4d
from c4d import utils

objects = []

def recurse_hierarchy(op):
    while op:
        if  op.IsInstanceOf(c4d.Opolygon):
            if op.GetPolygonCount() == 0:
                objects.append(op)
        recurse_hierarchy(op.GetDown())
        op = op.GetNext()

def main():
    op = doc.GetFirstObject()
    recurse_hierarchy(op)
    for op in objects:
        op.Remove()
    pass
