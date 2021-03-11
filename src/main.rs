mod core;
mod frontend;

fn main() {
    let source = "x = 5";
    println!("\n\nSource:\n\n{}\n", source);
    let lexer = frontend::lexer::Lexer::new(source);

    for t in lexer {
        println!("{}", t);
    }
}
