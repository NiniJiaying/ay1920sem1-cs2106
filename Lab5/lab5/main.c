#include "my_stdio.h"
#include <stdio.h>

#define BUF_SIZE 10240

int main() {
    MY_FILE *f = my_fopen("test.txt", "r");
    char* buf = malloc(BUF_SIZE * sizeof(char));
    printf("%d\n", my_fread(buf, sizeof(char), 9000, f));
    printf("%s\n", buf);
}

// test write
// MY_FILE *f = my_fopen("test.txt", "w+");
// char* buf = malloc(BUF_SIZE * sizeof(char));
// char* wbuf = "write";
// int t;
// scanf("%d", &t);
// for (int i=0;i<t;i++) {
//     buf[i] = 'a';
// }
// printf("%d\n",my_fwrite(buf, sizeof(char), t, f));
// my_fflush(f);

// test read
// MY_FILE *f = my_fopen("test.txt", "r");
// char* buf = malloc(BUF_SIZE * sizeof(char));
// int t;
// scanf("%d",&t);
// printf("%d\n", my_fread(buf, sizeof(char), t, f));
// printf("%s\n", buf);