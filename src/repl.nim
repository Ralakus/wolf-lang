
import
    lexer, util/log, strutils

proc strSplit(x: string): seq[string] =
    if x == ":":
        return @[]
    else:
        return x.split({' '})

proc repl*(debugLevelIn: int): bool =

    var debugLevel = debugLevelIn

    var lex = Lexer()
    var onFirstTok = true
    var onCommand = false

    while true:
        onFirstTok = true

        print("> ")
        let line = readLine(stdin)

        lex.initLexer(line[0].unsafeAddr())

        var tok: Token
        while tok.kind != TokenKind.eof:
            tok = lex.scanNext()

            if onFirstTok and tok.kind == TokenKind.colon:
                onCommand = true
                tok = lex.scanNext()
                if tok.kind == TokenKind.identifier:
                    var args = strSplit($(tok.data))
                    case args[0]:
                        of "exit":
                            return true
                        of "debug":
                            if args.len < 2:
                                debugLevel = 0
                            else:
                                debugLevel = args[1].parseInt().clamp(0, 3)
                        else:
                            errorln("Invalid command!")

            if debugLevel > 1 and not onCommand:
                noticeln($tok)

            onFirstTok = false

        onCommand = false

