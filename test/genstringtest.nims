#!/usr/bin/env nim 

import
    strutils

const
    genTo = 100

var str: string
for i in 1..genTo:
    str.add("\"" & repeat("0", i - ($i).len()) & $i & (if i==genTo: "\"" else: "\" + "))
echo str