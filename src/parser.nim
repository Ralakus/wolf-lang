
import
    ast, lexer, util/log, strutils, constants, streams
export ast

type
    ParserState = object
        lex: Lexer
        previous, current: Token
        debugLevel: int
    ParseError* = Exception

proc raiseException(state: ptr ParserState, msg: string) =
    raise newException(ParseError, "line: " & $state.current.pos.line & ", col: " & $state.current.pos.col & " \n" & msg)

proc advance(state: ptr ParserState) =
    state.previous = state.current

    state.current = state.lex.scanNext()

    if state.debugLevel >= debugTokenPrintLevel:
        noticeln($state.current)

proc consume(state: ptr ParserState, kind: TokenKind, msg: string) = 
    if state.current.kind == kind:
        state.advance()
    else:
        state.raiseException(msg)

proc match(state: ptr ParserState, tokens: varargs[TokenKind]): bool = 
    for i in tokens:
        if state.current.kind == i:
            state.advance()
            return true
    false

proc atom(state: ptr ParserState): AstNode = 
    if state.match(tkNumber):
        return initNumberNode(($state.previous.data)[0..<state.previous.len].parseFloat())
    elif state.match(tkString):
        return initStringNode(($state.previous.data)[0..<state.previous.len])
    else:
        state.raiseException("Expected expression")

proc expression(state: ptr ParserState): AstNode =
    if state.match(tkLparen):
        if state.match(
            tkPlus, tkMinus, tkStar, tkSlash,
            tkEqualEqual, tkBangEqual,
            tkGreater, tkGreaterEqual,
            tkLess, tkLessEqual):
            let op = state.previous.kind
            result = initBinaryNode(op, state.expression(), state.expression())
            state.consume(tkRparen, "Expected closing ')'")
        elif state.match(tkBang):
            let op = state.previous.kind
            result = initUnaryNode(op, state.expression())
            state.consume(tkRparen, "Expected closing ')'")
        else:
            state.raiseException("Expected operator!")
    else:
        return state.atom()

proc parse*(source: ptr char, debugLevel: int): AstNode =
    var state = ParserState()
    state.debugLevel = debugLevel
    state.lex.initLexer(source)
    try:
        state.addr().advance()
        result = state.addr().expression()
        state.addr().consume(tkEof, "Expected EOF")
    except ParseError:
        let msg = getCurrentExceptionMsg()
        var parts = msg.split(' ')
        let eline = parts[1].split(',')[0].parseInt()
        let ecol = parts[3].parseInt()
        var sourceStream = newStringStream($source)
        var i = 1
        var line = ""
        while sourceStream.readLine(line):
            if i == eline:
                stderr.styledWrite(line, "\n")
            i += 1
        stderr.styledWrite(fgRed, repeat("~", ecol-1), "^", repeat("~", 4), "\n", resetStyle)
        stderr.styledWrite(msg, "\n")
        state.addr().raiseException(msg)
    except:
        stderr.styledWrite(getCurrentExceptionMsg())
        state.addr().raiseException(getCurrentExceptionMsg())
