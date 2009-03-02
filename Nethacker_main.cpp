/*
 * nhwisher.cpp
 *
 *  Created on: Feb 27, 2009
 *      Author: kenny
 */

#include <iostream>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <getopt.h>
#include <limits.h>

//extern "C"
long int find_seed(const char *engraving, int engraving_length, const int *offsets, const int *changes, int num_changes);

using namespace std;

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

void findChange(string engraving, string corruption, uint32_t &offset, uint32_t &change) {
	for (uint32_t x = 0; x < engraving.size(); ++x)
		if (engraving[x] != corruption[x]) {
			offset = x;
			change = corruption[x] - 'a';
			cout << "Corruption: index " << offset << " changed to " << corruption[x] << endl;
		}
}

#if 0
bool searchWithinSeed(const string &engraving, const vector<uint32_t> &sequence, uint32_t &offset, const uint32_t &depth) {
	uint32_t cur_index = 0;
	vector<uint32_t> cur_sequence;

	use_last = false;

	while (true) {
		uint32_t n = rn2(engraving.size());
		unsigned char c1, c2, outcome;

		c1 = lowc(engraving[n]);
		do
			c2 = 'a' + rn2('z' - 'a'); while (c1 == c2);
		outcome = (engraving[n] == c1) ? c2 : highc(c2); /* keep same case */

		offset += 2;

		if (sequence[cur_index] == n && sequence[cur_index + 1] == outcome) {
			cur_index += 2;
			cur_sequence.push_back(n);
			cur_sequence.push_back(outcome);
			if (cur_index == sequence.size())
				break;
		} else {
			if (cur_index > 0) {
				/* try to match subsequences within the previous sequence match */
			}
			use_last = true;

			if (offset < 0 || (depth != 0 && offset >= depth))
				return false;
		}
	}

	return true;
}
#endif

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

	string engraving;
	vector<int> offsets;
	vector<int> changes;

	int next_opt;
	while ((next_opt = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
		switch (next_opt) {
		case 'h':
			print_usage();
			return EXIT_SUCCESS;
#if 0
		case 's':
			seed = atoi(optarg);
			seed_search = false;
			break;
#endif
		case 'e':
			engraving = string(optarg);
			transform(engraving.begin(), engraving.end(), engraving.begin(), ptr_fun(::tolower));
			break;
#if 0
		case 'f':
			first = atoi(optarg);
			break;
		case 'l':
			last = atoi(optarg);
			break;
#endif
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
		if (corruption.length() != engraving.length()) {
			cout << "Error: Corruption \"" << corruption << "\" is not the same length as original engraving!" << endl;
			return 1;
		}
		transform(corruption.begin(), corruption.end(), corruption.begin(), ptr_fun(::tolower));
		findChange(engraving, corruption, offset, change);

		offsets.push_back(offset);
		changes.push_back(change);
	}

	if (offsets.size() < 1) {
		print_usage();
		cout << endl << "Please use one or more corrupted engravings!" << endl;
		return EXIT_FAILURE;
	}

	long int seed = find_seed(engraving.c_str(), engraving.length(), &offsets[0], &changes[0], offsets.size());
	
	cout << "Found seed " << seed << endl;
#if 0
	if (seed_search) {
		/* search for the seed */
		uint32_t seed = first;
		uint32_t this_offset = offset;
		time_t start_time = time(0);

		while (seed < last) {
			srandom(seed);
			for (uint32_t i = 0; i < this_offset; i++)
				random();
			if (searchWithinSeed(engraving, sequence, this_offset, 1))
				break;
			this_offset = offset;
			if ((++seed % 500000) == 0) {
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

		if (searchWithinSeed(engraving, sequence, offset, 0))
			cout << "Found sequence match with in seed at offset " << offset << endl;
		else {
			cout << "Couldn't find match for sequence" << endl;
			return EXIT_FAILURE;
		}
	}

	uint32_t engravings = 0;
	while (true) {
		int outcome = (rn2(30) + 1);
		if (outcome == 21 || outcome == 23) {
			if (rn2(100) > 82) {
				cout << "Engrave " << engravings << " more times, then " << ((outcome == 21) ? "dip" : "quaff") << endl;
				break;
			} else
				use_last = true;
		}
		rn2(100);
		engravings += 1;
	}
#endif
}
