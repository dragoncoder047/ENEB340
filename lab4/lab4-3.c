#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_SAMPLE_COUNT 10
#define VALID_LOW 18000
#define VALID_HIGH 30000
#define BAD_VAL LONG_MIN

typedef enum { NORMAL, WARN, FAIL } sim_mode;

long ok_readings[] = { 24000, 24750, 23500, 25250, 23000, 24500, 23750, 25000 };
#define len(x) (sizeof(x) / sizeof(x[0]))
long get_reading_for(int x, sim_mode mode) {
    return mode == WARN && !(x % 4) ? 31500 : mode == FAIL && !(x % 5) ? BAD_VAL : ok_readings[(unsigned)x % len(ok_readings)];
}

#define streqi(x, y) (!strcasecmp(x, y))
#define min(x, y) ((x) < (y) ? (x) : (y))
#define max(x, y) ((x) > (y) ? (x) : (y))

double to_c(long reading) { return (double)reading / 1000.; }

sim_mode get_sim_mode(char* arg) {
    if (streqi(arg, "normal")) return NORMAL;
    if (streqi(arg, "warning")) return WARN;
    if (streqi(arg, "failure")) return FAIL;
    errno = EINVAL;
    return FAIL;
}

int get_count(char* arg) {
    char* end = NULL;
    int count = (int)strtol(arg, &end, 0);
    if (end - arg != (signed)strlen(arg)) {
        errno = EINVAL;
    }
    return count;
}

void usage(char* p) {
    fprintf(stderr, "usage: %s MODE [COUNT]\n", p);
    fprintf(stderr, "  where MODE = 'normal', 'warning', or 'failure'\n");
}

int main(int argc, char** argv) {
    if (argc != 3 && argc != 2) {
        usage(argv[0]);
        return 1;
    }
    sim_mode mode = get_sim_mode(argv[1]);
    if (errno) {
        perror("mode");
        usage(argv[0]);
        return 2;
    }

    int count = DEFAULT_SAMPLE_COUNT;
    if (argc == 3) {
        count = get_count(argv[2]);
        if (errno) {
            perror("count");
            usage(argv[0]);
            return 2;
        }
    }

    int good = 0, bad = 0, fails = 0;
    long min = INT_MAX, max = INT_MIN, sum = 0;
    for (int i = 0; i < count; i++) {
        long reading = get_reading_for(i, mode);
        if (reading == BAD_VAL) { fails++; printf("sample=%03i temp=???????? status=FAIL\n", i); continue; }
        if (reading >= VALID_LOW && reading <= VALID_HIGH) { good++; printf("sample=%03i temp=%.3f C status=OK\n", i, to_c(reading)); }
        else { bad++; printf("sample=%03i temp=%.3f C status=WARN\n", i, to_c(reading)); }
        min = min(min, reading);
        max = max(max, reading);
        sum += reading;
    }
    int valid = good + bad;
    printf("summary samples=%i valid=%i ok=%i warning=%i failure=%i\ntemperature min=%.3f C max=%.3f C average=%f C\n",
                    count,     valid,   good, bad,       fails,               to_c(min), to_c(max), to_c(sum) / valid);

    return 0;
}
