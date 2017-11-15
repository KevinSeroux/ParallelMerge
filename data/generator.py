#!/bin/python

from __future__ import print_function
import sys
from random import randint

def generateOne(a, factor = 1):
    a = a + randint(factor * -10, 10)
    print(a, end=' ')
    return a

def generateSeveral(n):
    a = generateOne(0, 10)
    for i in range(1, n):
        a = generateOne(a, 0)
    print()

n = int(sys.argv[1]);
print(n)
generateSeveral(n)
generateSeveral(n)
