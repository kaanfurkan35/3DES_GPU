/*****************************************************************************
KAAN FURKAN ALTINOK 2030021 - GÖKBERK AFŞİN PEKER 2094357
3DES ON GPU IMPLEMENTATION
Course : Applied Parallel Programming on GPU
Lecturer: Dr.Alptekin Temizel

This code read inputs from ascii character (input.txt), processes 3DES on GPU and outputs the encyrpted hexadecimal formatted text to encyrpted_hex.txt 
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <device_functions.h>
#include <cuda.h>
//Custom include
#include "conversion_utils.h"

#define BYTE char
#define ITER_COUNT 1

//Uncomment in order to see the results in console, for a better console view do ITER_COUNT 1 & use a small sized plaintext
#define PRINT

//Kernel to create 3*16 keys
__global__ void genKeysCuda(const BYTE* __restrict__  dev_key1, const BYTE* __restrict__  dev_key2, const BYTE* __restrict__  dev_key3, const BYTE* __restrict__  dev_pc_1, const BYTE* __restrict__ dev_shift_keys, const BYTE* __restrict__  dev_pc_2 , BYTE* __restrict__ dev_round_key1 , BYTE* __restrict__ dev_round_key2, BYTE* __restrict__ dev_round_key3)
{
	BYTE reg_temp;
	BYTE shift_ind;
	__shared__ BYTE temp[56] ;
	const BYTE* key;

	unsigned int tid = threadIdx.x;

	//different blocks for different key sets
	if (blockIdx.x == 0)	    { key = dev_key1;}
	else if (blockIdx.x == 1)	{ key = dev_key2;}
	else if (blockIdx.x == 2)	{ key = dev_key3;}

	//initial permutation to register
	reg_temp = key[dev_pc_1[tid] - 1];

	//loop starts
#pragma unroll
	for (BYTE i = 0; i < 16; i++)
	{
		shift_ind = tid-dev_shift_keys[i];

		//index adjustment for shift. since there is left and right portions, indexes needs careful adjustment
		if (shift_ind == 27) { shift_ind = 55; }
		else if (shift_ind == -1) { shift_ind = 27; }
		else if (shift_ind == 26 && dev_shift_keys[i] == 2) { shift_ind = 54; }
		else if (shift_ind == -2 ) { shift_ind = 26; }

		//automatic shift in parallel
		temp[shift_ind] = reg_temp;

		//wait for all threads
		__syncthreads();

		//save result to register for further use
		reg_temp = temp[tid];

		//final permutation and write back the resulting keys
		if (tid < 48)
		{
			if (blockIdx.x == 0)
			{
				*(dev_round_key1 + i *48 + tid) = temp[dev_pc_2[tid] - 1];
				continue;
			}
			else if (blockIdx.x == 1)
			{
				*(dev_round_key2 + i * 48 + tid) = temp[dev_pc_2[tid] - 1];
				continue;
			}
			else if (blockIdx.x == 2)
			{
				*(dev_round_key3 + i * 48 + tid) = temp[dev_pc_2[tid] - 1];
				continue;
			}
		}

	}
}

//Kernel for encyrption and decyrption purposes CHECK SYNCTHREADS
__global__ void encyrptCuda (const BYTE* __restrict__ dev_key, const BYTE* __restrict__  dev_pt, BYTE* __restrict__ dev_ct, const BYTE* __restrict__ dev_initperm, const BYTE* __restrict__ dev_expd, const BYTE* __restrict__ dev_s, const BYTE* __restrict__ dev_per, const BYTE* __restrict__ dev_finalper)
{
	//Temporary register variable
	BYTE reg_temp;

	//Shared variables
	__shared__ BYTE temp[64];
	__shared__ BYTE temp_loop[48];

	//Thread id calcs
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	unsigned int tid = threadIdx.x;

	//initial permutation to shared mem
	temp[tid] = dev_pt[blockIdx.x * blockDim.x + (dev_initperm[tid] - 1)];
	__syncthreads();

	//loop starts
#pragma unroll
	for (BYTE i = 0; i < 16; i++)
	{
		if (tid < 48)
		{
			//Expansion D-box permutation and XOR with round key in one line
			temp_loop[tid] = dev_key[48 * i + tid] ^ temp[dev_expd[tid] - 1 + 32];
		}
		__syncthreads();

		//S-box operations, compute row and col, get the value from matix, create 4 bit output for each 6bit. Middle 4 bits create col, first and last bit create row.
		if (tid < 8)
		{
			int row = 2 * temp_loop[tid * 6] + temp_loop[tid * 6 + 5];
			int col = 8 * temp_loop[tid * 6 + 1] + 4 * temp_loop[tid * 6 + 2] + 2 * temp_loop[tid * 6 + 3] + temp_loop[tid * 6 + 4];
			BYTE val = dev_s[col + 16 * (row + 4 * tid)];
			temp_loop[tid * 4] = val / 8; val = val % 8;
			temp_loop[(tid * 4) + 1] = val / 4; val = val % 4;
			temp_loop[(tid * 4) + 2] = val / 2; val = val % 2;
			temp_loop[(tid * 4) + 3] = val;
		}
		__syncthreads();

		//Straight P-box
		if (tid < 32)
		{
			//Permutation and XORing it with left portion of plaintext
			reg_temp = temp[tid] ^ temp_loop[dev_per[tid] - 1];
			
			//Swap operations
			if (i == 15)
			{
				//no swap at last step
				temp[tid] = reg_temp;
				goto out;
			}
			//swap left and right, change right portion with XORed data
			temp[tid] = temp[tid + 32];
			temp[tid + 32] = reg_temp;
		}
		__syncthreads();
	}
out:
	__syncthreads();
	//Final permutation
	dev_ct[idx] = temp[dev_finalper[tid] - 1];
}

// Helper function for using CUDA to create 3DES keys.
cudaError_t keyGen(BYTE* key1, BYTE* key2, BYTE* key3, BYTE round_key1[][48], BYTE round_key2[][48], BYTE round_key3[][48])
{
	//Cuda pointers
	BYTE* dev_key1 = nullptr;
	BYTE* dev_key2 = nullptr;
	BYTE* dev_key3 = nullptr;
	BYTE* dev_round_key1 = nullptr;
	BYTE* dev_round_key2 = nullptr;
	BYTE* dev_round_key3 = nullptr;
	BYTE* dev_pc_1 = nullptr;
	BYTE* dev_shift_keys = nullptr;
	BYTE* dev_pc_2 = nullptr;

    cudaError_t cudaStatus;

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
	BYTE shift_keys[16] =
	{ 1, 1, 2, 2,
		2, 2, 2, 2,
		1, 2, 2, 2,
		2, 2, 2, 1
	};

	//Key Compression Table
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

    // Choose which GPU to run on, change this on a multi-GPU system.
    cudaStatus = cudaSetDevice(0);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
        goto Error;
    }

    // Allocate GPU buffers for three keys and matrices
    cudaStatus = cudaMalloc((void**)&dev_key1, 64 * sizeof(BYTE));
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }

    cudaStatus = cudaMalloc((void**)&dev_key2, 64 * sizeof(BYTE));
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }

    cudaStatus = cudaMalloc((void**)&dev_key3, 64 * sizeof(BYTE));
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }

	cudaStatus = cudaMalloc((void**)&dev_pc_1, 56 * sizeof(BYTE));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}

	cudaStatus = cudaMalloc((void**)&dev_shift_keys, 16 * sizeof(BYTE));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}

	cudaStatus = cudaMalloc((void**)&dev_pc_2, 48 * sizeof(BYTE));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}

	cudaStatus = cudaMalloc((void**)&dev_round_key1, 16 * 48 * sizeof(BYTE));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}

	cudaStatus = cudaMalloc((void**)&dev_round_key2, 16 * 48 * sizeof(BYTE));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}

	cudaStatus = cudaMalloc((void**)&dev_round_key3, 16 * 48 * sizeof(BYTE));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}

    // Copy data from host memory to GPU buffers.
    cudaStatus = cudaMemcpy(dev_key1, key1, 64 * sizeof(BYTE), cudaMemcpyHostToDevice);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy failed!");
        goto Error;
    }

    cudaStatus = cudaMemcpy(dev_key2, key2, 64 * sizeof(BYTE), cudaMemcpyHostToDevice);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy failed!");
        goto Error;
    }

	cudaStatus = cudaMemcpy(dev_key3, key3, 64 * sizeof(BYTE), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

	cudaStatus = cudaMemcpy(dev_pc_1, pc_1, 56 * sizeof(BYTE), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

	cudaStatus = cudaMemcpy(dev_shift_keys, shift_keys, 16 * sizeof(BYTE), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

	cudaStatus = cudaMemcpy(dev_pc_2, pc_2, 48 * sizeof(BYTE), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

	cudaStatus = cudaMemset(dev_round_key1, 0, 16 * 48 * sizeof(BYTE));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemset failed!");
		goto Error;
	}

	cudaStatus = cudaMemset(dev_round_key2, 0, 16 * 48 * sizeof(BYTE));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemset failed!");
		goto Error;
	}

	cudaStatus = cudaMemset(dev_round_key3, 0, 16 * 48 * sizeof(BYTE));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemset failed!");
		goto Error;
	}

    // Launch a kernel on the GPU. A block for each key set. 56 threads each block
    genKeysCuda<<<3, 56>>>(dev_key1, dev_key2, dev_key3, dev_pc_1, dev_shift_keys, dev_pc_2, dev_round_key1, dev_round_key2, dev_round_key3);


    // Check for any errors launching the kernel
    cudaStatus = cudaGetLastError();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "addKernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
        goto Error;
    }
    
    // cudaDeviceSynchronize waits for the kernel to finish, and returns
    // any errors encountered during the launch.
    cudaStatus = cudaDeviceSynchronize();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
        goto Error;
    }

    // Copy resulting keys from GPU buffer to host memory.
    cudaStatus = cudaMemcpy(round_key1, dev_round_key1, 16 * 48 * sizeof(BYTE), cudaMemcpyDeviceToHost);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy failed!");
        goto Error;
    }

	cudaStatus = cudaMemcpy(round_key2, dev_round_key2, 16 * 48 * sizeof(BYTE), cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

	cudaStatus = cudaMemcpy(round_key3, dev_round_key3, 16 * 48 * sizeof(BYTE), cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

#ifdef PRINT
	//Print the keys
	printf("KEY SET 1:\n");
	for (int i = 0; i < 16; i++) {
		printf("%d.key:  ", i);
		bin48_to_hex(round_key1[i]);
		printf("\n");
	}
	printf("\nKEY SET 2:\n");
	for (int i = 0; i < 16; i++) {
		printf("%d.key:  ", i);
		bin48_to_hex(round_key2[i]);
		printf("\n");
	}
	printf("\nKEY SET 3:\n");
	for (int i = 0; i < 16; i++) {
		printf("%d.key:  ", i);
		bin48_to_hex(round_key3[i]);
		printf("\n");
	}
#endif

Error:
    cudaFree(dev_key3);
    cudaFree(dev_key1);
    cudaFree(dev_key2);
	cudaFree(dev_round_key1);
	cudaFree(dev_round_key2);
	cudaFree(dev_round_key3);
	cudaFree(dev_pc_1);
	cudaFree(dev_shift_keys);
	cudaFree(dev_pc_2);

    return cudaStatus;
}

// Helper function for using CUDA to process 3DES plaintext.
cudaError_t encrypt(BYTE* pt, BYTE round_key[16][48], BYTE* cipher_text, int ptblockSize)
{
	//Cuda pointers
	BYTE* dev_key = nullptr;
	BYTE* dev_pt = nullptr;
	BYTE* dev_ct = nullptr;
	BYTE* dev_initperm = nullptr;
	BYTE* dev_expd = nullptr;
	BYTE* dev_s = nullptr;
	BYTE* dev_per = nullptr;
	BYTE* dev_finalper = nullptr;
	cudaError_t cudaStatus;

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

	// Choose which GPU to run on, change this on a multi-GPU system.
	cudaStatus = cudaSetDevice(0);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
		goto Error;
	}

	// Allocate GPU buffers
	cudaStatus = cudaMalloc((void**)&dev_key, 16 * 48 * sizeof(BYTE));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}

	cudaStatus = cudaMalloc((void**)&dev_pt, 64 * ptblockSize *sizeof(BYTE));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}

	cudaStatus = cudaMalloc((void**)&dev_ct, 64 * ptblockSize * sizeof(BYTE));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}

	cudaStatus = cudaMalloc((void**)&dev_initperm, 64 * sizeof(BYTE));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}

	cudaStatus = cudaMalloc((void**)&dev_expd, 48 * sizeof(BYTE));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}

	cudaStatus = cudaMalloc((void**)&dev_s, 8 * 4 * 16 * sizeof(BYTE));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}

	cudaStatus = cudaMalloc((void**)&dev_per, 32 * sizeof(BYTE));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}

	cudaStatus = cudaMalloc((void**)&dev_finalper, 64 * sizeof(BYTE));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}

	// Copy input data from host memory to GPU buffers.
	cudaStatus = cudaMemcpy(dev_key, round_key, 16 * 48 * sizeof(BYTE), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

	cudaStatus = cudaMemcpy(dev_pt, pt, 64 * ptblockSize * sizeof(BYTE), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

	cudaStatus = cudaMemset(dev_ct, 0, 64 * ptblockSize * sizeof(BYTE));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

	cudaStatus = cudaMemcpy(dev_initperm, initial_perm, 64 * sizeof(BYTE), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

	cudaStatus = cudaMemcpy(dev_expd, exp_d, 48 * sizeof(BYTE), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

	cudaStatus = cudaMemcpy(dev_s, s, 8 * 4 * 16 * sizeof(BYTE), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

	cudaStatus = cudaMemcpy(dev_per, per, 32 * sizeof(BYTE), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

	cudaStatus = cudaMemcpy(dev_finalper, final_perm, 64 * sizeof(BYTE), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

	// Launch a kernel on the GPU, each 64 bit plaintext block counts as 1 block and each block has 64 threads due to 64 bit plaintext processing
	encyrptCuda << <ptblockSize, 64 >> > (dev_key, dev_pt, dev_ct, dev_initperm, dev_expd, dev_s, dev_per, dev_finalper);

	// Check for any errors launching the kernel
	cudaStatus = cudaGetLastError();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "Encrypt launch failed: %s\n", cudaGetErrorString(cudaStatus));
		goto Error;
	}

	// cudaDeviceSynchronize waits for the kernel to finish, and returns
	// any errors encountered during the launch.
	cudaStatus = cudaDeviceSynchronize();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching encrypt kernel!\n", cudaStatus);
		goto Error;
	}

	// Copy ciphertext from GPU buffer to host memory.
	cudaStatus = cudaMemcpy(cipher_text, dev_ct, 64 * ptblockSize* sizeof(BYTE), cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

Error:
	cudaFree(dev_key);
	cudaFree(dev_pt);
	cudaFree(dev_ct);
	cudaFree(dev_initperm);
	cudaFree(dev_expd);
	cudaFree(dev_s);
	cudaFree(dev_per);
	cudaFree(dev_finalper);

	return cudaStatus;
}

cudaError_t decrypt(BYTE* pt, BYTE round_key[16][48], BYTE* cipher_text, int ptblockSize) {

	BYTE rev_key[16][48];

	//Reverse the original key
	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < 48; j++)
		{
			rev_key[15 - i][j] = round_key[i][j];
		}
	}
	//Call encryption
	return encrypt(pt, rev_key, cipher_text, ptblockSize);
}

//Function that reads the formatted text. The structure of txt can be found in README.txt
void readText(BYTE* size_arr, float &size, BYTE* key1_int, BYTE* key2_int, BYTE* key3_int, BYTE* &pt_int)
{
	//Please refer to report for input text format which includes plaintext, keys and plaintext block size
	int data;
	FILE* file_new;
	file_new = fopen("input.txt", "r");
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
				if (row_ctr == 0) key1_int[column_ctr++] = data;
				else if (row_ctr == 1) key2_int[column_ctr++] = data;
				else if (row_ctr == 2) key3_int[column_ctr++] = data;
				else if (row_ctr == 3)
				{
					//Ascii to int
					size_arr[column_ctr++] = (int)(data - '0');
				}
				else
				{
					//Allocate only once for not converted plaintext, it will be converted to bit array later
					for (static bool first = true; first; first = false)
					{
						pt_int = (BYTE*)malloc(8 * (int)size * sizeof(BYTE));
					}
					pt_int[column_ctr++] = data;
				}
			}
		}
	}
	fclose(file_new);
}

void write_text(BYTE* input, int size, const char *str ) {
	int buf;
	FILE* fp;
	fp = fopen(str, "w");
	for (int j = 0; j < 16 * size; j++) {
		buf = 8 * input[j * 4] + 4 * input[j * 4 + 1] + 2 * input[j * 4 + 2] + input[j * 4 + 3];
		//printf("%01X", buf);
		fprintf(fp, "%X", buf);
	}
	fclose(fp);
}

int main()
{
	//Variables
	float size = 0.f;//holds plaintext block size which consists of 64bits
	BYTE* plain_text = nullptr;//plaintext
	//For encryption
	BYTE* cipher1_text = nullptr;//ciphertext after first encyrption
	BYTE* cipher2_text = nullptr;//ciphertext after second decyrption
	BYTE* cipher3_text = nullptr;//ciphertext after third encyrption
	//For decryption
	BYTE* deciphered1_text = nullptr;//ciphertext after first decyrption
	BYTE* deciphered2_text = nullptr;//ciphertext after second encyrption
	BYTE* deciphered3_text = nullptr;//ciphertext after third decyrption

	BYTE round_keys1[16][48];//round keys
	BYTE round_keys2[16][48];//round keys
	BYTE round_keys3[16][48];//round keys

	//Temporary items, used only for conversions from txt
	BYTE* pt_int = nullptr;//plaintext read from file
	BYTE key1_int[16];//key read from file
	BYTE key2_int[16];//key read from file
	BYTE key3_int[16];//key read from file
	BYTE size_arr[16];
	BYTE key1[64];//base key
	BYTE key2[64];//base key
	BYTE key3[64];//base key

	//Read the txt file to related pointers
	readText(size_arr, size, key1_int, key2_int, key3_int, pt_int);

	//Allocate buffers dynamically
	cipher1_text = (BYTE*)malloc(64 * (int)size * sizeof(BYTE));
	cipher2_text = (BYTE*)malloc(64 * (int)size * sizeof(BYTE));
	cipher3_text = (BYTE*)malloc(64 * (int)size * sizeof(BYTE));
	deciphered1_text = (BYTE*)malloc(64 * (int)size * sizeof(BYTE));
	deciphered2_text = (BYTE*)malloc(64 * (int)size * sizeof(BYTE));
	deciphered3_text = (BYTE*)malloc(64 * (int)size * sizeof(BYTE));
	plain_text = (BYTE*)malloc(64 * (int)size * sizeof(BYTE));

	//Hex to binary for input arrays
	char_to_bin(pt_int, plain_text, (int)size);
	hex_to_bin(key1_int, key1);
	hex_to_bin(key2_int, key2);
	hex_to_bin(key3_int, key3);
	cudaError_t cudaStatus;

	//Call key generation
	for (int i = 0; i < ITER_COUNT; i++) {
		cudaStatus = keyGen(key1, key2, key3, round_keys1, round_keys2, round_keys3);
		if (cudaStatus != cudaSuccess) {
			fprintf(stderr, "key generation failed!");
			return 1;
		}
	}
	//3DES Encryption
	for (int i = 0; i < ITER_COUNT; i++) {
		cudaStatus = encrypt(plain_text, round_keys1, cipher1_text, (int)size);
		if (cudaStatus != cudaSuccess) {
			fprintf(stderr, "encyrption1 %d failed!", i);
			return 1;
		}

		cudaStatus = decrypt(cipher1_text, round_keys2, cipher2_text, (int)size);
		if (cudaStatus != cudaSuccess) {
			fprintf(stderr, "decyrption2 failed!");
			return 1;
		}

		cudaStatus = encrypt(cipher2_text, round_keys3, cipher3_text, (int)size);
		if (cudaStatus != cudaSuccess) {
			fprintf(stderr, "encyrption3 failed!");
			return 1;
		}
	}

	//3DES Decryption to get the original plaintext
	for (int i = 0; i < ITER_COUNT; i++) {
		cudaStatus = decrypt(cipher3_text, round_keys3, deciphered3_text, (int)size);
		if (cudaStatus != cudaSuccess) {
			fprintf(stderr, "decyrption4 failed!");
			return 1;
		}

		cudaStatus = encrypt(deciphered3_text, round_keys2, deciphered2_text, (int)size);
		if (cudaStatus != cudaSuccess) {
			fprintf(stderr, "encyrption5 failed!");
			return 1;
		}

		cudaStatus = decrypt(deciphered2_text, round_keys1, deciphered1_text, (int)size);
		if (cudaStatus != cudaSuccess) {
			fprintf(stderr, "decyrption6 failed!");
			return 1;
		}
	}
	// cudaDeviceReset must be called before exiting in order for profiling and
	// tracing tools such as Nsight and Visual Profiler to show complete traces.
	cudaStatus = cudaDeviceReset();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaDeviceReset failed!");
		return 1;
	}

	//Print the results
#ifdef PRINT
	printf("\nPlaint text in ASCII:\n");
	bin64_to_hex(plain_text, (int)size);
	printf("\n\nEncyrpted text step 1:\n");
	bin64_to_hex(cipher1_text, (int)size);
	printf("\n\nDecyrpted text step 2:\n");
	bin64_to_hex(cipher2_text, (int)size);
	printf("\n\nFinal encyrpted text step 3:\n");
	bin64_to_hex(cipher3_text, (int)size);
	printf("\n\nDecyrpted text step 1:\n");
	bin64_to_hex(deciphered3_text, (int)size);
	printf("\n\nEncyrpted text step 2:\n");
	bin64_to_hex(deciphered2_text, (int)size);
	printf("\n\nFinal decyrpted text step 3:\n");
	bin64_to_hex(deciphered1_text, (int)size);
	printf("\n\n");
#endif //PRINT

	//Output the plaintext hex values and encyrpted hex values to plain_hex.txt & encyrpted_hex.txt 
	write_text(plain_text, (int)size, "plain_hex.txt");
	write_text(cipher3_text, (int)size, "encyrpted_hex.txt");


	//Free to prevent memory leak
	free(pt_int);
	free(plain_text);
	free(cipher1_text);
	free(cipher2_text);
	free(cipher3_text);
	free(deciphered1_text);
	free(deciphered2_text);
	free(deciphered3_text);

	return 0;
}
