use aes::Aes128;
use aes::cipher::{
    BlockEncrypt, BlockDecrypt, KeyInit,
    generic_array::GenericArray,
};

fn main() {
    let key = GenericArray::from([0u8; 16]);
    let mut block = GenericArray::from([42u8; 16]);
    let cipher = Aes128::new(&key);
    let block_copy = block.clone();
    cipher.encrypt_block(&mut block);
    cipher.decrypt_block(&mut block);
    assert_eq!(block, block_copy);
    let mut blocks = [block; 100];
    cipher.encrypt_blocks(&mut blocks);
    for block in blocks.iter_mut() {
        cipher.decrypt_block(block);
        assert_eq!(block, &block_copy);
    }
    cipher.decrypt_blocks(&mut blocks);
    for block in blocks.iter_mut() {
        cipher.encrypt_block(block);
        assert_eq!(block, &block_copy);
    }
}
