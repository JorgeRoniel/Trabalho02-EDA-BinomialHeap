#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef struct No{
    int grau;
    int chave;

    struct No* pai;
    struct No* filho;
    struct No* irmao;

} No;

typedef struct heap_binomial{
    struct No* cabeca;
} Heap;

No* criarNo(int chave){
    No* novo_no = (No*) malloc(sizeof(No));
    novo_no->chave = chave;
    novo_no->grau = 0;
    
    novo_no->filho = NULL;
    novo_no->pai = NULL;
    novo_no->irmao = NULL;

    return novo_no;
}

No* combina(No* n1, No* n2){
    if(n1->chave > n2->chave){
        No* aux = n1;
        n1 = n2;
        n2 = aux;
    }

    n2->pai = n1;
    n2->irmao = n1->filho;
    n1->filho = n2;
    n1->grau = n1->grau+1;

    return n1;
}

Heap* uniao(Heap* h1, Heap* h2){
    Heap* nova_heap = (Heap*) malloc(sizeof(Heap));
    nova_heap->cabeca = NULL;

    if(h1->cabeca == NULL) return h2;
    if(h2->cabeca == NULL) return h1;

    No* n1 = h1->cabeca;
    No* n2 = h2->cabeca;

    No* ant = NULL;
    No* atual = NULL;
    No* prox = NULL;

    while(n1 != NULL && n2 != NULL){
        if(n1->grau <= n2->grau){
            prox = n1;
            n1 = n1->irmao;
        }else{
            prox = n2;
            n2 = n2->irmao;
        }
        if(atual == NULL){
            atual = prox;
            nova_heap->cabeca = atual;
        }else{
            atual->irmao = prox;
            atual = prox;
        }
    }

    if(n1 != NULL) atual->irmao = n1;
    if(n2 != NULL) atual->irmao = n2;

    atual = nova_heap->cabeca;
    ant = NULL;
    prox = atual->irmao;

    while(prox != NULL){
        if(atual->grau != prox->grau || (prox->irmao != NULL && prox->irmao->grau == atual->grau)){
            ant = atual;
            atual = prox;
        }else{
            if(atual->chave <= prox->chave){
                atual->irmao = prox->irmao;
                atual = combina(atual, prox);
            }else{
                if(ant == NULL){
                    nova_heap->cabeca = prox;
                }else{
                    ant->irmao = prox;
                }

                prox = combina(prox, atual);
                atual = prox;
            }
        }
        prox = atual->irmao;
    }

    return nova_heap;
}

void insert(Heap* heap, int chave){
    Heap* h = (Heap*) malloc(sizeof(Heap));
    h->cabeca = criarNo(chave);
    heap->cabeca = uniao(heap, h)->cabeca;
}

int extractMin(Heap* heap){
    if(heap->cabeca == NULL) return -1;

    No* menorNo = heap->cabeca;
    No* anteriorAomenor = NULL;
    No* atual = heap->cabeca;
    No* ant = NULL;

    int menorValor = menorNo->chave;

    while(atual != NULL){
        if(atual->chave < menorValor){
            menorValor = atual->chave;
            anteriorAomenor = ant;
            menorNo = atual;
        }
        ant = atual;
        atual = atual->irmao;
    }

    if(anteriorAomenor != NULL){
        anteriorAomenor->irmao = menorNo->irmao;
    }else{
        heap->cabeca = menorNo->irmao;
    }

    Heap* nova_heap = (Heap*) malloc(sizeof(Heap));
    nova_heap->cabeca = NULL;
    No* n = menorNo->filho;
    while(n != NULL){
        No* prox = n->irmao;
        n->irmao = nova_heap->cabeca;
        nova_heap->cabeca = n;
        n->pai = NULL;
        n = prox;
    }

    heap->cabeca = uniao(heap, nova_heap)->cabeca;

    free(menorNo);
    return menorValor;
}