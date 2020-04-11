#[repr(u8)]
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum Token<'a> {
    LCurly,
    RCurly,
    LBracket,
    RBracket,
    LParenthesis,
    RParenthesis,

    Dot,
    Comma,
    Colon,
    SemiColon,
    Arrow,

    Plus,
    Minus,
    Star,
    Slash,

    Bang,
    BangEqual,
    Equal,
    EqualEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,

    Identifier(&'a str),
    Number(&'a str),
    Str(&'a str),

    KwClass,

    Error(&'a str),
    Eof,
}
