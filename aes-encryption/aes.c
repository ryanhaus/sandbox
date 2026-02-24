#include "aes.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

const static int DEBUG = 0;

// S-box used for encryption
const static uint8_t sbox[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

void aes_encryption_round(aes_block_t*, aes_block_t*, unsigned int);
void aes_sub_bytes(aes_block_t*);
void aes_shift_rows(aes_block_t*);
void aes_mix_columns(aes_block_t*);
void aes_add_round_key(aes_block_t*, aes_block_t*);
void next_round_key(aes_block_t*, unsigned int);

void aes_encrypt_block(aes_block_t* block, aes_block_t* cipher_key)
{
    // Initial round: only add cipher key
    aes_add_round_key(block, cipher_key);

    // Main rounds
    for (int i = 0; i < 10; i++)
    {
        if (DEBUG) printf("Entering round %i\n", i);
        aes_encryption_round(block, cipher_key, i);
    }
}

void aes_encryption_round(aes_block_t* block, aes_block_t* cipher_key, unsigned int round)
{
    // Key expansion
    next_round_key(cipher_key, round);

    // Perform a single round of AES encryption
    aes_sub_bytes(block);

    aes_shift_rows(block);

    if (round != 9) // This step does not occur on the last round
        aes_mix_columns(block);

    aes_add_round_key(block, cipher_key);

    // debug
    if (DEBUG)
    {
        printf("aes_encryption_round:\n");
        printf("\tBlock: ");
        aes_print_block_bytes(block);

        printf("\tRound key: ");
        aes_print_block_bytes(cipher_key);
    }
}

void aes_sub_bytes(aes_block_t* block)
{
    // Go through each byte in the block, substitute it with the appropriate
    // byte in the S-Box
    for (int i = 0; i < 16; i++)
    {
        uint8_t* byte = &block->bytes[i];
        *byte = sbox[*byte];
    }

    // debug
    if (DEBUG)
    {
        printf("aes_sub_bytes:\n");
        printf("\tBlock: ");
        aes_print_block_bytes(block);
    }
}

uint8_t* get_row_col(aes_block_t* block, unsigned int row, unsigned int col)
{
    // Helper function to get the byte from a specific row and column in the block
    assert(row < 4);
    assert(col < 4);

    int i = row + 4*col;
    return &block->bytes[i];
}

void shift_row(aes_block_t* block, unsigned int row)
{
    // Helper function to shift a row by 1 byte
    uint8_t* row_bytes[4];

    for (int i = 0; i < 4; i++)
    {
        row_bytes[i] = get_row_col(block, row, i);
    }

    // Shift the bytes
    int tmp = *row_bytes[3];

    *row_bytes[3] = *row_bytes[0];
    *row_bytes[0] = *row_bytes[1];
    *row_bytes[1] = *row_bytes[2];
    *row_bytes[2] = tmp;
}

void aes_shift_rows(aes_block_t* block)
{
    // Shift rows over
    // First row unchanged, second row by 1, third by 2, fourth by 3
    for (int row = 0; row < 4; row++)
    {
        for (int i = 0; i < row; i++)
        {
            shift_row(block, row);
        }
    }

    // debug
    if (DEBUG)
    {
        printf("aes_shift_rows:\n");
        printf("\tBlock: ");
        aes_print_block_bytes(block);
    }
}

uint8_t gf_mul(uint8_t a, uint8_t b)
{
    uint8_t result = 0;

    while (b)
    {
        if (b & 1)
        {
            result ^= a;
        }

        uint8_t high_bit = a & 0x80;
        a <<= 1;

        if (high_bit)
            a ^= 0x1B;

        b >>= 1;
    }

    return result;
}

void mix_column(aes_block_t* block, int col)
{
    // Helper function to mix a single column
    uint8_t* col_bytes[4];

    for (int i = 0; i < 4; i++)
    {
        col_bytes[i] = get_row_col(block, i, col);
    }

    // Mix the column by multiplying it by the matrix
    const static uint8_t matrix[4][4] = {
        { 2, 3, 1, 1, },
        { 1, 2, 3, 1, },
        { 1, 1, 2, 3, },
        { 3, 1, 1, 2, },
    };

    uint8_t result[4] = { 0 };

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            int val = *col_bytes[j];
            result[i] ^= gf_mul(matrix[i][j], val);
        }
    }

    // Copy bytes from result to column
    for (int i = 0; i < 4; i++)
    {
        *col_bytes[i] = result[i];
    }
}

