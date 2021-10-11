#include<stdio.h>
void baz();
int arr[100];
int l;
int bar(int i) {
   int j;
   for (j=0;j<i;j++) {
   	arr[j] = 0;
   }
   l += 1;
   baz();
   return arr[i-1];
}

void foo(int i) {
  if (i < 0) {
  	baz();
  } else {
  	bar(i);
  }

}

void baz() {
   if (l < 0) {
     printf("This should not be skipped\n");
   } else {
     bar(l);
   }
}

int main(int argc, char **argv) {
   int h;
   if (argc > 2) {
      bar(argc);
   } else {
     printf("This should be skipped too\n");
     return -1;
   }
   scanf("%d", &h);
   foo(h);
   return 0;
}
