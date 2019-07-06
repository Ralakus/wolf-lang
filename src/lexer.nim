
import
    tokenkind, util/ptrmath, system, strformat
export
    tokenkind

type
    Position* = tuple[line: int32, col: int32]

type
    Token* = object
        kind*: TokenKind 
        data*: ptr char
        len*:  int
        pos*:  Position

proc `$`*(token: Token): string =
    if(token.kind != tkEof):
        return fmt"{$token.kind:<12}" & " : " & fmt"{($token.data)[0..<token.len]:<16}" & " : (" & $token.pos.line & "," & $token.pos.col & ")"
    else:
        return fmt"{$tkEof:<12}" & " : " & fmt"""{"\\0":<16}""" & " : (" & $token.pos.line & "," & $token.pos.col & ")"

type
    Lexer* = object
        start: ptr char
        current: ptr char
        pos: Position
        startPos: Position

proc initLexer*(lexer: var Lexer, source: ptr char) {.inline.} =
    lexer.pos = (1.int32, 1.int32)
    lexer.start = source
    lexer.current = lexer.start

func isAtEnd(lexer: Lexer): bool {.inline.} =
    lexer.current[] == '\0'

func peek(lexer: Lexer): char {.inline.} =
    lexer.current[]

func peek_next(lexer: Lexer): char {.inline.} =
    lexer.current[1]

func isDigit(c: char): bool {.inline.} =
    c >= '0' and c <= '9'

func isAlpha(c: char): bool {.inline.} =
    (c >= 'a' and c <= 'z') or
    (c >= 'A' and c <= 'Z') or
    (c == '_')

func makeToken(lexer: Lexer, kind: TokenKind): Token {.inline.} =
    Token(kind: kind, data: lexer.start, len: lexer.current - lexer.start, pos: lexer.startPos)

proc advance(lexer: var Lexer): char {.inline.} =
    lexer.pos.col += 1
    lexer.current += 1

    lexer.current[-1]

proc match(lexer: var Lexer, expected: char): bool {.inline.} =
    if lexer.isAtEnd():             return false
    if lexer.current[] != expected: return false

    discard lexer.advance()
    true

proc skipWhitespace(lexer: var Lexer) {.inline.} = 
    while true:
        let c = lexer.peek()
        case c:
            of '\n': 
                lexer.pos.line += 1
                lexer.pos.col   = 0
                discard lexer.advance()
            of ' ', '\r', '\t':
                discard lexer.advance()
            of ';':
                if lexer.match(';'):
                    while(lexer.peek() != '\n' and not lexer.isAtEnd() ):
                        discard lexer.advance()
                else:
                    return
            else:
                return

proc number(lexer: var Lexer): Token {.inline.} =
    while lexer.peek().isDigit():
        discard lexer.advance()
    
    if lexer.peek() == '.' and lexer.peek_next().isDigit():
        discard lexer.advance()

        while lexer.peek().isDigit():
            discard lexer.advance()

    lexer.makeToken(tkNumber)

func matchKeyword(lexer: Lexer, str: string, depth: int): bool {.inline.} =
    if(lexer.current - lexer.start == str.len()):
        return ($lexer.start)[depth..<str.len()] == str[depth..^1]
    else:
        return false

