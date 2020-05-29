mod ast;
mod core;
mod frontend;
mod wasm;

use std::io::{BufRead, Write};

fn main() {
    loop {
        print!("> ");
        let mut source = String::new();
        std::io::stdout().flush().unwrap();
        std::io::stdin()
            .lock()
            .read_line(&mut source)
            .expect("Error reading input");

        if source.starts_with("exit") {
            break;
        }

        let lexer = frontend::lexer::Lexer::new(&source);
        let result = match frontend::parser::ExpressionParser::new().parse(&source, lexer) {
            Ok(result) => result,
            Err(e) => {
                match e {
                    lalrpop_util::ParseError::User { error } => println!("{}", error),
                    lalrpop_util::ParseError::UnrecognizedToken { token, expected } => {
                        println!("Expected {:?}; found {:?}", expected, token.1)
                    }
                    _ => println!("{:?}", e),
                };
                continue;
            }
        };

        println!("=> {:#?}", result);
    }
}
