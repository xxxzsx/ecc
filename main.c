#include <stdio.h>
#include <stddef.h>
#include <inttypes.h>

#define DIST   23         // Hamming distance between codewords
#define LENGTH 1024       // Size of the codeword basis to find

// Print a set of results.
void vshow(int i, uint64_t x, int dist) {
    uint64_t j;
    for (j = 0; (((uint64_t)1) << j) < x && j < 63; j++);
    printf("(%2d,%2d,%2d)  ", (int)(i + 1 + j), i + 1, dist);
    printf("0x%.8"PRIu32"%.8"PRIu32"  ", (uint32_t)(x >> 32), (uint32_t)x);
    for (j = ((uint64_t)1) << 63; j; j >>= 1)
        printf("%d", (x & j) ? 1 : 0);
    printf("\n");
}

int main() {
    uint64_t a[LENGTH];
    int i = find(a, DIST, LENGTH);
    vshow(i, a[i], DIST);
}