
import
    lexer, memfiles, os, util/log, argparse, repl, parser, constants, treewalk, msgpack4nim

proc main(): int =

    var
        astFlag, compileFlag, replFlag: bool = false
        helpFlag: bool = true
        outputOption: string = "out.cwlf"
        debugOption: string  = "1"
        inputArguments: seq[string] = @[]

    var argParser = newParser("Wolf version: " & CompileDate):
        help("Experimental Wolf language compiler version " & CompileDate)
        flag("-a", "--ast", help = "Input is ast")
        flag("-c", "--compile", help = "Compiles input to ast file")
        flag("-r", "--repl", help = "Overrides other flags and loads up repl environment")
        option("-o", "--output", help = "Output to this file", default = "out.wst")
        option("-d", "--debug", help = "Specifies debug level", default = "0")
        arg("input", nargs = -1)

    let helpMsg = argParser.help()

    try:
        var args = argParser.parse(commandLineParams())
        astFlag = args.ast
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

    let debugLevel = debugOption.parseInt().clamp(0, 4)

    if paramCount() < 1:
        errorln("Expected arguments!\n")
        echo helpMsg
        return 1

    if compileFlag and astFlag:
        errorln("Cannot compile bytecode to AST since it's already bytecode!")
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

    var ast: AstNode = nil
    try:
        if astFlag:
            var msgAst = newString(inputFileSize)
            copyMem(msgAst[0].addr(),memMap.mapMem(mappedSize = inputFileSize), inputFileSize)
            unpack(msgAst, ast)
        else:
            ast = parse(cast[ptr char](memMap.mapMem(mappedSize = inputFileSize)), debugLevel)
        if debugLevel >= debugAstPrintLevel:
            echo $ast
        if not compileFlag:
            var env = TreewalkEnvironment()
            var value = env.treewalk(ast)
            success()
            stdout.styledWriteLine(fgGreen, "=> ", styleBright, $value, resetStyle)
        else:
            outputOption.writeFile(pack(ast))
    except ParseError:
        return 2
    except:
        stderr.styledWriteLine(getCurrentExceptionMsg())
        return 1
        

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
