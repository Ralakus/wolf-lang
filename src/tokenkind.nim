
type
    TokenKind* = enum
        # Single char tokens
        tkLparen
        tkRparen
        tkLcurly
        tkRcurly
        tkLbracket
        tkRbracket

        tkComma
        tkDot
        tkSemicolon

        tkPlus
        tkMinus
        tkStar
        tkSlash

        # One or two char tokens
        tkBang
        tkBangEqual
        tkEqual
        tkEqualEqual
        tkGreater
        tkGreaterEqual
        tkLess
        tkLessEqual

        tkColon
        tkColonColon

        # Literals
        tkIdentifier
        tkString
        tkInterpolation
        tkNumber

        # Keywords
        tkKwAnd
        tkKwOr

        tkKwSelf
        tkKwStruct
        tkKwReturn
        tkKwImport
        tkKwLet
        tkKwDef

        tkKwIf
        tkKwElse
        tkKwElseif
        tkKwWhile
        tkKwFor
        tkKwBreak
        tkKwCall
        tkKwFrom

        tkKwTrue
        tkKwFalse
        tkKwNil

        tkErr
        tkEof

const tokenStrMap: array[tkEof.int + 1, string] = [

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
    "import",
    "let",
    "def",

    "if",
    "else",
    "elseif",
    "while",
    "for",
    "break",
    "call",
    "from",

    "true",
    "false",
    "nil",

    # Special
    "<err>",
    "eof"
]

proc `$`*(token: TokenKind): string =
    tokenStrMap[token.int]