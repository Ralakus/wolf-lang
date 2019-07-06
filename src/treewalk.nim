
import
    ast, lexer

type
    TreewalkValueKind* = enum
        tkvNumber
        tkvBool
        tkvString
    TreewalkValue* = object
        case kind: TreewalkValueKind
            of tkvNumber:
                numVal*: float64
            of tkvBool:
                boolVal*: bool
            of tkvString:
                strVal*: string
    TreewalkError* = Exception

proc `$`*(val: TreewalkValue): string = 
    case val.kind:
        of tkvNumber:
            return $val.numVal
        of tkvString:
            return val.strVal
        of tkvBool:
            return $val.boolVal

proc treewalk*(ast: AstNode): TreewalkValue =
    case ast.kind:
        of nkNumber:
            return TreewalkValue(kind: tkvNumber, numVal: ast.numberVal)
        of nkString:
            return TreewalkValue(kind: tkvString, strVal: ast.stringVal)
        of nkBool:
            return TreewalkValue(kind: tkvBool, boolVal: ast.boolVal)
        of nkUnary:
            case ast.unaryOp:
                of tkBang:
                    return TreewalkValue(kind: tkvBool, boolVal: not treewalk(ast.unaryExpression).boolVal)
                else:
                    raise newException(TreewalkError, "Invalid unary operator")
        of nkBinary:
            case ast.binaryOp:
                of tkPlus:
                    return TreewalkValue(kind: tkvNumber, numVal: treewalk(ast.binaryLeftExpression).numVal + treewalk(ast.binaryRightExpression).numVal)
                of tkMinus:
                    return TreewalkValue(kind: tkvNumber, numVal: treewalk(ast.binaryLeftExpression).numVal - treewalk(ast.binaryRightExpression).numVal)
                of tkStar:
                    return TreewalkValue(kind: tkvNumber, numVal: treewalk(ast.binaryLeftExpression).numVal * treewalk(ast.binaryRightExpression).numVal)
                of tkSlash:
                    return TreewalkValue(kind: tkvNumber, numVal: treewalk(ast.binaryLeftExpression).numVal / treewalk(ast.binaryRightExpression).numVal)
                of tkEqualEqual:
                    return TreewalkValue(kind: tkvBool, boolVal: treewalk(ast.binaryLeftExpression).numVal == treewalk(ast.binaryRightExpression).numVal)
                of tkLess:
                    return TreewalkValue(kind: tkvBool, boolVal: treewalk(ast.binaryLeftExpression).numVal < treewalk(ast.binaryRightExpression).numVal)
                of tkLessEqual:
                    return TreewalkValue(kind: tkvBool, boolVal: treewalk(ast.binaryLeftExpression).numVal <= treewalk(ast.binaryRightExpression).numVal)
                of tkGreater:
                    return TreewalkValue(kind: tkvBool, boolVal: treewalk(ast.binaryLeftExpression).numVal > treewalk(ast.binaryRightExpression).numVal)
                of tkGreaterEqual:
                    return TreewalkValue(kind: tkvBool, boolVal: treewalk(ast.binaryLeftExpression).numVal >= treewalk(ast.binaryRightExpression).numVal)
                else:
                    raise newException(TreewalkError, "Invalid binary operator")
        of nkList:
            for i in 0..ast.listExpressions.len() - 2:
                discard treewalk(ast.listExpressions[i])
            return treewalk(ast.listExpressions[ast.listExpressions.len() - 1])