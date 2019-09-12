#include <stdio.h>
#include <stdlib.h>

int main() {
    setenv("MYENV", "Hello World", 0);
    printf("%s\n", getenv("MYENV"));
}