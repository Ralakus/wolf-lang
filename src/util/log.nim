
import
    terminal, typetraits, system
export
    terminal

template print*(args: varargs[string, `$`]) = 
    stdout.styledWrite(resetStyle, "[ ]: ")
    for i in args:
        stdout.styledWrite(i)

template println*(args: varargs[string, `$`]) =
    print(args)
    stdout.styledWrite("\n")

template notice*(args: varargs[string, `$`]) =
    stdout.styledWrite(resetStyle, fgCyan, "[-]: ", resetStyle)
    for i in args:
        stdout.styledWrite(i)

template noticeln*(args: varargs[string, `$`]) =
    notice(args)
    stdout.styledWrite("\n")

template success*(args: varargs[string, `$`]) = 
    stdout.styledWrite(resetStyle, fgGreen,"[^]: ", resetStyle)
    for i in args:
        stdout.styledWrite(i)

template successln*(args: varargs[string, `$`]) =
    success(args)
    stdout.styledWrite("\n")

template warn*(args: varargs[string, `$`]) =
    stdout.styledWrite(resetStyle, fgGreen, "[*]: ", resetStyle)
    for i in args:
        stdout.styledWrite(i)

template warnln*(args: varargs[string, `$`]) =
    warn(args)
    stdout.styledWrite("\n")

template error*(args: varargs[string, `$`]) =
    stdout.styledWrite(resetStyle, fgRed, "[!]: ", resetStyle)
    for i in args:
        stdout.styledWrite(i)

template errorln*(args: varargs[string, `$`]) =
    error(args)
    stdout.styledWrite("\n")