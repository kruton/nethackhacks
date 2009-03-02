#include <stdio.h>
#include "cutil.h"

#define VECSIZE 31
#define FIRST_USABLE 344
#define USABLE_TOTAL 16
#define POOL_SIZE (FIRST_USABLE + USABLE_TOTAL)
#define NUM_THREADS 256
#define NUM_BLOCKS 128

#define MAX_HITS 32

#define MAX_MATCH_ATTEMPTS 3

#define MAX_ITER 0xFFFFFFFF
#define NUM_PER_ITER (NUM_THREADS * NUM_BLOCKS)
#define TOTAL_ITER (MAX_ITER / NUM_PER_ITER)

#define MUL(a, b) __umul24(a, b)

#define RND(a) (((unsigned int)buffer[a]) >> 1)

typedef struct {
	unsigned int fact[VECSIZE];
} intvec;

__global__ void attack(const unsigned int seed, const intvec *pool, const int *engraving, const int engraving_length, const int *offsets, const int *changes, const int num_changes, unsigned int *hits, unsigned int *hit_number) {
	unsigned int idx = MUL(blockIdx.x, blockDim.x) + threadIdx.x;
	int buffer[POOL_SIZE];

	long int word = seed + idx;
	buffer[0] = word;

	/* Initial seed of the RNG for period 31 */
	#pragma unroll
	for (int i = 1; i < 31; ++i) {
		long int hi = word / 127773;
		long int lo = word % 127773;
		word = MUL(16807, lo) - MUL(2836, hi);
		if (word < 0)
			word += 2147483647;
		buffer[i] = word;
	}

	/* Finish up RNG state */
	#pragma unroll
	for (int i = 31; i < 34; ++i) {
		buffer[i] = buffer[i - 31];
	}

	/* Generate numbers to discard plus USABLE_TOTAL real numbers */
	#pragma unroll
	for (int i = 34; i < POOL_SIZE; ++i) {
		buffer[i] = buffer[i - 31] + buffer[i - 3];
	}

	/* Start with the first usable number */
	int randidx = FIRST_USABLE;

	int letter;
	int offset;
	int attempt = 0;
	int i = 0;
	for (i = 0; i < num_changes && randidx < POOL_SIZE; ++i) {
		do offset = RND(randidx++) % engraving_length;
			while (offset != offsets[i] && ++attempt < MAX_MATCH_ATTEMPTS);
		if (attempt == MAX_MATCH_ATTEMPTS)
			return;
		do letter = RND(randidx++) % 25;
			while (letter == engraving[offsets[i]]);
		if (letter != changes[i])
			return;
	}

	unsigned int hit_location = atomicAdd(hit_number, 1);
	if (hit_location < MAX_HITS)
		hits[hit_location] = buffer[0];
}

void
init(int offset, intvec *pool)
{
    int kc = VECSIZE * 10 + offset;

    for (int i = 0; i < VECSIZE; ++i)
        pool[i].fact[i] = 1;

    int f = 3; int r = 0;
    while (--kc >= 0) {
        for (int i = 0; i < 31; ++i)
            pool[f].fact[i] += pool[r].fact[i];
        ++f;
        if (f >= VECSIZE) {
            f = 0;
            ++r;
        } else {
            ++r;
            if (r >= VECSIZE)
              r = 0;
        }
    }
}

