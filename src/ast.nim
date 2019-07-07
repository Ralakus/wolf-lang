
import
    tokenkind, strutils

type
    AstNodeKind* = enum
        nkNumber
        nkString
        nkBool
        nkUnary
        nkBinary
        nkList
        nkIf
    AstNode* = ref object
        case kind*: AstNodeKind
            of nkNumber:
                numberVal*: float64
            of nkString:
                stringVal*: string
            of nkBool:
                boolVal*: bool
            of nkUnary:
                unaryOp*: TokenKind
                unaryExpression*: AstNode
            of nkBinary:
                binaryOp*: TokenKind
                binaryLeftExpression*, binaryRightExpression*: AstNode
            of nkList:
                listExpressions*: seq[AstNode]
            of nkIf:
                ifConditionExpresion*: AstNode
                ifExpression*: AstNode
                ifElseifConditionExpressions*: seq[AstNode]
                ifElseIfExpressions*: seq[AstNode]
                ifElseExpression*: AstNode

proc initNumberNode*(num: float64): AstNode =
    AstNode(kind: nkNumber, numberVal: num)

proc initStringNode*(str: string): AstNode =
    AstNode(kind: nkString, stringVal: str)

proc initBoolNode*(boolean: bool): AstNode =
    AstNode(kind: nkBool, boolVal: boolean)

proc initUnaryNode*(op: TokenKind, expression: AstNode): AstNode =
    AstNode(kind: nkUnary, unaryOp: op,  unaryExpression: expression)

proc initBinaryNode*(op: TokenKind, left, right: AstNode): AstNode =
    AstNode(kind: nkBinary, binaryOp: op, binaryLeftExpression: left, binaryRightExpression: right)

proc initListNode*(expressions: seq[AstNode]): AstNode =
    AstNode(kind: nkList, listExpressions: expressions)

proc initIfNode*(ifCond: AstNode, ifExpr: AstNode, elseIfConds: seq[AstNode], elseifExprs: seq[AstNode], elseExpr: AstNode): AstNode = 
    AstNode(kind: nkIf, ifConditionExpresion: ifCond, ifExpression: ifExpr, ifElseifConditionExpressions: elseIfConds, ifElseIfExpressions: elseifExprs, ifElseExpression: elseExpr)

proc `$`*(node: AstNode): string =
    case node.kind:
        of nkNumber:
            return $node.numberVal
        of nkString:
            return node.stringVal
        of nkBool:
            return $node.boolVal
        of nkBinary:
            return "(" & $node.binaryOp & " " & $node.binaryLeftExpression & " " & $node.binaryRightExpression & ")"
        of nkUnary:
            return "(" & $node.unaryOp & " " & $node.unaryExpression & ")"
        of nkList:
            result.add("(")
            for i in node.listExpressions:
                result.add($i)
                result.add(" ")
            result = result.strip()
            result.add(")")
            return result
        of nkIf:
            result.add("(")
            result.add("if ") 
            result.add($node.ifConditionExpresion)
            result.add(" ")
            result.add($node.ifExpression)
            var index = 0
            for i in node.ifElseIfExpressions:
                result.add(" elseif ")
                result.add($node.ifElseifConditionExpressions[index])
                result.add(" ")
                result.add($i)
                result.add(" ")
                index += 1
            if node.ifElseExpression != nil:
                result.add("else ")
                result.add($node.ifElseExpression)
            result = result.strip()
            result.add(")")