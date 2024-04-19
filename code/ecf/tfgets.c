/* $begin tfgets */
#include "csapp.h"
#define LEN 100

jmp_buf buf;

void sigalrm_handler()
{
    siglongjmp(buf, 1);
}

char* tfgets(char* s, int size, FILE* stream)
{
    if (!sigsetjmp(buf, 1)) {
        Signal(SIGALRM, sigalrm_handler);
        alarm(5);
        return fgets(s, size, stream);
    }
    else
        return NULL;
}

int main() 
{
    char s[LEN];
    char *input = tfgets(s, LEN, stdin);

    if (input)
        printf("%s", input);
    else
        fprintf(stderr, "Time out! Nothing input\n");

    return 0;
}
/* $end tfgets */