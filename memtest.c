#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef DEBUG
#define log(fmt, ...) \
    fprintf(stderr, __FILE__ ":%3d " fmt, __LINE__, ##__VA_ARGS__)
#define logstr(str) fputs(str, stderr)
#else
#define log(fmt, ...)
#define logstr(str)
#endif

#define DEFAULT_TEST_COUNT 30
#define DEFAULT_TEST_STRING \
    "Now is the time for all good people to come to the aid of their country."
#define MAX_TEST_COUNT 10000
#define MAX_TEST_STRING_LENGTH 1000000

#define ABBREVIATE_OVER_CHARS 80
#define STRING_OR_ABBREVIATE(str) \
    (strlen(str) > ABBREVIATE_OVER_CHARS ? "..." : str)

// percent
#define PROBABILITY_OF_RANDOM_FREE 50

#define PROBABILITY_OF_LARGE_ALLOCATION 10 // percent for large allocation

#define PROBABILITY_OF_REALLOCATION 50 // percent for reallocation

#define MIN(a, b) ((a) < (b) ? (a) : (b))

int rand_between(int min, int max);
void print_usage(const char *progname);

int main(int argc, char *argv[])
{
    srand(time(NULL));

    int test_count = DEFAULT_TEST_COUNT;
    char test_string[MAX_TEST_STRING_LENGTH] = DEFAULT_TEST_STRING;

    int next_arg = 1;
    if (next_arg < argc &&
        (!strcmp(argv[next_arg], "-h") || !strcmp(argv[next_arg], "--help")))
    {
        print_usage(argv[0]);
    }
    if (next_arg + 1 < argc && !strcmp(argv[next_arg], "-n"))
    {
        if (1 != sscanf(argv[next_arg + 1], "%d", &test_count) ||
            test_count < 1 || test_count > MAX_TEST_COUNT)
        {
            log("test_count must be an integer between 1 and %d\n",
                MAX_TEST_COUNT);
            exit(1);
        }
        next_arg += 2;
    }
    if (next_arg + 1 < argc && !strcmp(argv[next_arg], "-s"))
    {
        strcpy(test_string, argv[next_arg + 1]);
        next_arg += 2;
    }
    if (next_arg + 1 < argc && !strcmp(argv[next_arg], "-f"))
    {
        FILE *fp = fopen(argv[next_arg + 1], "r");
        if (fp == NULL)
        {
            log("could not open file: %s\n", argv[next_arg + 1]);
            exit(1);
        }
        size_t string_len = fread(test_string, 1, sizeof(test_string) - 1, fp);
        test_string[string_len] = '\0';
        fclose(fp);
        next_arg += 2;
    }
    if (next_arg < argc)
    {
        log("unexpected argument: %s\n", argv[next_arg]);
        print_usage(argv[0]);
    }

    size_t test_string_len = strlen(test_string);
    log("conducting %d tests\n", test_count);
    log("test string: (%lu long) '%s'\n", test_string_len,
        STRING_OR_ABBREVIATE(test_string));

    char *allocated_regions[MAX_TEST_COUNT];

    for (int test_number = 0; test_number < test_count; test_number++)
    {
        log("begin test [%3d]\n", test_number);

        int chosen_block_size = rand_between(1, test_string_len + 1);
        log("chosen_block_size: %d\n", chosen_block_size);

        if (rand_between(1, 100) < PROBABILITY_OF_LARGE_ALLOCATION)
        {
            chosen_block_size = rand_between(test_string_len, 1000000);
            log("large allocation: %d\n", chosen_block_size);
        }

        // allocated_regions[0] = buffer is of size 30
        // allocated_regions[1] = buffer is of size 82
        // allocated_regions[27] = buffer is of size 8

        allocated_regions[test_number] = malloc(chosen_block_size);
        if (allocated_regions[test_number] == NULL)
        {
            log("malloc failed\n");
            exit(1);
        }

        int len_to_copy = MIN(test_string_len, chosen_block_size - 1);

        // log("[%3d] ptrs[%3d]: %p, going to copy %d chars\n", ix, ix,
        //         ptrs[ix], len_to_copy);

        strncpy(allocated_regions[test_number], test_string, len_to_copy);
        allocated_regions[test_number][len_to_copy] = '\0';

        log("stored in [%3d]: %p '%s'\n", test_number,
            allocated_regions[test_number],
            STRING_OR_ABBREVIATE(allocated_regions[test_number]));

        if (rand_between(1, 100) < PROBABILITY_OF_REALLOCATION)
        {
            int index_to_realloc = rand_between(0, test_number);
            log("index_to_realloc: %d\n", index_to_realloc);
            if (allocated_regions[index_to_realloc])
            {
                logstr("\n");
                log("reallocating [%3d] %p '%s'\n", index_to_realloc,
                    allocated_regions[index_to_realloc],
                    STRING_OR_ABBREVIATE(allocated_regions[index_to_realloc]));

                int new_size = rand_between(1, test_string_len * 5);
                log("new size: %d\n", new_size);

                allocated_regions[index_to_realloc] = realloc(allocated_regions[index_to_realloc], new_size);
                if (allocated_regions[index_to_realloc] == NULL)
                {
                    log("realloc failed\n");
                    exit(1);
                }
                allocated_regions[index_to_realloc][new_size - 1] = '\0';
            }
        }

        if (rand_between(1, 100) < PROBABILITY_OF_RANDOM_FREE)
        {
            int index_to_free = rand_between(0, test_number);
            if (allocated_regions[index_to_free])
            {
                logstr("\n");
                log("randomly freeing [%3d] %p '%s'\n", index_to_free,
                    allocated_regions[index_to_free],
                    STRING_OR_ABBREVIATE(allocated_regions[index_to_free]));

                free(allocated_regions[index_to_free]);

                log("freed [%3d] %p\n", index_to_free,
                    allocated_regions[index_to_free]);

                allocated_regions[index_to_free] = NULL;
            }
        }
        log("end test [%3d]\n\n\n", test_number);
    }

    logstr("\n\n\nCleaning up...\n");
    for (int region_to_free = 0; region_to_free < test_count;
         region_to_free++)
    {
        if (allocated_regions[region_to_free])
        {
            log("freeing [%3d] %p '%s'\n", region_to_free,
                allocated_regions[region_to_free],
                STRING_OR_ABBREVIATE(allocated_regions[region_to_free]));

            free(allocated_regions[region_to_free]);

            log("freed   [%3d] %p\n", region_to_free,
                allocated_regions[region_to_free]);
        }
        else
        {
            log("already freed [%3d] (is NULL)\n", region_to_free);
        }
    }

    return 0;
}

void print_usage(const char *progname)
{
    printf(
        "Usage: %s [-h | --help] [-n test_count] [-s test_string] [-f <file "
        "containing test string>\n"
        "Default: %d tests performed\n"
        "Default test string: '%s'\n"
        "Maximum test count: %d\n"
        "Maximum test string length: %d\n",
        progname, DEFAULT_TEST_COUNT, DEFAULT_TEST_STRING, MAX_TEST_COUNT,
        MAX_TEST_STRING_LENGTH - 1);
    exit(0);
}

int rand_between(int min, int max)
{
    return rand() % (max - min + 1) + min;
}
