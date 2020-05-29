use super::{token::Token, Position};

pub struct Lexer<'a> {
    source: &'a str,
    chars: std::str::CharIndices<'a>,
    previous_char: Option<(usize, char)>,
    pos: Position,
}

#[derive(Debug)]
pub struct LexerError<'a> {
    msg: &'a str,
    pos: Position,
}

impl<'a> Lexer<'a> {
    pub fn new(source: &'a str) -> Self {
        let mut chars = source.char_indices();
        let previous_char = chars.next();
        Self {
            source,
            chars,
            previous_char,
            pos: Position::new(1, 1),
        }
    }

    fn advance(&mut self) -> Option<(usize, char)> {
        self.previous_char = self.chars.next();
        self.pos.col += 1;
        self.previous_char
    }

    pub fn get_tok(&mut self) -> (Position, Token<'a>, Position) {
        // skip whitespace
        loop {
            match self.previous_char {
                Some((_, c)) if c.is_whitespace() => {
                    if c == '\n' {
                        self.pos.line += 1;
                        self.pos.col = 0;
                    }
                    self.advance();
                }
                _ => break,
            }
        }

        let pos = self.pos;

        let (i, c) = match self.previous_char {
            Some(ic) => ic,
            _ => return (pos, Token::Eof, self.pos),
        };

        (
            pos,
            match c {
                c if c.is_alphabetic() => self.identifier(i),
                c if c.is_digit(10) => self.number(i),
                '"' => self.string(i),
                c => self.symbol(c),
            },
            self.pos,
        )
    }

    #[inline]
    fn identifier(&mut self, start: usize) -> Token<'a> {
        let end = loop {
            match self.advance() {
                Some((i, c)) if !c.is_alphabetic() && c != '_' => break i,
                Some(_) => continue,
                _ => break self.source.len(),
            }
        };
        let identifier = &self.source[start..end];
        Self::match_identifier(identifier)
    }

    #[inline]
    fn match_identifier(identifier: &'a str) -> Token<'a> {
        match identifier {
            "class" => Token::KwClass,
            "var" => Token::KwVar,
            "true" => Token::KwTrue,
            "false" => Token::KwFalse,
            "if" => Token::KwIf,
            "else" => Token::KwElse,
            "while" => Token::KwWhile,
            "loop" => Token::KwLoop,
            "break" => Token::KwBreak,
            "continue" => Token::KwContinue,
            _ => Token::Identifier(identifier),
        }
    }

    #[inline]
    fn number(&mut self, start: usize) -> Token<'a> {
        let end = loop {
            match self.advance() {
                Some((_, '.')) => {
                    continue;
                }
                Some((i, c)) if !c.is_digit(10) => break i,
                Some(_) => continue,
                _ => break self.source.len(),
            }
        };
        let number = &self.source[start..end];
        Token::Number(number)
    }

    #[inline]
    fn string(&mut self, start: usize) -> Token<'a> {
        let end = loop {
            match self.advance() {
                Some((i, '"')) => {
                    self.advance();
                    break i;
                }
                Some(_) => continue,
                _ => return Token::Error("Unterminated string"),
            }
        };
        let string = &self.source[start + 1..end];
        Token::Str(string)
    }

    #[inline]
    fn symbol(&mut self, previous: char) -> Token<'a> {
        let current = match self.advance() {
            Some((_, c)) => c,
            None => '\0',
        };
        match (previous, current) {
            ('{', _) => Token::LCurly,
            ('}', _) => Token::RCurly,
            ('[', _) => Token::LBracket,
            (']', _) => Token::RBracket,
            ('(', _) => Token::LParenthesis,
            (')', _) => Token::RParenthesis,
            ('.', _) => Token::Dot,
            (',', _) => Token::Comma,
            (':', _) => Token::Colon,
            (';', _) => Token::SemiColon,
            ('+', _) => Token::Plus,
            ('-', '>') => {
                self.advance();
                Token::Arrow
            }
            ('-', _) => Token::Minus,
            ('*', _) => Token::Star,
            ('/', _) => Token::Slash,
            ('!', '=') => {
                self.advance();
                Token::BangEqual
            }
            ('!', _) => Token::Bang,
            ('=', '=') => {
                self.advance();
                Token::EqualEqual
            }
            ('=', _) => Token::Equal,
            ('<', '=') => {
                self.advance();
                Token::LessEqual
            }
            ('<', _) => Token::Less,
            ('>', '=') => {
                self.advance();
                Token::GreaterEqual
            }
            ('>', _) => Token::Greater,
            _ => Token::Error("Invalid character"),
        }
    }
}

impl<'a> Iterator for Lexer<'a> {
    type Item = Result<(Position, Token<'a>, Position), LexerError<'a>>;

    fn next(&mut self) -> Option<Self::Item> {
        match self.get_tok() {
            (pos, Token::Error(msg), _) => Some(Err(LexerError { msg, pos })),
            (_, t, _) if t == Token::Eof => None,
            ptp => Some(Ok(ptp)),
        }
    }
}

impl<'a> std::fmt::Display for LexerError<'a> {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{} at [{}:{}]", self.msg, self.pos.line, self.pos.col)
    }
}
