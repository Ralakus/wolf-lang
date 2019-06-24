
type
    TokenType* = enum
        # Single char tokens
        lparen
        rparen
        lcurly
        rcurly
        lbracket
        rbracket

        comma
        dot
        semicolon

        plus
        minus
        star
        slash

        # One or two char tokens
        bang
        bangEqual
        equal
        equalEqual
        greater
        greaterEqual
        less
        lessEqual

        colon
        colonColon

        # Literals
        identifier
        string
        interpolation
        number

        # Keywords
        kwAnd
        kwOr

        kwSelf
        kwStruct
        kwReturn

        kwIf
        kwElse
        kwWhile
        kwFor
        kwBreak

        kwTrue
        kwFalse
        kwNil

        err
        eof

const tokenStrMap: array[eof.int + 1, string] = [

    # Single char tokens
    "(",
    ")",
    "{",
    "}",
    "[",
    "]",

    ",",
    ".",
    ";",

    "+",
    "-",
    "*",
    "/",

    # One or two char tokens
    "!",
    "!=",
    "=",
    "==",
    ">",
    ">=",
    "<",
    "<=",

    ":",
    "::",

    # Literals
    "identifier",
    "string",
    "interpolation",
    "number",

    # Keywords
    "and",
    "or",

    "self",
    "struct",
    "return",

    "if",
    "else",
    "while",
    "for",
    "break",

    "true",
    "false",
    "nil",

    # Special
    "<err>",
    "eof"
]

proc `$`*(token: TokenType): string =
    tokenStrMap[token.int]