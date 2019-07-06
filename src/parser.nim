
import
    ast, lexer, util/log, strutils, constants, streams, util/ptrmath
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

proc check(state: ptr ParserState, tokens: varargs[TokenKind]): bool =
    for i in tokens:
        if state.current.kind == i:
            return true
    false

proc match(state: ptr ParserState, tokens: varargs[TokenKind]): bool = 
    for i in tokens:
        if state.current.kind == i:
            state.advance()
            return true
    false

proc strtod(str: ptr char, strEnd: ptr ptr char): float64 {.importc: "strtod", header: "<string.h>".}

proc atom(state: ptr ParserState): AstNode = 
    if state.match(tkNumber):
        return initNumberNode(strtod(state.previous.data, nil))
    elif state.match(tkString):
        var str = newString(state.previous.len)
        copyMem(addr(str[0]), state.previous.data, state.previous.len)
        return initStringNode(str)
    elif state.match(tkKwFalse):
        return initBoolNode(false)
    elif state.match(tkKwTrue):
        return initBoolNode(true)
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
        elif state.match(tkBang):
            let op = state.previous.kind
            result = initUnaryNode(op, state.expression())
        else:
            var list: seq[AstNode] = @[]
            while not state.check(tkRparen):
                if state.check(tkLparen):
                    list.add(state.expression())
                else:
                    list.add(state.atom())
            result = initListNode(list)
        state.consume(tkRparen, "Expected closing ')'")
    else:   
        return state.atom()

proc parse*(source: ptr char, debugLevel: int): AstNode =
    var state = ParserState(debugLevel: debugLevel)
    state.lex.initLexer(source)
    try:
        state.addr().advance()
        var list: seq[AstNode] = @[]
        while not state.addr().check(tkRparen, tkEof):
            if state.addr().check(tkLparen):
                list.add(state.addr().expression())
            else:
                list.add(state.addr().atom())
        result = initListNode(list)
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
                stderr.styledWrite(fgRed, repeat("~", ecol-1), "^", repeat("~", line.len() - ecol), "\n", resetStyle)
                stderr.styledWrite(msg, "\n")
                break
            i += 1
        state.addr().raiseException(msg)
    except:
        stderr.styledWrite(getCurrentExceptionMsg())
        state.addr().raiseException("Misc: " & getCurrentExceptionMsg())
