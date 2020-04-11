use super::Position;

#[derive(Clone, Copy, Debug)]
pub struct Token<'a> {
    pub kind: TokenType,
    pub data: TokenData<'a>,
    pub pos: Position,
}

#[derive(Clone, Copy, Debug)]
pub enum TokenData<'a> {
    None,
    Str(&'a str),
    Float(f64),
    Integer(isize),
}

#[repr(u8)]
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum TokenType {
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

    Identifier,
    Number,
    String,

    KwClass,

    Err,
    Eof,
}

impl<'a> std::fmt::Display for Token<'a> {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(
            f,
            "{:<12} {:<10} {:<12}",
            &format!("{:?}", self.kind),
            &format!("[{}:{}]", self.pos.line, self.pos.col),
            self.data
        )
    }
}

impl<'a> std::fmt::Display for TokenData<'a> {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Self::None => Ok(()),
            Self::Str(s) => write!(f, "<{}>", s),
            Self::Integer(n) => write!(f, "<{}>", n),
            Self::Float(n) => write!(f, "<{}>", n),
        }
    }
}
