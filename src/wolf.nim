
import tokentypes
import system
import lexer

proc main(): int =

    let source = readLine(stdin)

    var lex = Lexer()
    lex.initLexer(source[0].unsafeAddr)

    var tok: Token
    while tok.kind != TokenType.eof:
        tok = lex.scanNext()
        echo $tok

    0

when isMainModule:
    system.quit(main())