#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint8_t bytes[16];
} aes_block_t;

void aes_encrypt_block(aes_block_t*, aes_block_t*);
void aes_print_block_mat(aes_block_t*);
void aes_print_block_bytes(aes_block_t*);
