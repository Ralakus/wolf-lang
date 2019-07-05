
import
    lexer, util/log, strutils, parser, constants, os

proc strSplit(x: string): seq[string] =
    if x == ":":
        return @[]
    else:
        return x.split({' '})

proc repl*(debugLevelIn: int): bool =

    var debugLevel = debugLevelIn

    var cmdLex = Lexer()
    var onFirstTok = true
    var onCommand = false

    while true:
        onFirstTok = true

        print("> ")
        let line = readLine(stdin)

        cmdLex.initLexer(line[0].unsafeAddr())

        var tok: Token
        while tok.kind != tkEof:
            tok = cmdLex.scanNext()

            if onFirstTok and tok.kind == tkColon:
                onCommand = true
                tok = cmdLex.scanNext()
                if tok.kind == tkIdentifier:
                    var args = strSplit($(tok.data))
                    case args[0]:
                        of "exit":
                            return true
                        of "debug":
                            if args.len < 2:
                                debugLevel = 0
                            else:
                                debugLevel = args[1].parseInt().clamp(0, 3)
                        of "clear":
                            eraseScreen()
                            cursorUp(high(int))
                        of "sys":
                            if args.len < 2:
                                errorln("Expected arguments for \'sys\' command")
                            else:
                                tok = cmdLex.scanNext()
                                if execShellCmd($(tok.data)) != 0:
                                    errorln("Failed to execute \'", $(tok.data), "\'")
                        else:
                            errorln("Invalid command")

            onFirstTok = false

        if not onCommand:
            try:
                var ast = parse(line[0].unsafeAddr(), debugLevel)
                if debugLevel >= debugAstPrintLevel:
                    echo $ast
            except:
                discard

        onCommand = false

