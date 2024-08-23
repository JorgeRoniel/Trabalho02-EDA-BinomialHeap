#include <stdio.h>
#include <stdlib.h>
#include "heap_binomial.c"

int main(){
    Heap* h = (Heap*) malloc(sizeof(Heap));
    h->cabeca = NULL;

    insert(h, 10);
    insert(h, 20);
    insert(h, 5);
    insert(h, 30);

    printf("MIN: %d\n", extractMin(h));
    return 0;
}