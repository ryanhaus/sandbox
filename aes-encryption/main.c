#include <stdio.h>
#include <string.h>
#include "aes.h"

int main()
{
    aes_block_t blocks[3] = { 0 };
    int n_blocks = sizeof(blocks) / sizeof(aes_block_t);
    char* str = (char*)&blocks[0];

    strcpy(str, "The quick brown fox jumps over the lazy dog.");

    aes_block_t key = {
        {
            0x2B, 0x7E, 0x15, 0x16,
            0x28, 0xAE, 0xD2, 0xA6,
            0xAB, 0xF7, 0x15, 0x88,
            0x09, 0xCF, 0x4F, 0x3C,
        }
    };

    printf("Input:\n");
    aes_print_blocks_bytes(blocks, n_blocks);

    printf("\nKey:\n");
    aes_print_block_bytes(&key);

    aes_encrypt_blocks(blocks, n_blocks, &key);

    printf("\nOutput:\n");
    aes_print_blocks_bytes(blocks, n_blocks);

    return 0;
}
