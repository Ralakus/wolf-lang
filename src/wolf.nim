# This is just an example to get you started. A typical binary package
# uses this file as the main entry point of the application.

import wolf/wolf as wolf

var w = wolf.wolf_t()
var wptr = addr(w)

wptr.init()

wptr.setDebugMode(true)

discard wptr.loadFile("test/little.wolf".cstring)

discard wptr.run()

wptr.free()