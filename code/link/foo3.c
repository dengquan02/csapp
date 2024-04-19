#include <stdio.h>

void f(void);

int x = 15213;

int main() {
	printf("x = %d\n", x);
	f();
	printf("x = %d\n", x);
	return 0;
}
