#include <stdio.h>
#include "aes.h"

int main()
{
    aes_block_t block = {
        {
            0x4C, 0x6F, 0x72, 0x65,
            0x6D, 0x20, 0x69, 0x70,
            0x73, 0x75, 0x6D, 0x20,
            0x64, 0x6F, 0x6C, 0x6F, 
        }
    };

    aes_block_t key = {
        {
            0x2B, 0x7E, 0x15, 0x16,
            0x28, 0xAE, 0xD2, 0xA6,
            0xAB, 0xF7, 0x15, 0x88,
            0x09, 0xCF, 0x4F, 0x3C,
        }
    };

    printf("Initial block:\n");
    aes_print_block_mat(&block);

    printf("Initial cipher key:\n");
    aes_print_block_mat(&key);

    aes_encrypt_block(&block, &key);

    printf("Final block:\n");
    aes_print_block_mat(&block);

    printf("Final cipher key:\n");
    aes_print_block_mat(&key);

    return 0;
}
