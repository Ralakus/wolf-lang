
import
    ast, tokenkind, strutils, tables, util/log

type
    TreewalkDef* = tuple[params: seq[string], body: AstNode]
    TreewalkValueKind* = enum
        tkvNumber
        tkvBool
        tkvString
        tkvDef
    TreewalkValue* = object
        case kind: TreewalkValueKind
            of tkvNumber:
                numVal*: float64
            of tkvBool:
                boolVal*: bool
            of tkvString:
                strVal*: string
            of tkvDef:
                defName*: string
                def*: TreewalkDef
    TreewalkError* = object of Exception
    TreewalkEnvironment* = ref object
        parentEnv: TreewalkEnvironment
        vars: Table[string, TreewalkValue]
        defs: Table[string, TreewalkDef]


proc `$`*(val: TreewalkValue): string = 
    case val.kind:
        of tkvNumber:
            return $val.numVal
        of tkvString:
            return val.strVal
        of tkvBool:
            return $val.boolVal
        of tkvDef:
            discard

proc `==`*(l, r: TreewalkValue): bool =
    if l.kind != r.kind:
        return false
    else:
        case l.kind:
            of tkvBool:
                return l.boolVal == r.boolVal
            of tkvNumber:
                return l.numVal == r.numVal
            of tkvString:
                return l.strVal == r.strVal
            of tkvDef:
                return false

proc `+`*(l, r: TreewalkValue): TreewalkValue = 
    if l.kind != r.kind:
        return TreewalkValue(kind: tkvBool, boolVal: false)
    else:
        case l.kind:
            of tkvBool:
                raise newException(TreewalkError, "Cannot add two boolean values")
            of tkvNumber:
                return TreewalkValue(kind: tkvNumber, numVal: l.numVal + r.numVal)
            of tkvString:
                return TreewalkValue(kind: tkvString, strVal: l.strVal & r.strVal)
            of tkvDef:
                raise newException(TreewalkError, "Cannot add two function values")

proc treewalk*(env: var TreewalkEnvironment, ast: AstNode): TreewalkValue =
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
                    return TreewalkValue(kind: tkvBool, boolVal: not treewalk(env, ast.unaryExpression).boolVal)
                else:
                    raise newException(TreewalkError, "Invalid unary operator")
        of nkBinary:
            case ast.binaryOp:
                of tkPlus:
                    return treewalk(env, ast.binaryLeftExpression) + treewalk(env, ast.binaryRightExpression)
                of tkMinus:
                    return TreewalkValue(kind: tkvNumber, numVal: treewalk(env, ast.binaryLeftExpression).numVal - treewalk(env, ast.binaryRightExpression).numVal)
                of tkStar:
                    return TreewalkValue(kind: tkvNumber, numVal: treewalk(env, ast.binaryLeftExpression).numVal * treewalk(env, ast.binaryRightExpression).numVal)
                of tkSlash:
                    return TreewalkValue(kind: tkvNumber, numVal: treewalk(env, ast.binaryLeftExpression).numVal / treewalk(env, ast.binaryRightExpression).numVal)
                of tkEqualEqual:
                    return TreewalkValue(kind: tkvBool, boolVal: treewalk(env, ast.binaryLeftExpression) == treewalk(env, ast.binaryRightExpression))
                of tkLess:
                    return TreewalkValue(kind: tkvBool, boolVal: treewalk(env, ast.binaryLeftExpression).numVal < treewalk(env, ast.binaryRightExpression).numVal)
                of tkLessEqual:
                    return TreewalkValue(kind: tkvBool, boolVal: treewalk(env, ast.binaryLeftExpression).numVal <= treewalk(env, ast.binaryRightExpression).numVal)
                of tkGreater:
                    return TreewalkValue(kind: tkvBool, boolVal: treewalk(env, ast.binaryLeftExpression).numVal > treewalk(env, ast.binaryRightExpression).numVal)
                of tkGreaterEqual:
                    return TreewalkValue(kind: tkvBool, boolVal: treewalk(env, ast.binaryLeftExpression).numVal >= treewalk(env, ast.binaryRightExpression).numVal)
                of tkEqual:
                    if ast.binaryLeftExpression.kind != nkIdentifier:
                        raise newException(TreewalkError, "Expected binary left for assignment to be identifier")
                    else:
                        if env.vars.hasKey(ast.binaryLeftExpression.identName):
                            env.vars[ast.binaryLeftExpression.identName] = treewalk(env, ast.binaryRightExpression)
                            return env.vars[ast.binaryLeftExpression.identName]
                        else:
                            raise newException(TreewalkError, "Variable " & ast.binaryLeftExpression.identName & " not defined")
                else:
                    raise newException(TreewalkError, "Invalid binary operator")
        of nkList:
            if ast.listExpressions.len() > 0:
                if ast.listExpressions[0].kind == nkIdentifier: # Function call
                    var name = ast.listExpressions[0].identName
                    var subEnv = TreewalkEnvironment(parentEnv: env)
                    if env.defs.hasKey(name):
                        if ast.listExpressions.len() - 1 == env.defs[name].params.len():
                            for i in 1..ast.listExpressions.len() - 1:
                                subEnv.vars[env.defs[name].params[i-1]] = treewalk(env, ast.listExpressions[i])
                            return treewalk(subEnv, env.defs[name].body)
                        else:
                            raise newException(TreewalkError, $(ast.listExpressions.len() - 1) & " arguments are given but " & name & " expects " & $(env.defs[name].params.len()) & " arguments")
                    else:
                        if env.vars.hasKey(ast.listExpressions[0].identName):
                            return env.vars[ast.listExpressions[0].identName]
                        else:
                            raise newException(TreewalkError, "Variable " & ast.listExpressions[0].identName & " not defined")
                else:
                    for i in 0..ast.listExpressions.len() - 2:
                        discard treewalk(env, ast.listExpressions[i])
                    return treewalk(env, ast.listExpressions[ast.listExpressions.len() - 1])
            else:
                return TreewalkValue(kind: tkvBool, boolVal: false)
        of nkIf:
            if treewalk(env, ast.ifConditionExpresion).boolVal:
                return treewalk(env, ast.ifExpression)
            else:
                var index = 0
                for i in ast.ifElseifConditionExpressions:
                    if treewalk(env, i).boolVal:
                        return treewalk(env, ast.ifElseIfExpressions[index])
                    index += 1
                if ast.ifElseExpression != nil:
                    return treewalk(env, ast.ifElseExpression)
                return TreewalkValue(kind: tkvBool, boolVal: false)
        of nkLet:
            if not env.vars.hasKey(ast.letName):
                env.vars[ast.letName] = if ast.letValue != nil: treewalk(env, ast.letValue) else: TreewalkValue(kind: tkvBool, boolVal: false)
                return env.vars[ast.letName]
            else:
                raise newException(TreewalkError, "Variable " & ast.identName & " already defined")
        of nkIdentifier:
            if env.vars.hasKey(ast.identName):
                return env.vars[ast.identName]
            else:
                raise newException(TreewalkError, "Variable " & ast.identName & " not defined")
        of nkDef:
            if ast.defName != "":
                if env.defs.hasKey(ast.defName):
                    raise newException(TreewalkError, "Function " & ast.identName & " already defined")
            else:
                env.defs[ast.defName] = (params: ast.defParams, body: ast.defBody)
                return TreewalkValue(kind: tkvDef, defName: ast.defName, def: env.defs[ast.defName])