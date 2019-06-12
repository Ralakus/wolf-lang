
import os
import streams

import wolf/util/common
import wolf/util/arg_parser
import wolf/util/logger
import wolf/wolf
import wolf/repl
import wolf/vm

proc main(): int =
    var paramParser: argParser
    let paramParserPtr = addr(paramParser)
    paramParserPtr.init()
    defer: paramParserPtr.free()

    var paramHelp: arg
    let paramHelpPtr = addr(paramHelp)
    paramHelpPtr.init("h", "help", "Displays this help message", false)
    paramParserPtr.addArg(paramHelpPtr)
    defer: paramHelpPtr.free()

    var paramDebug: arg
    let paramDebugPtr = addr(paramDebug)
    paramDebugPtr.init("d", "debug",
        "Enables debug printout from lexer, parser, and virtual machine",
        false)
    paramParserPtr.addArg(paramDebugPtr)
    defer: paramDebugPtr.free()

    var paramCompile: arg
    let paramCompilePtr = addr(paramCompile)
    paramCompilePtr.init("c", "compile",
        "Compiles source files into bytecode files", false)
    paramParserPtr.addArg(paramCompilePtr)
    defer: paramCompilePtr.free()

    var paramBytecode: arg
    let paramBytecodePtr = addr(paramBytecode)
    paramBytecodePtr.init("b", "bytecode",
        "Runs bytecode from input, does not lex or parse", false)
    paramParserPtr.addArg(paramBytecodePtr)
    defer: paramBytecodePtr.free()

    var paramOutput: arg
    let paramOutputPtr = addr(paramOutput)
    paramOutputPtr.init("o", "output", "Outputs to file", true)
    paramParserPtr.addArg(paramOutputPtr)
    defer: paramOutputPtr.free()

    var paramRepl: arg
    let paramReplPtr = addr(paramRepl)
    paramReplPtr.init("r", "repl", "Starts up in repl environment", false)
    paramParserPtr.addArg(paramReplPtr)
    defer: paramReplPtr.free()

    var commandArgs = commandLineParams()
    commandArgs.insert(getAppFilename(), 0)
    var args = commandArgs.allocCStringArray()
    defer: deallocCStringArray(args)

    if not paramParserPtr.parse((paramCount() + 1).cint, args):
        errorln("Failed to parse arguments!")
        return 1

    if paramCount() == 0:
        errorln("Expected arguments!")
        return 1

    if paramHelp.found:
        paramParserPtr.printHelp("Wolf language 0.1.0".cstring)
        return 0

    if paramRepl.found:
        var replArgs = replParams(debugMode: paramDebug.found)
        discard repl(addr(replArgs))
    else:
        var instance: wolfInstance
        let instancePtr = addr(instance)
        instancePtr.init()
        defer: instancePtr.free()

        instancePtr.setDebugMode(paramDebug.found)

        var fileName: cstring

        if paramParser.ea_len > 0:
            fileName = paramParser.extra_args[0]
        else:
            errorln("Expected input file!")
            return 1
        
        if paramBytecode.found:
            if not instancePtr.loadBytecodeFile(fileName):
                errorln("Failed to load bytecode file!")
                return 1
            if not instancePtr.run():
                errorln("Failed to execute bytecode!")
                return 1
        elif paramCompile.found:

            var output: cstring
            if paramOutput.found:
                output = paramOutput.preceeding_args[0]
            else:
                output = "out.cwlf"

            if not instancePtr.loadFile(fileName):
                errorln("Failed to load source file!")
                return 1

            var slen: csize = 0
            var serialized: ptr uint8 = wolf_bytecode_serialize(addr(instance.bytecode), addr(slen), true)
            discard instancePtr.unload()

            proc free(p: pointer) {.importc: "free".}
            free(serialized)

            var file = newFileStream($output, fmWrite)
            file.writeData(serialized, slen)
            file.close()

        else:
            if not instancePtr.loadFile(fileName):
                errorln("Failed to load source file!")
                return 1
            if not instancePtr.run():
                errorln("Failed to execute bytecode!")
                return 1

    return 0

when isMainModule:
    system.quit(main())