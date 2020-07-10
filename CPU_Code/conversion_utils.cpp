#include <stdio.h>
#define BYTE char
//Function definitions

//Hex to binary conversion as look-up table, all numbers and letters until F is considered only
void hex_to_bin(BYTE* s_in, BYTE* s_out) {
	for (int i = 0; i < 16; i++)
	{
		switch (s_in[i]) {
		case '0':
			s_out[i * 4] = 0;
			s_out[(i * 4) + 1] = 0;
			s_out[(i * 4) + 2] = 0;
			s_out[(i * 4) + 3] = 0;
			break;
		case '1':
			s_out[i * 4] = 0;
			s_out[(i * 4) + 1] = 0;
			s_out[(i * 4) + 2] = 0;
			s_out[(i * 4) + 3] = 1;
			break;
		case '2':
			s_out[i * 4] = 0;
			s_out[(i * 4) + 1] = 0;
			s_out[(i * 4) + 2] = 1;
			s_out[(i * 4) + 3] = 0;
			break;
		case '3':
			s_out[i * 4] = 0;
			s_out[(i * 4) + 1] = 0;
			s_out[(i * 4) + 2] = 1;
			s_out[(i * 4) + 3] = 1;
			break;
		case '4':
			s_out[i * 4] = 0;
			s_out[(i * 4) + 1] = 1;
			s_out[(i * 4) + 2] = 0;
			s_out[(i * 4) + 3] = 0;
			break;
		case '5':
			s_out[i * 4] = 0;
			s_out[(i * 4) + 1] = 1;
			s_out[(i * 4) + 2] = 0;
			s_out[(i * 4) + 3] = 1;
			break;
		case '6':
			s_out[i * 4] = 0;
			s_out[(i * 4) + 1] = 1;
			s_out[(i * 4) + 2] = 1;
			s_out[(i * 4) + 3] = 0;
			break;
		case '7':
			s_out[i * 4] = 0;
			s_out[(i * 4) + 1] = 1;
			s_out[(i * 4) + 2] = 1;
			s_out[(i * 4) + 3] = 1;
			break;
		case '8':
			s_out[i * 4] = 1;
			s_out[(i * 4) + 1] = 0;
			s_out[(i * 4) + 2] = 0;
			s_out[(i * 4) + 3] = 0;
			break;
		case '9':
			s_out[i * 4] = 1;
			s_out[(i * 4) + 1] = 0;
			s_out[(i * 4) + 2] = 0;
			s_out[(i * 4) + 3] = 1;
			break;
		case 'A':
		case 'a':
			s_out[i * 4] = 1;
			s_out[(i * 4) + 1] = 0;
			s_out[(i * 4) + 2] = 1;
			s_out[(i * 4) + 3] = 0;
			break;
		case 'B':
		case 'b':
			s_out[i * 4] = 1;
			s_out[(i * 4) + 1] = 0;
			s_out[(i * 4) + 2] = 1;
			s_out[(i * 4) + 3] = 1;
			break;
		case 'C':
		case 'c':
			s_out[i * 4] = 1;
			s_out[(i * 4) + 1] = 1;
			s_out[(i * 4) + 2] = 0;
			s_out[(i * 4) + 3] = 0;
			break;
		case 'D':
		case 'd':
			s_out[i * 4] = 1;
			s_out[(i * 4) + 1] = 1;
			s_out[(i * 4) + 2] = 0;
			s_out[(i * 4) + 3] = 1;
			break;
		case 'E':
		case 'e':
			s_out[i * 4] = 1;
			s_out[(i * 4) + 1] = 1;
			s_out[(i * 4) + 2] = 1;
			s_out[(i * 4) + 3] = 0;
			break;
		case 'F':
		case 'f':
			s_out[i * 4] = 1;
			s_out[(i * 4) + 1] = 1;
			s_out[(i * 4) + 2] = 1;
			s_out[(i * 4) + 3] = 1;
			break;
		default:
			printf("\nInvalid hexadecimal digit %c",
				s_in[i]);
		}
	}
}