void aes_mix_columns(aes_block_t* block)
{
    // Multiply each colum by the matrix:
    // [ 2 3 1 1 ]
    // [ 1 2 3 1 ]
    // [ 1 1 2 3 ]
    // [ 3 1 1 2 ]

    for (int col = 0; col < 4; col++)
    {
        mix_column(block, col);
    }

    // debug
    if (DEBUG)
    {
        printf("aes_mix_columns:\n");
        printf("\tBlock: ");
        aes_print_block_bytes(block);
    }
}

void aes_add_round_key(aes_block_t* block, aes_block_t* round_key)
{
    // Add every byte from round_key to block
    for (int i = 0; i < 16; i++)
    {
        block->bytes[i] ^= round_key->bytes[i];
    }

    // debug
    if (DEBUG)
    {
        printf("aes_add_round_key:\n");
        printf("\tBlock: ");
        aes_print_block_bytes(block);
    }
}

void rotate_word(uint8_t* word)
{
    // Helper function to perform the RotWord step used in key expansion
    uint8_t tmp = word[3];

    word[3] = word[0];
    word[0] = word[1];
    word[1] = word[2];
    word[2] = tmp;
}

void next_round_key(aes_block_t* round_key, unsigned int current_round)
{
    // Values used in key expansion
    const static uint8_t rcon[] = {
        0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
    };

    assert(current_round < 10);

    // Create bytes in next key
    aes_block_t key_state[2] = { 0 };
    memcpy(&key_state[0], round_key, sizeof(aes_block_t));

    aes_block_t* next_key = &key_state[1];

    // bytes 0-3
    // RotWord (only 4 bytes from end of round key)
    if (DEBUG > 1)
    {
        printf("Init:       "); aes_print_block_bytes(next_key);
    }

    memcpy(&next_key->bytes[0], &round_key->bytes[12], 4 * sizeof(uint8_t));

    if (DEBUG > 1)
    {
        printf("Copy bytes: "); aes_print_block_bytes(next_key);
    }

    rotate_word(&next_key->bytes[0]);

    if (DEBUG > 1)
    {
        printf("rotWord:    "); aes_print_block_bytes(next_key);
    }

    // SubBytes
    for (int i = 0; i < 4; i++)
    {
        uint8_t* byte = &next_key->bytes[i];
        *byte = sbox[*byte];
    }

    if (DEBUG > 1)
    {
        printf("subBytes:   "); aes_print_block_bytes(next_key);
    }

    // Add rcon val to first byte of last word
    next_key->bytes[0] ^= rcon[current_round];
       
    if (DEBUG > 1)
    {
        printf("rcon:       "); aes_print_block_bytes(next_key);
    }

    // Add other bytes
    for (int i = 0; i < 4; i++)
    {
        next_key->bytes[i] = key_state[0].bytes[i] ^ key_state[1].bytes[i];
    }

    // bytes 4-15
    for (int i = 4; i < 16; i++)
    {
        next_key->bytes[i] = key_state[0].bytes[i] ^ key_state[1].bytes[i - 4];
    }

    if (DEBUG > 1)
    {
        printf("add bytes:  "); aes_print_block_bytes(next_key);
    }

    memcpy(round_key, next_key, sizeof(aes_block_t));


    // debug
    if (DEBUG)
    {
        printf("next_round_key:\n");
        printf("\tRound key: ");
        aes_print_block_bytes(round_key);
    }
}

void aes_print_block_mat(aes_block_t* block)
{
    char str[16][3] = { 0 };
    
    // Print bytes
    for (int i = 0; i < 16; i++)
    {
        int row = i / 4;
        int col = i % 4;
        uint8_t* byte = get_row_col(block, row, col);

        sprintf(str[i], "%02X ", *byte);
    }

    // Add newlines & term char
    str[3][2] = '\n';
    str[7][2] = '\n';
    str[11][2] = '\n';
    str[15][2] = 0;
    
    printf("%s\n", (char*)str);
}

void aes_print_block_bytes(aes_block_t* block)
{
    char str[16][3] = { 0 };

    for (int i = 0; i < 16; i++)
    {
        sprintf(str[i], "%02X ", block->bytes[i]);
    }

    str[15][2] = 0;

    printf("%s\n", (char*)str);
}
