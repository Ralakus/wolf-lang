#[derive(Debug, Clone)]
pub struct Module {
    pub declarations: (),
    pub statements: Vec<Statement>,
}

#[derive(Debug, Clone)]
pub enum StatementOrDeclaration {
    Statement(Statement),
    Declaration,
}

#[derive(Debug, Clone)]
pub struct Block {
    pub statements: Vec<Statement>,
    pub end_expression: Option<Expression>,
}

#[derive(Debug, Clone)]
pub enum Statement {
    Expression(Expression),
    Var(String, Option<Expression>),
    Break(Option<Expression>),
    Continue,
}

#[derive(Debug, Clone)]
pub enum BinaryOperation {
    Add,
    Sub,
    Mul,
    Div,

    Equal,
    Greater,
    GreaterEqual,
    Lesser,
    LesserEqual,
}

#[derive(Debug, Clone)]
pub enum UnaryOperation {
    Not,
    Negate,
}

#[derive(Debug, Clone)]
pub enum Expression {

    If(Box<Expression>, Box<Block>, Vec<(Expression, Block)>, Option<Box<Block>>),

    While(Box<Expression>, Box<Block>),

    Loop(Box<Block>),

    Block(Box<Block>),

    Call(Box<Expression>, Vec<Expression>),

    Binary(BinaryOperation, Box<Expression>, Box<Expression>),

    Unary(UnaryOperation, Box<Expression>),

    Number(f64),

    Identifier(String),

    String(String),

    Bool(bool),
}
