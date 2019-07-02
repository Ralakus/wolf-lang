# Package

version       = "0.1.0"
author        = "Ralakus"
description   = "A simple programming language"
license       = "MIT"
srcDir        = "src"
bin           = @["wolf"]
binDir        = "build"
backend       = "c"


# Dependencies

requires "nim >= 0.20.0" # , "msgpack4nim"