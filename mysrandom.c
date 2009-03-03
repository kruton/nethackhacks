#include <stdint.h>
#include <stdio.h>
#define __USE_MISC
#define __USE_BSD
#include <stdlib.h>

typedef struct { uint32_t fact[31]; } intvec;

intvec pool[31];
int f, r;

void
init(int offset)
{
    int kc = 31 * 10 + offset;

    int i;
    for (i = 0; i < 31; ++i)
    	pool[i].fact[i] = 1;

    f = 3; r = 0;
    while (--kc >= 0) {
	for (i = 0; i < 31; ++i)
	    pool[f].fact[i] += pool[r].fact[i];
	++f;
	if (f >= 31) {
	    f = 0;
	    ++r;
	} else {
	    ++r;
	    if (r >= 31)
	      r = 0;
	}
    }
#if 0
    for (int j = 0; j < 31; ++j) {
	printf("%d:\n", j);
	for (int i = 0; i < 31; ++i)
	    printf("\t%u\n", pool[j].fact[i]);
    }
#endif
}

#if 0
inline int
mysrandom_r(unsigned int seed, struct random_data *buf) {
	int p[31];
	int i, j;
	static int first = 1;

	if (first) { first = 0; init(0); }
	p[0] = seed;
	for (i=1; i<31; i++) {
		p[i] = (16807LL * p[i-1]) % 2147483647;
		if (p[i] < 0) {
			p[i] += 2147483647;
		}
	}

	for (j = 0; j < 31; ++j) {
		buf->state[j] = 0;
		for (i = 0; i < 31; ++i) {
			buf->state[j] += pool[j].fact[i] * p[i];
		}
	}
	buf->rand_type = 3;
	buf->fptr = &buf->state[f];
	buf->rptr = &buf->state[r];
	return 0;
}

static struct random_data data;
static int table[32];
void
srandom(unsigned int seed) {
	data.state = &table[1];
	mysrandom_r(seed, &data);
}
long int
random() {
	int32_t retval;
	random_r(&data, &retval);
	return retval;
}
#endif

static int p[31];
static inline int nthrandom(int myf)
{
	int i;
	int result = 0;
	for (i = 0; i < 31; ++i) {
		result += pool[myf].fact[i] * p[i];
	}
	return (result >> 1) & 0x7fffffff;
}

static int first = 1;
static int myf, myf2;
int
firstrandom(unsigned int seed) {
	int i;

	if (first) {
		first = 0;
		init(2);
		myf = f - 2;
		if (myf < 0) myf += 31;
		myf2 = f - 1;
		if (myf2 < 0) myf2 += 31;
	}
	if (seed) {
	    p[0] = seed;
	    p[1] = (16807LL * p[0]) % 2147483647;
	    if (p[1] < 0) {
		    p[1] += 2147483647;
	    }
	    /* this is a non-dividing modulo.  it fails for 2147483647 and
	     * doesn't have the special cases for negative numbers, but they
	     * can never occur after the first seed, hence the one step above
	     */
	    for (i=2; i<31; i++) {
		    uint64_t m = 16807ULL * p[i-1];
		    uint32_t m2 = (m & 0x7fffffff) + (m >> 31);
		    uint32_t m3 = (m2 & 0x7fffffff) + (m2 >> 31);
		    p[i] = m3;
	    }
	}
	return nthrandom(myf);
}

int
secondrandom()
{
	return nthrandom(myf2);
}

#if 0
main()
{
    srandom(1000);
    printf("%d %d\n", random(), firstrandom(1000));

}
#endif
