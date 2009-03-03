/*
 * nhwisher.cpp
 *
 *  Created on: Feb 27, 2009
 *      Author: kenny
 */

#include <iostream>
#include <cstdlib>
#include <vector>
#include <stdlib.h>
#include <getopt.h>
#include <limits.h>
#include <stdint.h>

using namespace std;

#if 0
#define likely(x)		__builtin_expect((x),1)
#define unlikely(x)		__builtin_expect((x),0)
#else
#define likely(x)		(x)
#define unlikely(x)		(x)
#endif

extern "C" int firstrandom(unsigned int seed);
extern "C" int secondrandom();

vector<uint32_t> random_cache;
uint32_t last_random;
string program;

void print_usage() {
	cout << "Usage: " << program << " [options] -s <srandom seed> -e <engraving> <corrupted> [<corrupted> ...]" << endl;
	cout << "       " << program << " [options] -e <engraving> <corrupted> [<corrupted> ...]" << endl;
	cout << endl;
	cout << "       -h" << endl;
	cout << "       --help                this help screen" << endl;
	cout << "       -s <seed>" << endl;
	cout << "       --seed <ssed>         use specified srandom seed" << endl;
	cout << "       -o" << endl;
	cout << "       --offset <integer>    number of random() calls to make before starting" << endl;
	cout << "       -e" << endl;
	cout << "       --engraving <string>  engraving you are trying to simulate" << endl;
	cout << "       -b" << endl;
	cout << "       --begin <integer>     starting seed to check" << endl;
	cout << "       -l" << endl;
	cout << "       --last <integer>      end seed to check" << endl;
}

inline char highc(char c) {
	return ((char) (('a' <= c && c <= 'z') ? (c & ~040) : c));
}

inline char lowc(char c) {
	return ((char) (('A' <= c && c <= 'Z') ? (c | 040) : c));
}

inline uint32_t rn2(int x) {
	if (!random_cache.empty()) {
		last_random = random_cache.front();
		random_cache.erase(random_cache.begin());
	} else
		last_random = random();
	return last_random % x;
}

inline void save_random(uint32_t number) {
	if (random_cache.empty())
		random_cache.push_back(number);
	else
		random_cache.insert(random_cache.begin(), number);
}

inline void save_last() {
	save_random(last_random);
}

void findChange(string engraving, string corruption, uint32_t &offset, uint32_t &change) {
	for (uint32_t x = 0; x < engraving.size(); ++x)
		if (engraving[x] != corruption[x]) {
			offset = x;
			change = lowc(corruption[x]) - 'a';
			cout << "Corruption: index " << offset << " changed to " << corruption[x] << " (" << change << ")" << endl;
		}
}

bool searchWithinSeed(const string &engraving, const vector<uint32_t> &offsets, const vector<uint32_t> &changes, uint32_t &offset, const uint32_t &depth) {
	uint32_t cur_index = 0;
	vector<uint32_t> cur_sequence;

	while (true) {
		uint32_t n = rn2(engraving.size());
		if (likely(offsets[cur_index] != n)) {
			++offset;
			if (offset == 0xFFFFFFFF || (depth != 0 && offset >= depth))
				return false;
			else
				continue;
		}

		unsigned char c1, c2;

		c1 = engraving[n];
		do {
			c2 = rn2('z' - 'a');
			++offset;
			if (offset == 0xFFFFFFFF || (depth != 0 && offset >= depth))
				return false;
		} while (c1 == c2);

		if (changes[cur_index] == c2) {
			++cur_index;
			cur_sequence.push_back(n);
			cur_sequence.push_back(c2);
			if (cur_index == offsets.size())
				break;
		} else {
			if (cur_index > 0) {
				/* try to match subsequences within the previous sequence match */
			}
			save_last();
		}
	}

	return true;
}

