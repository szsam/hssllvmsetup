#include<stdio.h>
int main(int argc, char **argv) {
	int n, i;
	int j;
	scanf("%d", &n);
	j = 0;
	for (i=0; i < n; i++) {
		printf("Loop Iteration:%d\n", i);
		j += i;
	}
	return j;
}
