
switch("cc", "clang")
switch("verbosity", "2")
switch("gc", "markAndSweep")
switch("debugger", "native")

switch("nimcache", "build/cache")

switch("verbosity", "2")

when defined(release):
    switch("opt", "speed")
    switch("checks", "off")
    switch("stackTrace", "off")
    switch("lineTrace", "off")

when defined(debug):
    switch("checks", "on")
    switch("stackTrace", "on")
    switch("lineTrace", "on")
    switch("opt", "none")

