#[derive(Clone, Debug, Copy, PartialEq, Eq, Default)]
pub struct Position {
    pub line: i32,
    pub col: i32,
}

impl Position {
    pub fn new(line: i32, col: i32) -> Self {
        Self { line, col }
    }
}
