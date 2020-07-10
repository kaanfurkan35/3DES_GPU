/*****************************************************************************
KAAN FURKAN ALTINOK 2030021 - GÖKBERK AFŞİN PEKER 2094357
3DES ON CPU IMPLEMENTATION
Course : Applied Parallel Programming on GPU
Lecturer: Dr.Alptekin Temizel

This program takes a foramtted txt file and encyrpts in 3DES mode with OPENMP. 
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>
#include <omp.h>
#include "conversion_utils.hpp" //Custom include

#define BYTE char
#define ITER_COUNT 1

//DO NOT USE THIS WHEN USING OPENMP & Release mode, uncomment this line to disable prints only in NON-OPENMP & Debug mode
//#define PRINT

double PCFreq = 0.0;
__int64 CounterStart = 0;

//Windows counter start
void StartCounter()
{
	LARGE_INTEGER li;
	if (!QueryPerformanceFrequency(&li))
		printf("QueryPerformanceFrequency failed!\n");

	PCFreq = (double) li.QuadPart / 1000000.0;

	QueryPerformanceCounter(&li);
	CounterStart = li.QuadPart;
}

//Windows counter get
double GetCounter()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return (double) (li.QuadPart - CounterStart) / PCFreq;
}

//Permutation functions
void permute64(BYTE k[64], BYTE* arr) {
	BYTE temp[64];
	for (int i = 0; i < 64; i++) {
		temp[i] = k[arr[i] - 1];
	}
	for (int i = 0; i < 64; i++) {
		k[i] = temp[i];
	}
}

void permute32(BYTE k[32], BYTE* arr) { 
	BYTE temp[32];
	for (int i = 0; i < 32; i++) {
		temp[i] = k[arr[i] - 1];
	}
	for (int i = 0; i < 32; i++) {
		k[i] = temp[i];
	}
}

void permute32to48(BYTE k[32], BYTE* arr, BYTE out[48]) { 
	for (int i = 0; i < 48; i++) {
		out[i] = k[arr[i] - 1];
	}
}

void permute56to48(BYTE k[56], BYTE* arr, BYTE out[][48] , int loop) {
	for (int i = 0; i < 48; i++) {
		out[loop][i] = k[arr[i] - 1];
	}
}

void permute64to56(BYTE k[64], BYTE* arr, BYTE out[56]) {
	for (int i = 0; i < 56; i++) {
		out[i] = k[arr[i] - 1];
	}
}

//Shift function
void shift_left(BYTE k[28], int shifts) {
	BYTE temp[28] = { 0 };
	for (int i = 0; i < shifts; i++) {
		temp[i] = k[i];
	}
	for (int j = 0; j < 28-shifts; j++) {
		k[j] = k[j + shifts];
	}
	for (int j = 0; j < shifts; j++) {
		k[28 - shifts +j] = temp[j];
	}
}

//XOR functions
void xor48(BYTE a[48], BYTE b[48]) {
	for (int i = 0; i < 48; i++) {
		b[i] = a[i] ^ b[i];
	}
}

void xor32(BYTE a[32], BYTE b[32]) {
	for (int i = 0; i < 32; i++) {
		b[i] = a[i] ^ b[i];
	}
}

//Splitting 32 bits  start=0->(left) / start=1->(right)
void splitto32(BYTE source[64], int start_bit, BYTE output[32]) { 
	if (start_bit == 0) {
		for (int i = 0; i < 32; i++) {
			output[i] = source[i];
		}
	}
	else if (start_bit == 1) {
		for (int i = 0; i < 32; i++) {
			output[i] = source[32 + i];
		}
	}

}

//Splitting 28 bits  start=0->(left) / start=1->(right)
void splitto28(BYTE source[56], int start_bit, BYTE output[28]) { 
	if (start_bit == 0) {
		for (int i = 0; i < 28; i++) {
			output[i] = source[i];
		}
	}
	else if (start_bit == 1) {
		for (int i = 0; i < 28; i++) {
			output[i] = source[28 + i];
		}
	}
}

//Swap function 32 bits
void swap32(BYTE a[32], BYTE b[32]) {
	BYTE temp[32];
	for (int i = 0; i < 32; i++) {
		temp[i] = a[i];
		a[i] = b[i];
		b[i] = temp[i];
	}
}

//Array combination functions
void combiner64(BYTE a[32], BYTE b[32], BYTE c[64]) {
	for (int i = 0; i < 32; i++) {
		c[i] = a[i];
		c[i+32] = b[i];
	}
}

void combiner56(BYTE a[28], BYTE b[28], BYTE c[56]) {
	for (int i = 0; i < 28; i++) {
		c[i] = a[i];
		c[i+28] = b[i];
	}
}

//Key generation function
void key_generator(BYTE key_char[16] , BYTE round_keys[][48]) {
	//Permuted choice table
	BYTE pc_1[56] =
	{ 57,49,41,33,25,17,9,
		1,58,50,42,34,26,18,
		10,2,59,51,43,35,27,
		19,11,3,60,52,44,36,
		63,55,47,39,31,23,15,
		7,62,54,46,38,30,22,
		14,6,61,53,45,37,29,
		21,13,5,28,20,12,4
	};

	//Shift table
	int shift_keys[16] =
	{ 1, 1, 2, 2,
		2, 2, 2, 2,
		1, 2, 2, 2,
		2, 2, 2, 1
	};

	//Key- Compression Table
	BYTE pc_2[48] =
	{ 14,17,11,24,1,5,
		3,28,15,6,21,10,
		23,19,12,4,26,8,
		16,7,27,20,13,2,
		41,52,31,37,47,55,
		30,40,51,45,33,48,
		44,49,39,56,34,53,
		46,42,50,36,29,32
	};

	BYTE key[64];
	hex_to_bin(key_char, key);

	BYTE key56[56];
	//Convert key from 64 bit to 56 bit
	permute64to56(key, pc_1, key56);

	//Split
	BYTE left[28];
	splitto28(key56, 0, left);
	BYTE right[28];
	splitto28(key56, 1, right);

	for (int i = 0; i < 16; i++) {
		//Shift
		shift_left(left, shift_keys[i]);
		shift_left(right, shift_keys[i]);

		//Combine
		combiner56(left, right, key56);

		//Key Compression and key generation
		permute56to48(key56, pc_2, round_keys, i);
	}
}

//Encyrption function
void encrypt(BYTE pt[64], BYTE round_key[16][48], BYTE cipher_text[64] ,int size) {

	//Initial Permutation
	BYTE initial_perm[64] =
	{ 58,50,42,34,26,18,10,2,
		60,52,44,36,28,20,12,4,
		62,54,46,38,30,22,14,6,
		64,56,48,40,32,24,16,8,
		57,49,41,33,25,17,9,1,
		59,51,43,35,27,19,11,3,
		61,53,45,37,29,21,13,5,
		63,55,47,39,31,23,15,7
	};

	//Expansion D-box Table
	BYTE exp_d[48] =
	{ 32,1,2,3,4,5,4,5,
		6,7,8,9,8,9,10,11,
		12,13,12,13,14,15,16,17,
		16,17,18,19,20,21,20,21,
		22,23,24,25,24,25,26,27,
		28,29,28,29,30,31,32,1
	};

	//S-box Table, total 8 s-boxes
	BYTE s[8][4][16] =
	{ {
		14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7, //0
		0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8,
		4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0,
		15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13
	},
	{
		15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10, //1
		3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5,
		0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15,
		13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9
	},
	{
		10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8, //2
		13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1,
		13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7,
		1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12
	},
	{
		7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15, //3
		13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9,
		10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4,
		3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14
	},
	{
		2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9, //4
		14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6,
		4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14,
		11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3
	},
	{
		12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11, //5
		10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8,
		9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6,
		4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13
	},
	{
		4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1, //6
		13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6,
		1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2,
		6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12
	},
	{
		13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7, //7
		1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2,
		7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8,
		2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11
	} };

	//Straight Permutation Table
	BYTE per[32] =
	{ 16,7,20,21,
		29,12,28,17,
		1,15,23,26,
		5,18,31,10,
		2,8,24,14,
		32,27,3,9,
		19,13,30,6,
		22,11,4,25
	};

	//Final Permutation Table
	BYTE final_perm[64] =
	{ 40,8,48,16,56,24,64,32,
		39,7,47,15,55,23,63,31,
		38,6,46,14,54,22,62,30,
		37,5,45,13,53,21,61,29,
		36,4,44,12,52,20,60,28,
		35,3,43,11,51,19,59,27,
		34,2,42,10,50,18,58,26,
		33,1,41,9,49,17,57,25
	};

	//Initial Permutation
	permute64(pt, initial_perm);

	//Splitting
	BYTE left[32];
	splitto32(pt, 0, left);
	BYTE right[32];
	splitto32(pt, 1, right);

#ifdef PRINT
	printf("Round\t\tRound Output\t\tRound Key\n");
#endif // PRINT

	for (int i = 0; i < 16; i++) {

		//Expansion D-box
		BYTE right_expanded[48];
		permute32to48(right, exp_d, right_expanded);

		//xor round_key[i] and right_expanded
		xor48(round_key[i], right_expanded); 

		//S-boxes
		BYTE sbox_out[32];
		for (int j = 0; j < 8; j++) {
			int row = 2 * right_expanded[j * 6] + right_expanded[j * 6 + 5];
			int col = 8 * right_expanded[j * 6 + 1] + 4 * right_expanded[j * 6 + 2] + 2 * right_expanded[j * 6 + 3] + right_expanded[j * 6 + 4];
			int val = s[j][row][col];
			sbox_out[j * 4] = val / 8; val = val % 8;
			sbox_out[(j * 4) + 1] = val / 4; val = val % 4;
			sbox_out[(j * 4) + 2] = val / 2; val = val % 2;
			sbox_out[(j * 4) + 3] = val;
		}

		//Straight P-box
		permute32(sbox_out, per);

		//XOR left and sbox_out store to sbox_out
		xor32(left, sbox_out);

		//Swap operations
		swap32(left, sbox_out);
		if (i != 15) {
			swap32(left, right);
		}
#ifdef PRINT
		printf("%d:\t\t", i + 1);
		bin32_to_hex(left);
		bin32_to_hex(right);
		printf("\t");
		bin48_to_hex(round_key[i]);
		printf("\n");
#endif // PRINT
	}

	//Combination
	combiner64(left, right, cipher_text);

	//Final Permutation
	permute64(cipher_text, final_perm);

#ifdef PRINT
	printf("\nCipher text: ");
	bin64_to_hex(cipher_text,size);
	printf("\n");
#endif // PRINT

}

//Decyrption function
void decrypt(BYTE pt[64], BYTE round_key[16][48], BYTE cipher_text[64],int size) {

	BYTE rev_key[16][48];
	//Reverse the key set
	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < 48; j++)
		{
			rev_key[15 - i][j] = round_key[i][j];
		}
	}
	//Call encryption
	encrypt(pt, rev_key, cipher_text,size);
}

int main() 
{
	BYTE data;
	int size=0;
	int flag = 0;
	BYTE* pt_char = NULL;
	BYTE key1_char[16];
	BYTE key2_char[16];
	BYTE key3_char[16];
	BYTE round_keys1[16][48];//round keys
	BYTE round_keys2[16][48];//round keys
	BYTE round_keys3[16][48];//round keys
	BYTE cipher1_text[64];//ciphertext after first encyrption
	BYTE cipher2_text[64];//ciphertext after second decyrption
	BYTE cipher3_text[64];//ciphertext after third encyrption

	//Read the txt, fill and aallocate related arrays
	FILE* file_new;
	fopen_s(&file_new, "input.txt", "r");
	BYTE size_arr[16];
	int first = 1;
	if (file_new) {
		int row_ctr;
		int column_ctr = 0;

		for (row_ctr = 0; row_ctr < 5; row_ctr++) {
			//Obtain the size as integer from txt
			if (row_ctr == 4)
			{
				float decimal_mul = 1.f;
				int temp = column_ctr;
				//Reverse the array
				for (int i = 0; i < temp - 1; i++) {
					int t = size_arr[i];
					size_arr[i] = size_arr[temp - 1];
					size_arr[temp - 1] = t;
					temp--;
				}

				//Convert to decimal int
				for (int i = 0; i < column_ctr; i++)
				{
					size = size + size_arr[i] * decimal_mul;
					decimal_mul *= 10;
				}
			}
			column_ctr = 0;
			//Read char by char until new line
			while ((data = getc(file_new)) != '\n') {
				if (row_ctr == 0) key1_char[column_ctr++] = data;
				else if (row_ctr == 1) key2_char[column_ctr++] = data;
				else if (row_ctr == 2) key3_char[column_ctr++] = data;
				else if (row_ctr == 3)
				{
					//Ascii to int
					size_arr[column_ctr++] = (int)(data - '0');
				}
				else
				{
					//Allocate only once for not converted plaintext, it will be converted to bit array later
					if (first) {
						pt_char = (BYTE*)malloc(8 * (int)size * sizeof(BYTE));
						first = 0;
					}
					pt_char[column_ctr++] = data;
				}
			}
		}
	}
	fclose(file_new);
	
	//Hex to binary for input arrays
	BYTE* plain_text = (BYTE*)malloc(64 * size * sizeof(BYTE));
	char_to_bin(pt_char, plain_text,size);

	//Generate round keys
	StartCounter();
	key_generator(key1_char, round_keys1);
	key_generator(key2_char, round_keys2);
	key_generator(key3_char, round_keys3);
	double key_gen_time = GetCounter();

	//Loop for all plaintext blocks in the txt
	double avg_time = 0;
	for (int k = 0; k < ITER_COUNT; k++) {
		StartCounter();
		#pragma omp parallel for
		for (int i = 0; i < size; i++){
#ifdef PRINT
			printf("%d th block -------------------------------------------\n",i+1);
			printf("Encrypt\n");
#endif // PRINT
			encrypt((plain_text + 64 * i), round_keys1, cipher1_text, 1);
#ifdef PRINT
			printf("\nDecrypt\n");
#endif // PRINT
			decrypt(cipher1_text, round_keys2, cipher2_text, 1);
#ifdef PRINT
			printf("\nEncrypt\n");
#endif // PRINT
			encrypt(cipher2_text, round_keys3, cipher3_text, 1);
		}
		if (k!=-0) avg_time += GetCounter();
	}
	avg_time = avg_time / (ITER_COUNT-1);
	printf("Avg Key Gen Time CPU (ms): %.3f\n", key_gen_time / 1000);
	printf("Avg Cyrpto Time CPU (ms): %.3f\n", avg_time/1000);
	//Free dynamically allocated memory
	free(pt_char);
	free(plain_text);
	return 0;
}