//Char to binary conversion, converts ascii to bits
void char_to_bin(BYTE* s_in, BYTE* s_out, int size) {
	for (int i = 0; i < 8 * size; i++)
	{
		switch (s_in[i]) {
		case '0':
			s_out[i * 8] = 0;
			s_out[(i * 8) + 1] = 0;
			s_out[(i * 8) + 2] = 1;
			s_out[(i * 8) + 3] = 1;
			s_out[(i * 8) + 4] = 0;
			s_out[(i * 8) + 5] = 0;
			s_out[(i * 8) + 6] = 0;
			s_out[(i * 8) + 7] = 0;
			break;
		case '1':
			s_out[i * 8] = 0;
			s_out[(i * 8) + 1] = 0;
			s_out[(i * 8) + 2] = 1;
			s_out[(i * 8) + 3] = 1;
			s_out[(i * 8) + 4] = 0;
			s_out[(i * 8) + 5] = 0;
			s_out[(i * 8) + 6] = 0;
			s_out[(i * 8) + 7] = 1;
			break;
		case '2':
			s_out[i * 8] = 0;
			s_out[(i * 8) + 1] = 0;
			s_out[(i * 8) + 2] = 1;
			s_out[(i * 8) + 3] = 1;
			s_out[(i * 8) + 4] = 0;
			s_out[(i * 8) + 5] = 0;
			s_out[(i * 8) + 6] = 1;
			s_out[(i * 8) + 7] = 0;
			break;
		case '3':
			s_out[i * 8] = 0;
			s_out[(i * 8) + 1] = 0;
			s_out[(i * 8) + 2] = 1;
			s_out[(i * 8) + 3] = 1;
			s_out[(i * 8) + 4] = 0;
			s_out[(i * 8) + 5] = 0;
			s_out[(i * 8) + 6] = 1;
			s_out[(i * 8) + 7] = 1;
			break;
		case '4':
			s_out[i * 8] = 0;
			s_out[(i * 8) + 1] = 0;
			s_out[(i * 8) + 2] = 1;
			s_out[(i * 8) + 3] = 1;
			s_out[(i * 8) + 4] = 0;
			s_out[(i * 8) + 5] = 1;
			s_out[(i * 8) + 6] = 0;
			s_out[(i * 8) + 7] = 0;
			break;
		case '5':
			s_out[i * 8] = 0;
			s_out[(i * 8) + 1] = 0;
			s_out[(i * 8) + 2] = 1;
			s_out[(i * 8) + 3] = 1;
			s_out[(i * 8) + 4] = 0;
			s_out[(i * 8) + 5] = 1;
			s_out[(i * 8) + 6] = 0;
			s_out[(i * 8) + 7] = 1;
			break;
		case '6':
			s_out[i * 8] = 0;
			s_out[(i * 8) + 1] = 0;
			s_out[(i * 8) + 2] = 1;
			s_out[(i * 8) + 3] = 1;
			s_out[(i * 8) + 4] = 0;
			s_out[(i * 8) + 5] = 1;
			s_out[(i * 8) + 6] = 1;
			s_out[(i * 8) + 7] = 0;
			break;
		case '7':
			s_out[i * 8] = 0;
			s_out[(i * 8) + 1] = 0;
			s_out[(i * 8) + 2] = 1;
			s_out[(i * 8) + 3] = 1;
			s_out[(i * 8) + 4] = 0;
			s_out[(i * 8) + 5] = 1;
			s_out[(i * 8) + 6] = 1;
			s_out[(i * 8) + 7] = 1;
			break;
		case '8':
			s_out[i * 8] = 0;
			s_out[(i * 8) + 1] = 0;
			s_out[(i * 8) + 2] = 1;
			s_out[(i * 8) + 3] = 1;
			s_out[(i * 8) + 4] = 1;
			s_out[(i * 8) + 5] = 0;
			s_out[(i * 8) + 6] = 0;
			s_out[(i * 8) + 7] = 0;
			break;
		case '9':
			s_out[i * 8] = 0;
			s_out[(i * 8) + 1] = 0;
			s_out[(i * 8) + 2] = 1;
			s_out[(i * 8) + 3] = 1;
			s_out[(i * 8) + 4] = 1;
			s_out[(i * 8) + 5] = 0;
			s_out[(i * 8) + 6] = 0;
			s_out[(i * 8) + 7] = 1;
			break;
		case 'A':
		case 'a':
			s_out[i * 8] = 0;
			s_out[(i * 8) + 1] = 1;
			s_out[(i * 8) + 2] = 1;
			s_out[(i * 8) + 3] = 0;
			s_out[(i * 8) + 4] = 0;
			s_out[(i * 8) + 5] = 0;
			s_out[(i * 8) + 6] = 0;
			s_out[(i * 8) + 7] = 1;
			break;
		case 'B':
		case 'b':
			s_out[i * 8] = 0;
			s_out[(i * 8) + 1] = 1;
			s_out[(i * 8) + 2] = 1;
			s_out[(i * 8) + 3] = 0;
			s_out[(i * 8) + 4] = 0;
			s_out[(i * 8) + 5] = 0;
			s_out[(i * 8) + 6] = 1;
			s_out[(i * 8) + 7] = 0;
			break;
		case 'C':
		case 'c':
			s_out[i * 8] = 0;
			s_out[(i * 8) + 1] = 1;
			s_out[(i * 8) + 2] = 1;
			s_out[(i * 8) + 3] = 0;
			s_out[(i * 8) + 4] = 0;
			s_out[(i * 8) + 5] = 0;
			s_out[(i * 8) + 6] = 1;
			s_out[(i * 8) + 7] = 1;
			break;
		case 'D':
		case 'd':
			s_out[i * 8] = 0;
			s_out[(i * 8) + 1] = 1;
			s_out[(i * 8) + 2] = 1;
			s_out[(i * 8) + 3] = 0;
			s_out[(i * 8) + 4] = 0;
			s_out[(i * 8) + 5] = 1;
			s_out[(i * 8) + 6] = 0;
			s_out[(i * 8) + 7] = 0;
			break;
		case 'E':
		case 'e':
			s_out[i * 8] = 0;
			s_out[(i * 8) + 1] = 1;
			s_out[(i * 8) + 2] = 1;
			s_out[(i * 8) + 3] = 0;
			s_out[(i * 8) + 4] = 0;
			s_out[(i * 8) + 5] = 1;
			s_out[(i * 8) + 6] = 0;
			s_out[(i * 8) + 7] = 1;
			break;
		case 'F':
		case 'f':
			s_out[i * 8] = 0;
			s_out[(i * 8) + 1] = 1;
			s_out[(i * 8) + 2] = 1;
			s_out[(i * 8) + 3] = 0;
			s_out[(i * 8) + 4] = 0;
			s_out[(i * 8) + 5] = 1;
			s_out[(i * 8) + 6] = 1;
			s_out[(i * 8) + 7] = 0;
			break;
		case 'G':
		case 'g':
			s_out[i * 8] = 0;
			s_out[(i * 8) + 1] = 1;
			s_out[(i * 8) + 2] = 1;
			s_out[(i * 8) + 3] = 0;
			s_out[(i * 8) + 4] = 0;
			s_out[(i * 8) + 5] = 1;
			s_out[(i * 8) + 6] = 1;
			s_out[(i * 8) + 7] = 1;
			break;
		case 'H':
		case 'h':
			s_out[i * 8] = 0;
			s_out[(i * 8) + 1] = 1;
			s_out[(i * 8) + 2] = 1;
			s_out[(i * 8) + 3] = 0;
			s_out[(i * 8) + 4] = 1;
			s_out[(i * 8) + 5] = 0;
			s_out[(i * 8) + 6] = 0;
			s_out[(i * 8) + 7] = 0;
			break;
		case 'I':
		case 'i':
			s_out[i * 8] = 0;
			s_out[(i * 8) + 1] = 1;
			s_out[(i * 8) + 2] = 1;
			s_out[(i * 8) + 3] = 0;
			s_out[(i * 8) + 4] = 1;
			s_out[(i * 8) + 5] = 0;
			s_out[(i * 8) + 6] = 0;
			s_out[(i * 8) + 7] = 1;
			break;
		case 'J':
		case 'j':
			s_out[i * 8] = 0;
			s_out[(i * 8) + 1] = 1;
			s_out[(i * 8) + 2] = 1;
			s_out[(i * 8) + 3] = 0;
			s_out[(i * 8) + 4] = 1;
			s_out[(i * 8) + 5] = 0;
			s_out[(i * 8) + 6] = 1;
			s_out[(i * 8) + 7] = 0;
			break;
		case 'K':
		case 'k':
			s_out[i * 8] = 0;
			s_out[(i * 8) + 1] = 1;
			s_out[(i * 8) + 2] = 1;
			s_out[(i * 8) + 3] = 0;
			s_out[(i * 8) + 4] = 1;
			s_out[(i * 8) + 5] = 0;
			s_out[(i * 8) + 6] = 1;
			s_out[(i * 8) + 7] = 1;
			break;
		case 'L':
		case 'l':
			s_out[i * 8] = 0;
			s_out[(i * 8) + 1] = 1;
			s_out[(i * 8) + 2] = 1;
			s_out[(i * 8) + 3] = 0;
			s_out[(i * 8) + 4] = 1;
			s_out[(i * 8) + 5] = 1;
			s_out[(i * 8) + 6] = 0;
			s_out[(i * 8) + 7] = 0;
			break;
		case 'M':
		case 'm':
			s_out[i * 8] = 0;
			s_out[(i * 8) + 1] = 1;
			s_out[(i * 8) + 2] = 1;
			s_out[(i * 8) + 3] = 0;
			s_out[(i * 8) + 4] = 1;
			s_out[(i * 8) + 5] = 1;
			s_out[(i * 8) + 6] = 0;
			s_out[(i * 8) + 7] = 1;
			break;
		case 'N':
		case 'n':
			s_out[i * 8] = 0;
			s_out[(i * 8) + 1] = 1;
			s_out[(i * 8) + 2] = 1;
			s_out[(i * 8) + 3] = 0;
			s_out[(i * 8) + 4] = 1;
			s_out[(i * 8) + 5] = 1;
			s_out[(i * 8) + 6] = 1;
			s_out[(i * 8) + 7] = 0;
			break;
		case 'O':
		case 'o':
			s_out[i * 8] = 0;
			s_out[(i * 8) + 1] = 1;
			s_out[(i * 8) + 2] = 1;
			s_out[(i * 8) + 3] = 0;
			s_out[(i * 8) + 4] = 1;
			s_out[(i * 8) + 5] = 1;
			s_out[(i * 8) + 6] = 1;
			s_out[(i * 8) + 7] = 1;
			break;
		case 'P':
		case 'p':
			s_out[i * 8] = 0;
			s_out[(i * 8) + 1] = 1;
			s_out[(i * 8) + 2] = 1;
			s_out[(i * 8) + 3] = 1;
			s_out[(i * 8) + 4] = 0;
			s_out[(i * 8) + 5] = 0;
			s_out[(i * 8) + 6] = 0;
			s_out[(i * 8) + 7] = 0;
			break;
		case 'Q':
		case 'q':
			s_out[i * 8] = 0;
			s_out[(i * 8) + 1] = 1;
			s_out[(i * 8) + 2] = 1;
			s_out[(i * 8) + 3] = 1;
			s_out[(i * 8) + 4] = 0;
			s_out[(i * 8) + 5] = 0;
			s_out[(i * 8) + 6] = 0;
			s_out[(i * 8) + 7] = 1;
			break;
		case 'R':
		case 'r':
			s_out[i * 8] = 0;
			s_out[(i * 8) + 1] = 1;
			s_out[(i * 8) + 2] = 1;
			s_out[(i * 8) + 3] = 1;
			s_out[(i * 8) + 4] = 0;
			s_out[(i * 8) + 5] = 0;
			s_out[(i * 8) + 6] = 1;
			s_out[(i * 8) + 7] = 0;
			break;
		case 'S':
		case 's':
			s_out[i * 8] = 0;
			s_out[(i * 8) + 1] = 1;
			s_out[(i * 8) + 2] = 1;
			s_out[(i * 8) + 3] = 1;
			s_out[(i * 8) + 4] = 0;
			s_out[(i * 8) + 5] = 0;
			s_out[(i * 8) + 6] = 1;
			s_out[(i * 8) + 7] = 1;
			break;
		case 'T':
		case 't':
			s_out[i * 8] = 0;
			s_out[(i * 8) + 1] = 1;
			s_out[(i * 8) + 2] = 1;
			s_out[(i * 8) + 3] = 1;
			s_out[(i * 8) + 4] = 0;
			s_out[(i * 8) + 5] = 1;
			s_out[(i * 8) + 6] = 0;
			s_out[(i * 8) + 7] = 0;
			break;
		case 'U':
		case 'u':
			s_out[i * 8] = 0;
			s_out[(i * 8) + 1] = 1;
			s_out[(i * 8) + 2] = 1;
			s_out[(i * 8) + 3] = 1;
			s_out[(i * 8) + 4] = 0;
			s_out[(i * 8) + 5] = 1;
			s_out[(i * 8) + 6] = 0;
			s_out[(i * 8) + 7] = 1;
			break;
		case 'V':
		case 'v':
			s_out[i * 8] = 0;
			s_out[(i * 8) + 1] = 1;
			s_out[(i * 8) + 2] = 1;
			s_out[(i * 8) + 3] = 1;
			s_out[(i * 8) + 4] = 0;
			s_out[(i * 8) + 5] = 1;
			s_out[(i * 8) + 6] = 1;
			s_out[(i * 8) + 7] = 0;
			break;
		case 'W':
		case 'w':
			s_out[i * 8] = 0;
			s_out[(i * 8) + 1] = 1;
			s_out[(i * 8) + 2] = 1;
			s_out[(i * 8) + 3] = 1;
			s_out[(i * 8) + 4] = 0;
			s_out[(i * 8) + 5] = 1;
			s_out[(i * 8) + 6] = 1;
			s_out[(i * 8) + 7] = 1;
			break;
		case 'X':
		case 'x':
			s_out[i * 8] = 0;
			s_out[(i * 8) + 1] = 1;
			s_out[(i * 8) + 2] = 1;
			s_out[(i * 8) + 3] = 1;
			s_out[(i * 8) + 4] = 1;
			s_out[(i * 8) + 5] = 0;
			s_out[(i * 8) + 6] = 0;
			s_out[(i * 8) + 7] = 0;
			break;
		case 'Y':
		case 'y':
			s_out[i * 8] = 0;
			s_out[(i * 8) + 1] = 1;
			s_out[(i * 8) + 2] = 1;
			s_out[(i * 8) + 3] = 1;
			s_out[(i * 8) + 4] = 1;
			s_out[(i * 8) + 5] = 0;
			s_out[(i * 8) + 6] = 0;
			s_out[(i * 8) + 7] = 1;
			break;
		case 'Z':
		case 'z':
			s_out[i * 8] = 0;
			s_out[(i * 8) + 1] = 1;
			s_out[(i * 8) + 2] = 1;
			s_out[(i * 8) + 3] = 1;
			s_out[(i * 8) + 4] = 1;
			s_out[(i * 8) + 5] = 0;
			s_out[(i * 8) + 6] = 1;
			s_out[(i * 8) + 7] = 0;
			break;
		default:
			printf("\nInvalid hexadecimal digit %c",
				s_in[i]);
		}
	}
}

//Binary to hex conversions with printf
void bin32_to_hex(BYTE input[32]) {
	BYTE buf[8];
	for (int j = 0; j < 8; j++) {
		buf[j] = 8 * input[j * 4] + 4 * input[j * 4 + 1] + 2 * input[j * 4 + 2] + input[j * 4 + 3];
		printf("%01X", buf[j]);
	}
}

void bin48_to_hex(BYTE input[48]) {
	BYTE buf[12];
	for (int j = 0; j < 12; j++) {
		buf[j] = 8 * input[j * 4] + 4 * input[j * 4 + 1] + 2 * input[j * 4 + 2] + input[j * 4 + 3];
		printf("%01X", buf[j]);
	}
}

void bin64_to_hex(BYTE* input, BYTE size) {
	BYTE buf;
	for (int j = 0; j < 16 * size; j++) {
		buf = 8 * input[j * 4] + 4 * input[j * 4 + 1] + 2 * input[j * 4 + 2] + input[j * 4 + 3];	
		printf("%01X", buf);
	}
}

