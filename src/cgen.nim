
import
    ast

proc cgen*(node: AstNode): string =
    case node.kind:
        of nkNumber:
            return "";
        of nkString:
            return "";
        of nkBool:
            return "";
        of nkBinary:
            return "";
        of nkUnary:
            return "";
        of nkList:
            return "";
        of nkIf:
            return "";
        of nkLet:
            return "";
        of nkIdentifier:
            return "";
        of nkDef:
            return "";