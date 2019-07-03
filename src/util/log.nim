
import
    terminal, typetraits, system
export
    terminal

proc print*(args: varargs[string, `$`]) = 
    stdout.resetAttributes()
    stdout.write("[ ]: ")
    for i in args:
        stdout.write(i)

proc println*(args: varargs[string, `$`]) =
    print(args)
    stdout.write("\n")

proc notice*(args: varargs[string, `$`]) =
    setForegroundColor(fgCyan)
    stdout.write("[-]: ")
    stdout.resetAttributes()
    for i in args:
        stdout.write(i)

proc noticeln*(args: varargs[string, `$`]) =
    notice(args)
    stdout.write("\n")

proc success*(args: varargs[string, `$`]) = 
    setForegroundColor(fgGreen)
    stdout.write("[^]: ")
    stdout.resetAttributes()
    for i in args:
        stdout.write(i)

proc successln*(args: varargs[string, `$`]) =
    success(args)
    stdout.write("\n")

proc warn*(args: varargs[string, `$`]) =
    setForegroundColor(fgYellow)
    stdout.write("[*]: ")
    stdout.resetAttributes()
    for i in args:
        stdout.write(i)

proc warnln*(args: varargs[string, `$`]) =
    warn(args)
    stdout.write("\n")

proc error*(args: varargs[string, `$`]) =
    setForegroundColor(fgRed)
    stdout.write("[!]: ")
    stdout.resetAttributes()
    for i in args:
        stdout.write(i)

proc errorln*(args: varargs[string, `$`]) =
    error(args)
    stdout.write("\n")