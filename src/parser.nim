
import
    ast, lexer, util/log, strutils, constants, streams, util/ptrmath
export ast

type
    ParserState = object
        lex: Lexer
        previous, current: Token
        debugLevel: int
    ParseError* = object of Exception

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

proc atom(state: ptr ParserState): AstNode
proc expression(state: ptr ParserState): AstNode
proc ifExpression(state: ptr ParserState): AstNode
proc letExpression(state: ptr ParserState): AstNode
proc defExpression(state: ptr ParserState): AstNode

proc atom(state: ptr ParserState): AstNode = 
    if state.match(tkNumber):
        return initNumberNode(strtod(state.previous.data, nil))

    elif state.match(tkString):
        var str = newString(state.previous.len)
        copyMem(addr(str[0]), state.previous.data + 1, state.previous.len - 2)
        return initStringNode(str)

    elif state.match(tkKwFalse):
        return initBoolNode(false)

    elif state.match(tkKwTrue):
        return initBoolNode(true)

    elif state.match(tkIdentifier):
        var name = newString(state.previous.len)
        copyMem(addr(name[0]), state.previous.data, state.previous.len)
        return initIdentifierNode(name)

    else:
        state.raiseException("Expected expression")

proc expression(state: ptr ParserState): AstNode =
    if state.match(tkLparen):
        if state.match(
            tkPlus, tkMinus, tkStar, tkSlash,
            tkEqualEqual, tkBangEqual,
            tkGreater, tkGreaterEqual,
            tkLess, tkLessEqual, tkEqual):
            let op = state.previous.kind
            result = initBinaryNode(op, state.expression(), state.expression())

        elif state.match(tkBang):
            let op = state.previous.kind
            result = initUnaryNode(op, state.expression())

        elif state.match(tkKwIf):
            result = state.ifExpression()

        elif state.match(tkKwLet):
            result = state.letExpression()

        elif state.match(tkKwDef):
            result = state.defExpression()

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

proc ifExpression(state: ptr ParserState): AstNode =
    let ifCond = state.expression()
    let ifExpr = state.expression()
    var elseifConds: seq[AstNode] = @[]
    var elseifExprs: seq[AstNode] = @[]
    var elseExpr: AstNode = nil
    while state.match(tkKwElse):
        if state.match(tkKwIf):
            elseifConds.add(state.expression())
            elseifExprs.add(state.expression())
        else:
            elseExpr = state.expression()
    
    return initIfNode(ifCond, ifExpr, elseifConds, elseifExprs, elseExpr)

proc letExpression(state: ptr ParserState): AstNode = 
    state.consume(tkIdentifier, "Expected identifier")
    var name = newString(state.previous.len)
    copyMem(addr(name[0]), state.previous.data, state.previous.len)
    var value: AstNode = nil
    if not state.check(tkRparen):
        value = state.expression()
    return initLetNode(name, value)

proc defExpression(state: ptr ParserState): AstNode =
    var name: string = ""
    if state.match(tkIdentifier):
        var name = newString(state.previous.len)
        copyMem(addr(name[0]), state.previous.data, state.previous.len)

    state.consume(tkLparen, "Expected '('")
    var params: seq[string] = @[]
    while state.match(tkIdentifier):
        var param = newString(state.previous.len)
        copyMem(addr(param[0]), state.previous.data, state.previous.len)
        params.add(param)
    state.consume(tkRparen, "Expected ')'")
    
    var body = state.expression()

    return initDefNode(name, params, body)

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
