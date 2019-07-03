
import
    tokenkind

type
    AstNodeKind* = enum
        nkNumberVal
        nkStringVal
        nkUnary
        nkBinary
        nkGrouping
    AstNode* = ref object
        case kide: AstNodeKind
            of nkNumberVal:
                numberVal: float64
            of nkStringVal:
                stringVal: string
            of nkUnary:
                unaryOp: TokenKind
                expression: AstNode
            of nkBinary:
                binaryOp: TokenKind
                leftExpression, rightExpression: AstNode
            of nkGrouping:
                groupedExpression: AstNode
