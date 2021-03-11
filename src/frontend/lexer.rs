use super::{
    token::{Token, TokenData, TokenType},
    Position,
};

pub struct Lexer<'a> {
    source: &'a str,
    chars: std::str::CharIndices<'a>,
    previous_char: Option<(usize, char)>,
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

    pub fn get_tok(&mut self) -> Token<'a> {
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

        let (i, c) = match self.previous_char {
            Some(ic) => ic,
            _ => {
                return Token {
                    kind: TokenType::Eof,
                    data: TokenData::None,
                    pos: self.pos,
                }
            }
        };

        match c {
            c if c.is_alphabetic() => self.identifier(i),
            c if c.is_digit(10) => self.number(i),
            '"' => self.string(i),
            c => self.symbol(c),
        }
    }

    #[inline]
    fn identifier(&mut self, start: usize) -> Token<'a> {
        let pos = self.pos;
        let end = loop {
            match self.advance() {
                Some((i, c)) if !c.is_alphabetic() => break i,
                Some(_) => continue,
                _ => break self.source.len(),
            }
        };
        let identifier = &self.source[start..end];
        Self::match_identifier(identifier, pos)
    }

    #[inline]
    fn match_identifier(identifier: &'a str, pos: Position) -> Token<'a> {
        match identifier {
            "class" => Token {
                kind: TokenType::KwClass,
                data: TokenData::None,
                pos,
            },
            _ => Token {
                kind: TokenType::Identifier,
                data: TokenData::Str(identifier),
                pos,
            },
        }
    }

    #[inline]
    fn number(&mut self, start: usize) -> Token<'a> {
        let mut is_float = false;
        let start_pos = self.pos;
        let end = loop {
            match self.advance() {
                Some((_, '.')) => {
                    is_float = true;
                    continue;
                }
                Some((i, c)) if !c.is_digit(10) => break i,
                Some(_) => continue,
                _ => break self.source.len(),
            }
        };
        let number_str = &self.source[start..end];
        let number = if is_float {
            let float_value = number_str.parse::<f64>();
            match float_value {
                Ok(f) => TokenData::Float(f),
                Err(e) => {
                    eprintln!("{}", e);
                    TokenData::Str(number_str)
                }
            }
        } else {
            let int_value = number_str.parse::<isize>();
            match int_value {
                Ok(n) => TokenData::Integer(n),
                Err(e) => {
                    eprintln!("{}", e);
                    TokenData::Str(number_str)
                }
            }
        };
        Token {
            kind: TokenType::Number,
            data: number,
            pos: start_pos,
        }
    }

    #[inline]
    fn string(&mut self, start: usize) -> Token<'a> {
        let start_pos = self.pos;
        let end = loop {
            match self.advance() {
                Some((i, '"')) => {
                    self.advance();
                    break i;
                }
                Some(_) => continue,
                _ => {
                    return Token {
                        kind: TokenType::Err,
                        data: TokenData::Str("Unterminated string"),
                        pos: start_pos,
                    }
                }
            }
        };
        let string = &self.source[start + 1..end];
        Token {
            kind: TokenType::String,
            data: TokenData::Str(string),
            pos: start_pos,
        }
    }

    #[inline]
    fn symbol(&mut self, previous: char) -> Token<'a> {
        let pos = self.pos;
        let current = match self.advance() {
            Some((_, c)) => c,
            None => '\0',
        };
        match (previous, current) {
            ('{', _) => Token {
                kind: TokenType::LCurly,
                data: TokenData::None,
                pos,
            },
            ('}', _) => Token {
                kind: TokenType::RCurly,
                data: TokenData::None,
                pos,
            },
            ('[', _) => Token {
                kind: TokenType::LBracket,
                data: TokenData::None,
                pos,
            },
            (']', _) => Token {
                kind: TokenType::RBracket,
                data: TokenData::None,
                pos,
            },
            ('(', _) => Token {
                kind: TokenType::LParenthesis,
                data: TokenData::None,
                pos,
            },
            (')', _) => Token {
                kind: TokenType::RParenthesis,
                data: TokenData::None,
                pos,
            },
            ('.', _) => Token {
                kind: TokenType::Dot,
                data: TokenData::None,
                pos,
            },
            (',', _) => Token {
                kind: TokenType::Comma,
                data: TokenData::None,
                pos,
            },
            (':', _) => Token {
                kind: TokenType::Colon,
                data: TokenData::None,
                pos,
            },
            (';', _) => Token {
                kind: TokenType::SemiColon,
                data: TokenData::None,
                pos,
            },
            ('+', _) => Token {
                kind: TokenType::Plus,
                data: TokenData::None,
                pos,
            },
            ('-', '>') => {
                self.advance();
                Token {
                    kind: TokenType::Arrow,
                    data: TokenData::None,
                    pos,
                }
            }
            ('-', _) => Token {
                kind: TokenType::Minus,
                data: TokenData::None,
                pos,
            },
            ('*', _) => Token {
                kind: TokenType::Star,
                data: TokenData::None,
                pos,
            },
            ('/', _) => Token {
                kind: TokenType::Slash,
                data: TokenData::None,
                pos,
            },
            ('!', '=') => {
                self.advance();
                Token {
                    kind: TokenType::BangEqual,
                    data: TokenData::None,
                    pos,
                }
            }
            ('!', _) => Token {
                kind: TokenType::Bang,
                data: TokenData::None,
                pos,
            },
            ('=', '=') => {
                self.advance();
                Token {
                    kind: TokenType::EqualEqual,
                    data: TokenData::None,
                    pos,
                }
            }
            ('=', _) => Token {
                kind: TokenType::Equal,
                data: TokenData::None,
                pos,
            },
            ('<', '=') => {
                self.advance();
                Token {
                    kind: TokenType::LessEqual,
                    data: TokenData::None,
                    pos,
                }
            }
            ('<', _) => Token {
                kind: TokenType::Less,
                data: TokenData::None,
                pos,
            },
            ('>', '=') => {
                self.advance();
                Token {
                    kind: TokenType::GreaterEqual,
                    data: TokenData::None,
                    pos,
                }
            }
            ('>', _) => Token {
                kind: TokenType::Greater,
                data: TokenData::None,
                pos,
            },
            _ => Token {
                kind: TokenType::Err,
                data: TokenData::Str("Invalid character"),
                pos,
            },
        }
    }
}

impl<'a> Iterator for Lexer<'a> {
    type Item = Token<'a>;

    fn next(&mut self) -> Option<Self::Item> {
        match self.get_tok() {
            t if t.kind == TokenType::Eof => None,
            t => Some(t),
        }
    }
}
