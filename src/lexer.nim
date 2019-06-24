
import tokentypes
import util/ptrmath
import system

type
    Position* = tuple[line: int32, col: int32]

type
    Token* = object
        kind*: TokenType 
        data*: ptr char
        len*:  int
        pos*:  Position

proc `$`*(token: Token): string =
    if(token.kind != TokenType.eof):
        return $token.kind & " : " & ($token.data)[0..<token.len] & " : (" & $token.pos.line & "," & $token.pos.col & ")"
    else:
        return $TokenType.eof & " : '\\0' : (" & $token.pos.line & "," & $token.pos.col & ")"

type
    Lexer* = object
        start: ptr char
        current: ptr char
        pos: Position

proc initLexer*(lexer: var Lexer, source: ptr char) =
    lexer.pos = (1.int32, 1.int32)
    lexer.start = source
    lexer.current = lexer.start

func isAtEnd(lexer: Lexer): bool {.inline.} =
    lexer.current[] == '\0'

func peek(lexer: Lexer): char {.inline.} =
    lexer.current[]

func isDigit(c: char): bool =
    c >= '0' and c <= '9'

func isAlpha(c: char): bool =
    (c >= 'a' and c <= 'z') or
    (c >= 'A' and c <= 'Z') or
    (c == '_')

func makeToken(lexer: Lexer, kind: TokenType): Token =
    Token(kind: kind, data: lexer.start, len: lexer.current - lexer.start, pos: lexer.pos)

proc advance(lexer: var Lexer): char {.inline.} =
    lexer.pos.col += 1
    lexer.current += 1
    lexer.current[-1]

proc skipWhitespace(lexer: var Lexer) = 
    while true:
        let c = lexer.peek()
        case c:
            of '\n': 
                lexer.pos.line += 1
                lexer.pos.col  = 1
            of ' ', '\r', '\t':
                discard lexer.advance()
            of '#':
                while(lexer.peek() != '\n' and not lexer.isAtEnd() ):
                    discard lexer.advance()
            else:
                return

proc number(lexer: var Lexer): Token =
    nil

proc identifier(lexer: var Lexer): Token = 
    while(lexer.peek().isAlpha() or lexer.peek().isDigit()):
        discard lexer.advance()
    lexer.makeToken(TokenType.identifier)


proc string(lexer: var Lexer): Token =
    nil

## Scans for next token in loaded string
proc scanNext*(lexer: var Lexer): Token =
    lexer.skipWhitespace()
    lexer.start = lexer.current

    if lexer.isAtEnd():
        discard lexer.advance()
        return lexer.makeToken(TokenType.eof)

    let c = lexer.advance()
    if c.isAlpha():
        return lexer.identifier()
    if c.isDigit():
        return lexer.number()

