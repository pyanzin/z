#include <stdlib.h>

void* space;
unsigned INIT_SIZE = 0x1000;

void init() {
    space = malloc(INIT_SIZE);
}