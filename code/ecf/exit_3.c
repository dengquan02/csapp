/* $begin exit_3 */
#include "csapp.h"

int main(int argc, char* argv[]) {
    Kill(getpid(), SIGKILL);
}
/* $end exit_3 */