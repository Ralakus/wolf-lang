
import
    ast, lexer, util/log

type
    ParserState = object
        previous, current: Token
        lex: Lexer
        prefixNode: AstNode
        debugLevel: int
    UnexpectedToken = Exception
    Prec = enum
        precNone,
        precAssignment,  # =
        precOr,          # or
        precAnd,         # and
        precEquality,    # == !=
        precComparison,  # < > <= >=
        precTerm,        # + -
        precFactor,      # * /
        precUnary,       # ! -
        precCall,        # . () []
        precPrimary
    ParseFn = proc(state: ref ParserState): AstNode
    ParseRule = object
        prefix: ParseFn
        infix: ParseFn
        precedence: Prec

proc advance(state: ref ParserState) = 
    state.previous = state.current

    state.current = state.lex.scanNext()

    if state.debugLevel > 1:
        noticeln($state.current)

proc consume(state: ref ParserState, kind: TokenKind, msg: string) = 
    if state.current.kind == kind:
        state.advance()
    else:
        raise newException(UnexpectedToken, msg)

proc expression(state: ref ParserState): AstNode
proc literal(state: ref ParserState): AstNode
proc unary(state: ref ParserState): AstNode
proc binary(state: ref ParserState): AstNode
proc grouping(state: ref ParserState): AstNode

const rules: array[tkEof.int + 1, ParseRule] = [
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkLparen
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkRparen
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkLcurly
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkRcurly
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkLbracket
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkRbracket
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkComma
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkDot
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkSemicolon
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkPlus
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkMinus
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkStar
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkSlash
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkBang
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkBangEqual
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkEqual
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkEqualEqual
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkGreater
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkGreaterEqual
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkLess
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkLessEqual
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkColon
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkColonColon
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkIdentifier
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkString
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkInterpolation
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkNumber
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkKwAnd
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkKwOr
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkKwSelf
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkKwStruct
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkKwReturn
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkKwIf
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkKwElse
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkKwWhile
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkKwFor
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkKwBreak
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkKwTrue
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkKwFalse
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkKwNil
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkErr
    ParseRule(prefix: nil, infix: nil, precedence: Prec.precNone), # tkEof
]


proc expression(state: ref ParserState): AstNode =
    nil
proc literal(state: ref ParserState): AstNode =
    nil
proc unary(state: ref ParserState): AstNode =
    nil
proc binary(state: ref ParserState): AstNode = 
    nil
proc grouping(state: ref ParserState): AstNode =
    nil