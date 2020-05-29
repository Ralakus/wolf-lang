use crate::ast::{BinaryOperation, Expression, UnaryOperation};

const HEADER: &[u8] = &[b'\0', b'a', b's', b'm'];
const VERSION: &[u8] = &[0x01, 0x00, 0x00, 0x00];

pub fn generate_wasm(ast: &Expression) -> Vec<u8> {
    let mut module = Vec::from(HEADER);
    module.extend_from_slice(VERSION);

    module
}