int main(int argc, char *argv[]) {
	program = string(argv[0]);
	const char* const short_options = "hs:o:e:f:l:";
	const struct option long_options[] = {
			{"help", 0, NULL, 'h'},
			{"seed", 1, NULL, 's'},
			{"offset", 1, NULL, 'o'},
			{"engraving", 1, NULL, 'e'},
			{"first", 1, NULL, 'f'},
			{"last", 1, NULL, 'l'},
			{NULL, 0, NULL, 0}
	};

	uint32_t offset = 0;
	uint32_t seed = 1;
	bool seed_search = true;
	string engraving;
	vector<uint32_t> offsets;
	vector<uint32_t> changes;
	uint32_t first = 1, last = 4294967295U;

	int next_opt;
	while ((next_opt = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
		switch (next_opt) {
		case 'h':
			print_usage();
			return EXIT_SUCCESS;
		case 's':
			seed = (unsigned int)strtoul(optarg, NULL, 0);
			seed_search = false;
			break;
		case 'e':
			engraving = string(optarg);
			transform(engraving.begin(), engraving.end(), engraving.begin(), ptr_fun(::tolower));
			break;
		case 'f':
			first = (unsigned int)strtoul(optarg, NULL, 0);
			break;
		case 'l':
			last = (unsigned int)strtoul(optarg, NULL, 0);
			break;
		}
	}

	if (engraving.size() <= 0) {
		print_usage();
		cout << endl << "Engraving argument is required!" << endl;
		return EXIT_FAILURE;
	}

	for (int i = optind; i < argc; i++) {
		uint32_t offset, change;

		string corruption = string(argv[i]);
		transform(corruption.begin(), corruption.end(), corruption.begin(), ptr_fun(::tolower));
		findChange(engraving, corruption, offset, change);

		offsets.push_back(offset);
		changes.push_back(change);
	}

	if (offsets.size() == 0) {
		print_usage();
		cout << endl << "Please use one or more corrupted engravings!" << endl;
		return EXIT_FAILURE;
	}

	if (seed_search) {
		/* search for the seed */
		uint32_t seed = first;
		uint32_t this_offset = offset;
		time_t start_time = time(0);

		while (seed < last) {
			uint32_t n = firstrandom(seed) % engraving.size();
			if (unlikely(n == offsets[0])) {
				uint32_t m = secondrandom() % ('z' - 'a');
				if (unlikely(m == (unsigned int)engraving[n] || m == changes[0])) {
					srandom(seed);
					for (uint32_t i = 0; i < this_offset; i++)
						random();
					if (unlikely(searchWithinSeed(engraving, offsets, changes, this_offset, 1))) {
						cout << "Seed found " << seed << endl;
					}
					this_offset = offset;
				}
			}
			this_offset = offset;
			if (unlikely((++seed % (4*524288)) == 0)) {
				uint32_t completed = seed - first;
				uint32_t elapsed = time(0) - start_time;
				uint32_t remain = last - seed;
				long eta = (remain / completed) * elapsed;

				cout << "Checking seed: " << seed << ", ETA: " << eta;
				if (elapsed > 0)
					cout << ", " << (completed / elapsed) << " per second";
				cout << endl;
			}
		}

		offset = this_offset;

		if (seed == INT_MAX) {
			cout << "Couldn't find seed that generated this corruption sequence." << endl;
			return EXIT_FAILURE;
		}

		cout << "Seed found: " << seed << endl;
	} else {
		cout << "Searching for good fountain dipping turn with seed " << seed << " and offset " << offset << endl;
		srandom(seed);
		for (uint32_t i = 0; i < offset; i++)
			random();

		if (searchWithinSeed(engraving, offsets, changes, offset, 0))
			cout << "Found sequence match with in seed at offset " << offset << endl;
		else {
			cout << "Couldn't find match for sequence" << endl;
			return EXIT_FAILURE;
		}
	}

	uint32_t engravings = 0;
	while (true) {
		int outcome = (rn2(30) + 1); // all consume
		if (outcome == 21 || outcome == 23) {
			int chance = rn2(100); // true consume
			if (chance > 90) {
				int dryup = rn2(3);
				if (dryup != 0) {
					cout << "Engrave " << engravings << " more times, then " << ((outcome == 21) ? "dip" : "quaff") << endl;
					break;
				} else {
					save_last();
				}
			}
		} else {
			rn2(8); // false consume
		}
		engravings += 1;
	}
}
