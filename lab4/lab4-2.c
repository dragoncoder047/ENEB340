#include <stdio.h>
#include <stdlib.h>
void pb(unsigned long long n) {
    unsigned long long p = 1;
    while ((p << 1) <= n) p <<= 1;
    while (p) {
        putchar(p & n ? '1' : '0');
        p >>= 1;
    }
    putchar('\n');
}
int main(int argc, char** argv) {
    pb(strtoull(argv[1], NULL, 10));
    return 0;
}