long int find_seed(const char *engraving, int engraving_length, const int *offsets, const int *changes, int num_changes) {
	dim3 threads, blocks;
	float time_kernel;

	int cuda_device = 0;
	int num_devices = 0;
	CUDA_SAFE_CALL( cudaGetDeviceCount(&num_devices) );

	if (num_devices == 0) {
		printf("Your system does not have a CUDA-capable device.\n");
		return 1;
	}

	if (cuda_device >= num_devices) {
		printf("Choose device ID between 0 and %d.\n", num_devices - 1);
		return 1;
	}

	cudaSetDevice(cuda_device);

	cudaEvent_t start_event, stop_event;
	CUDA_SAFE_CALL( cudaEventCreate(&start_event) );
	CUDA_SAFE_CALL( cudaEventCreate(&stop_event) );

	/* Convert engraving to integers */
	int *eng_ints = (int *)malloc(sizeof(int) * engraving_length);
	for (int i = 0; i < engraving_length; ++i)
		eng_ints[i] = engraving[i] - 'a';
	
	/* Allocate device buffers */
	int *engraving_d, *changes_d;
	int *offsets_d;
	intvec *pool_d;
	unsigned int *hits_d, *hit_number_d;
	CUDA_SAFE_CALL( cudaMalloc((void**)&engraving_d, sizeof(int) * engraving_length) );
	CUDA_SAFE_CALL( cudaMalloc((void**)&offsets_d, sizeof(int) * num_changes) );
	CUDA_SAFE_CALL( cudaMalloc((void**)&changes_d, sizeof(int) * num_changes) );
	CUDA_SAFE_CALL( cudaMalloc((void**)&hits_d, sizeof(unsigned int) * MAX_HITS) );
	CUDA_SAFE_CALL( cudaMalloc((void**)&hit_number_d, sizeof(unsigned int)) );
	CUDA_SAFE_CALL( cudaMalloc((void**)&pool_d, sizeof(intvec) * VECSIZE) );

	/* Set up parameters of kernel run */
	threads = dim3(NUM_THREADS, 1);
	blocks = dim3(NUM_BLOCKS, 1);

	/* seed initial pool data */
	intvec pool[31];
	init(2, pool);

	/* initialize hit data */
	unsigned int hit_number = 0;
	unsigned int hits[MAX_HITS];
	unsigned long seed = 0;

	/* Copy all data from host to device */
	printf("Copying data from host to CUDA device...\n");
	cudaMemcpy(engraving_d, eng_ints, sizeof(int) * engraving_length, cudaMemcpyHostToDevice);
	cudaMemcpy(offsets_d, offsets, sizeof(int) * num_changes, cudaMemcpyHostToDevice);
	cudaMemcpy(changes_d, changes, sizeof(int) * num_changes, cudaMemcpyHostToDevice);
	cudaMemcpy(hits_d, &hits, sizeof(unsigned int) * MAX_HITS, cudaMemcpyHostToDevice);
	cudaMemcpy(hit_number_d, &hit_number, sizeof(unsigned int), cudaMemcpyHostToDevice);
	cudaMemcpy(pool_d, &pool, sizeof(intvec) * VECSIZE, cudaMemcpyHostToDevice);

	/* Print out initial status bar for user to see. */
	printf("Starting seed search on CUDA device...\n");
	printf("[");
	for (int i = 0; i < 50; ++i)
		printf(".");
	printf("] %3u%%", 0);
	fflush(stdout);

	/* Run kernel in loop to prevent GUI from becoming unresponsive */
	cudaEventRecord(start_event, 0);
	const int report_after = TOTAL_ITER / 100;
	int reports = report_after;
	for (int i = 0; i < TOTAL_ITER && !hit_number; ++i) {
		attack<<<threads, blocks>>>(seed, pool_d, engraving_d, engraving_length, offsets_d, changes_d, num_changes, hits_d, hit_number_d);
		CUT_CHECK_ERROR("Kernel execution failed");
		cudaMemcpy(&hit_number, hit_number_d, sizeof(unsigned int), cudaMemcpyDeviceToHost);
		if (hit_number > 0)
			cudaMemcpy(&hits, hits_d, sizeof(unsigned int) * MAX_HITS, cudaMemcpyDeviceToHost);
		seed += NUM_PER_ITER;
		if (--reports == 0) {
			unsigned int pct_complete = (seed * 100LL) / MAX_ITER;
			unsigned int blocks_done = pct_complete / 2;
			printf("\r[");
			for (int pct = 0; pct < blocks_done; ++pct)
				printf("#");
			for (int pct = blocks_done; pct < 50; ++pct)
				printf(".");
			printf("] %3u%%", pct_complete);
			fflush(stdout);
			reports = report_after;
		}
	}
	cudaEventRecord(stop_event, 0);
	cudaEventSynchronize(stop_event);
	CUDA_SAFE_CALL( cudaEventElapsedTime(&time_kernel, start_event, stop_event) );
	printf("\n\nFinished scan of space in %.2f ms (%.2f seeds/ms)\n", time_kernel, seed / time_kernel);

	printf("number of hits = %u, last seed = %u\n", hit_number, seed);
	if (hit_number > 0) {
		printf("Seed hits:\n");
		for (int i = 0; i < min(hit_number, MAX_HITS); ++i) {
			printf("\t%u\n", hits[i]);
		}
	}

	/* Tear down for exit */
	cudaEventDestroy(start_event);
	cudaEventDestroy(stop_event);

	CUDA_SAFE_CALL( cudaFree((void**)&engraving_d) );
	CUDA_SAFE_CALL( cudaFree((void**)&offsets_d) );
	CUDA_SAFE_CALL( cudaFree((void**)&changes_d) );
	CUDA_SAFE_CALL( cudaFree((void**)&hits_d) );
	CUDA_SAFE_CALL( cudaFree((void**)&hit_number_d) );

	free(eng_ints);

	return hit_number;
}
