#!/usr/bin/env -S nim --run c

import
    system, os

const
    lines = 1_000_000
    file = "test/noice.wolf"

var fLines: int
for line in lines(file):
    fLines += 1

iterator step(start, stop, step: int): int =
    var i = start
    while i < stop:
        yield i
        i += step

var contents = readFile(file)
for i in step(0, lines, fLines):
    echo contents