
import
    system, lexer, memfiles, os, util/log, argparse, repl

proc main(): int =

    var argParser = newParser("Wolf version 0.1.0"):
        help("Experimental Wolf language compiler version 0.1.0")
        flag("-b", "--bytecode", help = "Input is bytecode")
        flag("-c", "--compile", help = "Compiles input to bytecode file")
        flag("-r", "--repl", help = "Overrides other flags and loads up repl environment")
        option("-o", "--output", help = "Output to this file", default = "out.cwlf")
        option("-d", "--debug", help = "Specifies debug level", choices = @["0", "1", "2", "3"], default = "0")
        arg("input", nargs = -1)

    var args = argParser.parse(commandLineParams())
    if args.help:
        return 0

    let debugLevel = case(args.debug):
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
        echo argParser.help()
        return 1

    if args.compile and args.bytecode:
        errorln("Cannot compile bytecode to bytecode since it's already bytecode!")
        return 1

    if args.repl:
        if repl(debugLevel):
            return 0
        else:
            return 1

    if args.input.len < 1:
        errorln("Expected at least one input!")
        return 1

    let inputFile = args.input[0]
    let inputFileSize = getFileSize(inputFile).int

    if debugLevel > 0:
        noticeln(inputFile, " size: ", $inputFileSize)


    var memMap: MemFile = memfiles.open(inputFile, allowRemap = true)
    defer:
        memMap.close()

    var lex = Lexer()
    lex.initLexer(cast[ptr char](memMap.mapMem(mappedSize = inputFileSize)))

    if debugLevel > 1:
        noticeln("Tokens: ")

    var tok: Token
    while tok.kind != tkEof:
        tok = lex.scanNext()
        if debugLevel > 1:
            noticeln($tok)

    0

when isMainModule:
    system.quit(main())