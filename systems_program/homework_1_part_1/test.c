#include <limits.h>
#include <stdio.h>
int main(void) {
    unsigned int max = UINT_MAX;
    long maxd = (long)max;
    printf("max      = %lu = 0x%x\n", sizeof(max), max);
    printf("max      = %lu = 0x%lx\n", sizeof(maxd), maxd);
    return 0;
}