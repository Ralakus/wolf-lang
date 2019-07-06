
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