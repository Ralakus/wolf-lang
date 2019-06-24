#!/usr/bin/env python3

import sys, os

file = None

try:
    file = open("little.wolf")
except:
    try:
        file = open("test/little.wolf")
    except:
        pass

test = file.readline()

for i in range(0, 999_999):
    print(test, " == ")
print(test, end = '')