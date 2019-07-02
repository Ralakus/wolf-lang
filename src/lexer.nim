
import tokenkind
import util/ptrmath
import system
import strformat

type
    Position* = tuple[line: int32, col: int32]

type
    Token* = object
        kind*: TokenKind 
        data*: ptr char
        len*:  int
        pos*:  Position

proc `$`*(token: Token): string =
    if(token.kind != TokenKind.eof):
        return fmt"{$token.kind:<12}" & " : " & fmt"{($token.data)[0..<token.len]:<16}" & " : (" & $token.pos.line & "," & $token.pos.col & ")"
    else:
        return fmt"{$TokenKind.eof:<12}" & " : " & fmt"""{"\\0":<16}""" & " : (" & $token.pos.line & "," & $token.pos.col & ")"

type
    Lexer* = object
        start: ptr char
        current: ptr char
        pos: Position

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
    Token(kind: kind, data: lexer.start, len: lexer.current - lexer.start, pos: lexer.pos)

proc advance(lexer: var Lexer): char {.inline.} =
    lexer.pos.col += 1
    lexer.current += 1

    if lexer.current[-1] == '\n':
        lexer.pos.line += 1
        lexer.pos.col   = 1

    lexer.current[-1]

proc match(lexer: var Lexer, expected: char): bool {.inline.} =
    if lexer.isAtEnd():             return false
    if lexer.current[] != expected: return false

    discard lexer.advance()
    return true

proc skipWhitespace(lexer: var Lexer) {.inline.} = 
    while true:
        let c = lexer.peek()
        case c:
            of '\n': 
                lexer.pos.line += 1
                lexer.pos.col  = 1
                discard lexer.advance()
            of ' ', '\r', '\t':
                discard lexer.advance()
            of '#':
                while(lexer.peek() != '\n' and not lexer.isAtEnd() ):
                    discard lexer.advance()
            else:
                return

proc number(lexer: var Lexer): Token {.inline.} =
    while lexer.peek().isDigit():
        discard lexer.advance()
    
    if lexer.peek() == '.' and lexer.peek_next().isDigit():
        discard lexer.advance()

        while lexer.peek().isDigit():
            discard lexer.advance()

    lexer.makeToken(TokenKind.number)

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
                return lexer.makeToken(TokenKind.kwAnd)
        of 'b':
            if lexer.matchKeyword("break", 1):
                return lexer.makeToken(TokenKind.kwBreak)
        of 'e':
            if lexer.matchKeyword("else", 1):
                return lexer.makeToken(TokenKind.kwElse)
        of 'f':
            if lexer.current - lexer.start > 1:
                case lexer.start[1]:
                    of 'a':
                        if lexer.matchKeyword("false", 2):
                            return lexer.makeToken(TokenKind.kwFalse)
                    of 'o':
                        if lexer.matchKeyword("for", 2):
                            return lexer.makeToken(TokenKind.kwFor)
                    else:
                        return lexer.makeToken(TokenKind.identifier)

        of 'i':
            if lexer.matchKeyword("if", 1):
                return lexer.makeToken(TokenKind.kwIf)

        of 'n':
            if lexer.matchKeyword("nil", 1):
                return lexer.makeToken(TokenKind.kwNil)

        of 'o':
            if lexer.matchKeyword("or", 1):
                return lexer.makeToken(TokenKind.kwOr)
        
        of 'r':
            if lexer.matchKeyword("return", 1):
                return lexer.makeToken(TokenKind.kwReturn)

        of 's':
            if lexer.current - lexer.start > 1:
                case lexer.start[1]:
                    of 'e':
                        if lexer.matchKeyword("self", 2):
                            return lexer.makeToken(TokenKind.kwSelf)
                        if lexer.matchKeyword("struct", 2):
                            return lexer.makeToken(TokenKind.kwStruct)
                    else:
                        return lexer.makeToken(TokenKind.identifier)
        of 't':
            if lexer.matchKeyword("true", 1):
                return lexer.makeToken(TokenKind.kwTrue)

        of 'w':
            if lexer.matchKeyword("while", 1):
                return lexer.makeToken(TokenKind.kwWhile)
                
        else:
            return lexer.makeToken(TokenKind.identifier)

    lexer.makeToken(TokenKind.identifier)

proc string(lexer: var Lexer): Token {.inline.} =
    while lexer.peek() != '\"' and not lexer.isAtEnd():
        discard lexer.advance()
    
    if lexer.isAtEnd():
        return lexer.makeToken(TokenKind.err)

    discard lexer.advance()

    lexer.makeToken(TokenKind.string)

## Scans for next token in loaded string
proc scanNext*(lexer: var Lexer): Token {.inline.} =
    lexer.skipWhitespace()
    lexer.start = lexer.current

    if lexer.isAtEnd():
        discard lexer.advance()
        return lexer.makeToken(TokenKind.eof)

    let c = lexer.advance()
    if c.isAlpha():
        return lexer.identifier()
    if c.isDigit():
        return lexer.number()

    case c:
        of '(': return lexer.makeToken(TokenKind.lparen)
        of ')': return lexer.makeToken(TokenKind.rparen)
        of '{': return lexer.maketoken(TokenKind.lcurly)
        of '}': return lexer.maketoken(TokenKind.rcurly)
        of '[': return lexer.maketoken(TokenKind.lbracket)
        of ']': return lexer.maketoken(TokenKind.rbracket)

        of ',': return lexer.maketoken(TokenKind.comma)
        of '.': return lexer.maketoken(TokenKind.dot)
        of ';': return lexer.maketoken(TokenKind.semicolon)

        of '+': return lexer.maketoken(TokenKind.plus)
        of '-': return lexer.maketoken(TokenKind.minus)
        of '*': return lexer.maketoken(TokenKind.star)
        of '/': return lexer.maketoken(TokenKind.slash)

        of '!':
            return lexer.makeToken(if lexer.match('='): TokenKind.bangEqual else: TokenKind.bang)
        of '=':
            return lexer.makeToken(if lexer.match('='): TokenKind.equalEqual else: TokenKind.equal)
        of '<':
            return lexer.makeToken(if lexer.match('='): TokenKind.lessEqual else: TokenKind.less)
        of '>':
            return lexer.makeToken(if lexer.match('='): TokenKind.greaterEqual else: TokenKind.greater)
        of ':':
            return lexer.makeToken(if lexer.match(':'): TokenKind.colonColon else: TokenKind.colon)

        of '\"': return lexer.string()

        else: return lexer.makeToken(TokenKind.err)