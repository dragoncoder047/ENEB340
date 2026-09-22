#include <stdio.h>
#include <stdlib.h>
#ifndef KW
#define KW register
#endif
int main(int argc, char** argv) {
    KW unsigned long long x = strtoull(argv[1], NULL, 10);
    KW long double f = 1.;
    while (x > 1) {
        f *= (long double)x;
        x--;
    }
    printf("%Lf\n", f);
    return 0;
}
