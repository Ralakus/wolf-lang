#!/usr/bin/env python3

gen_to = int(input()) + 1
for i in range(1, gen_to):
    print((("\"{0:s}\"").format("{0:0" + str(i) + "d}")).format(i), end = '')
    if i != gen_to - 1:
        print(" + ", end = '')