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
lines = 1_000_000

"""for i in range(0, lines - 1):
    print("(==", test)
print(test, end = '')
for i in range(0, lines - 1):
    print(")", end = '')"""
for i in range(0, lines - 1):
    print(test)
print(test, end = '')