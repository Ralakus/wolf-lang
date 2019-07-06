
import
    lexer, memfiles, os, util/log, argparse, repl, parser, constants

proc main(): int =

    var
        bytecodeFlag, compileFlag, replFlag: bool = false
        helpFlag: bool = true
        outputOption: string = "out.cwlf"
        debugOption: string  = "1"
        inputArguments: seq[string] = @[]

    var argParser = newParser("Wolf version: " & CompileDate):
        help("Experimental Wolf language compiler version " & CompileDate)
        flag("-b", "--bytecode", help = "Input is bytecode")
        flag("-c", "--compile", help = "Compiles input to bytecode file")
        flag("-r", "--repl", help = "Overrides other flags and loads up repl environment")
        option("-o", "--output", help = "Output to this file", default = "out.cwlf")
        option("-d", "--debug", help = "Specifies debug level", choices = @["0", "1", "2", "3"], default = "0")
        arg("input", nargs = -1)

    let helpMsg = argParser.help()

    try:
        var args = argParser.parse(commandLineParams())
        bytecodeFlag = args.bytecode
        compileFlag = args.compile
        replFlag = args.repl
        helpFlag = args.help
        outputOption = args.output
        debugOption = args.debug
        inputArguments = args.input
    except:
        errorln(getCurrentExceptionMsg())
        stdout.styledWrite(helpMsg)
        return 1

    if helpFlag:
        return 0

    let debugLevel = case(debugOption):
        of "0":
            0
        of "1":
            1
        of "2":
            2
        of "3":
            3
        else:
            0

    if paramCount() < 1:
        errorln("Expected arguments!\n")
        echo helpMsg
        return 1

    if compileFlag and bytecodeFlag:
        errorln("Cannot compile bytecode to bytecode since it's already bytecode!")
        return 1

    if replFlag:
        if repl(debugLevel):
            return 0
        else:
            return 1

    if inputArguments.len < 1:
        errorln("Expected at least one input!")
        return 1 

    let inputFile = inputArguments[0]
    let inputFileSize = getFileSize(inputFile).int

    if debugLevel > 0:
        noticeln(inputFile, " size: ", $inputFileSize)

    var memMap: MemFile = memfiles.open(inputFile, allowRemap = true)
    defer:
        memMap.close()

    try:
        var ast = parse(cast[ptr char](memMap.mapMem(mappedSize = inputFileSize)), debugLevel)
        if debugLevel >= debugAstPrintLevel:
            echo $ast
    except:
        discard

    0

proc lexerTest(): int = 

    let inputFile = paramStr(1)
    let inputFileSize = getFileSize(inputFile).int

    var memMap: MemFile = memfiles.open(inputFile, allowRemap = true)
    defer:
        memMap.close()

    var lex = Lexer()
    lex.initLexer(cast[ptr char](cast[ptr char](memMap.mapMem(mappedSize = inputFileSize))))

    var tok: Token
    while tok.kind != tkEof:
        tok = lex.scanNext()

    0
    
proc parserTest(): int = 

    let inputFile = paramStr(1)
    let inputFileSize = getFileSize(inputFile).int

    var memMap: MemFile = memfiles.open(inputFile, allowRemap = true)
    defer:
        memMap.close()

    try:
        var ast = parse(cast[ptr char](memMap.mapMem(mappedSize = inputFileSize)), 0)
    except:
        discard

    0

when isMainModule:
    system.quit(main())
