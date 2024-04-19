/* 
 * name: dq
 * time: 2023/9/1
 */

#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "cachelab.h"

struct cache_line {
    int valid;
    int tag;
    int time_stamp; // 最近访问的计数
} *cache;

int count = 0;
int vflag = 0, s = 0, E = 0, b = 0;
char *t = NULL;
int hit_count = 0, miss_count = 0, eviction_count = 0;

void memory_access(long long address) {
    count ++;
    int set_index = (address >> b) & ((1 << s) - 1);
    int tag = address >> (s + b);
    
    int miss = 1, load = 0, lru = -1;
    for (int i = set_index * E; i < (set_index + 1) * E; i ++) {
        if (cache[i].valid && cache[i].tag == tag) {
            miss = 0;
            cache[i].time_stamp = count;
            break;
        }
        if (! cache[i].valid) {
            cache[i].valid = 1;
            cache[i].tag = tag;
            cache[i].time_stamp = count;
            load = 1;
            break;
        }
        if (lru == -1 || cache[lru].time_stamp > cache[i].time_stamp) {
            lru = i;
        }
    }
    if (miss) {
        miss_count ++;
        if (vflag) {
            printf(" miss");
        }
        if (! load) {
            cache[lru].valid = 1;
            cache[lru].tag = tag;
            cache[lru].time_stamp = count;
            eviction_count ++;
            if (vflag) {
                printf(" eviction");
            }
        }
    }
    else {
        hit_count ++;
        if (vflag) {
            printf(" hit");
        }
    }
}

int main(int argc, char *argv[])
{
    // 1. 处理命令行参数
    const char *str = "Usage: ./csim [-hv] -s <num> -E <num> -b <num> -t <file>\n"
        "Options:\n  -h         Print this help message.\n"
        "  -v         Optional verbose flag.\n"
        "  -s <num>   Number of set index bits.\n"
        "  -E <num>   Number of lines per set.\n"
        "  -b <num>   Number of block offset bits.\n"
        "  -t <file>  Trace file.\n\n"
        "Examples:\n"
        "  linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace\n"
        "  linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace\n";
    int opt;
    while ((opt = getopt(argc, argv, "h::v::s:E:b:t:")) != -1) {
        switch (opt) {
            case 'h':
                printf("%s", str);
                return 0;
            case 'v':
                vflag = 1;
                break;
            case 's':
                s = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                t = optarg;
                break;
            default: /* '?' */
                fprintf(stderr, "Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    // printf("vflag=%d; s=%d; E=%d; b=%d; t=%s; optind=%d\n",
    //         vflag, s, E, b, t, optind);
    if (!s || !E || !b || !t) {
        fprintf(stderr, "argument fault\n");
        exit(EXIT_FAILURE);
    }

    // 2. 进行cache存储
    cache = calloc(E << s, sizeof(*cache));
    if (cache == NULL) {
        // 分配错误
        fprintf(stderr, "malloc fault\n");
        exit(EXIT_FAILURE);
    }
    memset(cache, 0, (E << s) * sizeof(*cache));

    // 3. 从文件中读入内容并处理
    FILE *my_stream;
    my_stream = fopen(t, "r");
    char op;
    long long address;
    int size;
    while (fscanf(my_stream, " %c %llx,%d\n", &op, &address, &size) != EOF) {
        if (op == 'I') {
            continue;
        }
        if (vflag) {
            printf("%c %llx,%d", op, address, size);
        }
        memory_access(address);
        if (op == 'M') { // Modify操作：a data load followed by a data store
            memory_access(address);
        }
        if (vflag) {
            printf("\n");
        }
    }

    fclose (my_stream);
    free(cache);
    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}