proc identifier(lexer: var Lexer): Token {.inline.} = 
    while lexer.peek().isAlpha() or lexer.peek().isDigit():
        discard lexer.advance()

    case lexer.start[0]:
        of 'a':
            if lexer.matchKeyword("and", 1):
                return lexer.makeToken(tkKwAnd)
        of 'b':
            if lexer.matchKeyword("break", 1):
                return lexer.makeToken(tkKwBreak)
        of 'c':
            if lexer.matchKeyword("call", 1):
                return lexer.makeToken(tkKwCall)
        of 'e':
            if lexer.matchKeyword("elseif", 1):
                return lexer.makeToken(tkKwElseif)
            elif lexer.matchKeyword("else", 1):
                return lexer.makeToken(tkKwElse)
        of 'f':
            if lexer.current - lexer.start > 1:
                case lexer.start[1]:
                    of 'a':
                        if lexer.matchKeyword("false", 2):
                            return lexer.makeToken(tkKwFalse)
                    of 'o':
                        if lexer.matchKeyword("for", 2):
                            return lexer.makeToken(tkKwFor)
                    of 'r':
                        if lexer.matchKeyword("from", 2):
                            return lexer.makeToken(tkKwFrom)
                    else:
                        return lexer.makeToken(tkIdentifier)

        of 'i':
            if lexer.current - lexer.start > 1:
                case lexer.start[1]:
                    of 'f':
                        if lexer.matchKeyword("if", 2):
                            return lexer.makeToken(tkKwIf)
                    of 'm':
                        if lexer.matchKeyword("import", 2):
                            return lexer.makeToken(tkKwImport)
                    else:
                        return lexer.makeToken(tkIdentifier)

        of 'n':
            if lexer.matchKeyword("nil", 1):
                return lexer.makeToken(tkKwNil)

        of 'o':
            if lexer.matchKeyword("or", 1):
                return lexer.makeToken(tkKwOr)
        
        of 'r':
            if lexer.matchKeyword("return", 1):
                return lexer.makeToken(tkKwReturn)

        of 's':
            if lexer.current - lexer.start > 1:
                case lexer.start[1]:
                    of 'e':
                        if lexer.matchKeyword("self", 2):
                            return lexer.makeToken(tkKwSelf)
                        if lexer.matchKeyword("struct", 2):
                            return lexer.makeToken(tkKwStruct)
                    else:
                        return lexer.makeToken(tkIdentifier)
        of 't':
            if lexer.matchKeyword("true", 1):
                return lexer.makeToken(tkKwTrue)

        of 'w':
            if lexer.matchKeyword("while", 1):
                return lexer.makeToken(tkKwWhile)
                
        else:
            return lexer.makeToken(tkIdentifier)

    lexer.makeToken(tkIdentifier)

proc string(lexer: var Lexer): Token {.inline.} =
    while lexer.peek() != '\"' and not lexer.isAtEnd():
        discard lexer.advance()
    
    if lexer.isAtEnd():
        return lexer.makeToken(tkErr)

    discard lexer.advance()

    lexer.makeToken(tkString)

## Scans for next token in loaded string
proc scanNext*(lexer: var Lexer): Token {.inline.} =
    lexer.skipWhitespace()
    lexer.start = lexer.current
    lexer.startPos = lexer.pos

    if lexer.isAtEnd():
        discard lexer.advance()
        return lexer.makeToken(tkEof)

    let c = lexer.advance()
    if c.isAlpha():
        return lexer.identifier()
    if c.isDigit():
        return lexer.number()

    case c:
        of '(': return lexer.makeToken(tkLparen)
        of ')': return lexer.makeToken(tkRparen)
        of '{': return lexer.maketoken(tkLcurly)
        of '}': return lexer.maketoken(tkRcurly)
        of '[': return lexer.maketoken(tkLbracket)
        of ']': return lexer.maketoken(tkRbracket)

        of ',': return lexer.maketoken(tkComma)
        of '.': return lexer.maketoken(tkDot)
        of ';': return lexer.maketoken(tkSemicolon)

        of '+': return lexer.maketoken(tkPlus)
        of '-': return lexer.maketoken(tkMinus)
        of '*': return lexer.maketoken(tkStar)
        of '/': return lexer.maketoken(tkSlash)

        of '!':
            return lexer.makeToken(if lexer.match('='): tkBangEqual else: tkBang)
        of '=':
            return lexer.makeToken(if lexer.match('='): tkEqualEqual else: tkEqual)
        of '<':
            return lexer.makeToken(if lexer.match('='): tkLessEqual else: tkLess)
        of '>':
            return lexer.makeToken(if lexer.match('='): tkGreaterEqual else: tkGreater)
        of ':':
            return lexer.makeToken(if lexer.match(':'): tkColonColon else: tkColon)

        of '\"': return lexer.string()

        else: return lexer.makeToken(tkErr)