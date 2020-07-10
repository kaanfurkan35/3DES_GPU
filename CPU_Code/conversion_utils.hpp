#pragma once

#define BYTE char

//Function declarations for hex,char and binary conversions

//Hex to binary conversion as look-up table, all numbers and letters until F is considered only
void hex_to_bin(BYTE* s_in, BYTE* s_out);

//Char to binary conversion, converts ascii to bits
void char_to_bin(BYTE* s_in, BYTE* s_out, int size);

//Binary to hex conversions with printf
void bin32_to_hex(BYTE input[32]);
void bin48_to_hex(BYTE input[48]);
void bin64_to_hex(BYTE* input, BYTE size);