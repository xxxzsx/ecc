// Error Correction Codes.

#pragma once

#include <stddef.h>
#include <inttypes.h>
#include <stdbool.h>

static uint64_t last = 0;
static uint64_t last_cnt = 1;


// Count how many bits are set in a uint64_t.
// Filched from sci.crypt. 64 1-bit counters, 32 2-bit counters, etc.
int count(uint64_t x) {
    uint32_t c;

    x = (x & 0x5555555555555555LL) + ((x >> 1) & 0x5555555555555555LL);
    x = (x & 0x3333333333333333LL) + ((x >> 2) & 0x3333333333333333LL);
    c = ((uint32_t)x) + ((uint32_t)(x >> 32));
    c = (c & 0x0f0f0f0f) + ((c >> 4)  & 0x0f0f0f0f);
    c = (c & 0x00ff00ff) + ((c >> 8)  & 0x00ff00ff);
    c = (c & 0x0000ffff) + ((c >> 16) & 0x0000ffff);
    return (int)c;
}

/**
 * Test if a[len] has Hamming distance at least dist from t.
 * Also recursively test c with more powers of two XORed to t.
 * Return false if c fails this trial or any recursive trial.
 * Otherwise c passes, return true.
 * 
 * Properties:
 *   uint64_t* a  - array of codewords for powers of two
 *   int len      - length of array
 *   int dist     - required Hamming distance
 *   uint64_t t   - trial vector to compare codeword to
 *   int pos      - next position to choose
 *   int num      - number of positions still to be chosen
 *   int cnt      - final number of chosen positions total
 */
int test(uint64_t* a, int len, int dist, uint64_t t, int pos, int num, int cnt) {
    int i;

    if (num) {
        for (i = pos; i < len; i++) {
            t ^= a[i];
            if (!test(a, len, dist, t, i + 1, num - 1, cnt))
                return false;
            t ^= a[i];
        }
    } else {
        for (i = pos; i < len; i++) {
            t ^= a[i];
            if (1 + cnt + count(a[len] ^ t) < dist) {
                last = t;
                last_cnt = 1 + cnt;
                return false;
            }
            t ^= a[i];
        }
    }

    return true;
}

// Find values that all differ by at least limit bits.
void find(uint64_t *a, int dist, int length) {
    int i, k;
    uint64_t j;
    uint64_t trial;

    for (j = 0, i = 0; i < length; i++) {
        for (; ; j++) {
            if (last_cnt + count(j ^ last) < dist)
                continue;

            if (count(j) + 1 < dist) {
                last = (uint64_t)0;
                last_cnt = 1;
                continue;
            }

            a[i] = j;
            for (k = 0; k < dist - 2; k++)
                if (!test(a, i, dist, (uint64_t)0, 0, k, k + 1))
                    continue;

            break;
        }
    }
}