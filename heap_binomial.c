#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAX_CONSULTORIOS 10
#define MIN_CONSULTORIOS 3

typedef struct No {
    int grau;
    int chave;
    struct No* pai;
    struct No* filho;
    struct No* irmao;
} No;

typedef struct heap_binomial {
    struct No* cabeca;
} Heap;

typedef struct Consultorio {
    Heap* fila_prioritaria;
    Heap* fila_nao_prioritaria;
} Consultorio;

Consultorio consultorios[MAX_CONSULTORIOS];
int num_consultorios = MIN_CONSULTORIOS;
FILE* historico;

No* criarNo(int chave) {
    No* novo_no = (No*) malloc(sizeof(No));
    novo_no->chave = chave;
    novo_no->grau = 0;
    novo_no->filho = NULL;
    novo_no->pai = NULL;
    novo_no->irmao = NULL;
    return novo_no;
}

No* combina(No* n1, No* n2) {
    if (n1->chave > n2->chave) {
        No* aux = n1;
        n1 = n2;
        n2 = aux;
    }
    n2->pai = n1;
    n2->irmao = n1->filho;
    n1->filho = n2;
    n1->grau++;
    return n1;
}

Heap* uniao(Heap* h1, Heap* h2) {
    Heap* nova_heap = (Heap*) malloc(sizeof(Heap));
    nova_heap->cabeca = NULL;

    if (h1->cabeca == NULL) return h2;
    if (h2->cabeca == NULL) return h1;

    No* n1 = h1->cabeca;
    No* n2 = h2->cabeca;
    No* atual = NULL;
    No* prox = NULL;

    while (n1 != NULL && n2 != NULL) {
        if (n1->grau <= n2->grau) {
            prox = n1;
            n1 = n1->irmao;
        } else {
            prox = n2;
            n2 = n2->irmao;
        }
        if (atual == NULL) {
            atual = prox;
            nova_heap->cabeca = atual;
        } else {
            atual->irmao = prox;
            atual = prox;
        }
    }

    if (n1 != NULL) atual->irmao = n1;
    if (n2 != NULL) atual->irmao = n2;

    atual = nova_heap->cabeca;
    No* ant = NULL;
    prox = atual->irmao;

    while (prox != NULL) {
        if (atual->grau != prox->grau || (prox->irmao != NULL && prox->irmao->grau == atual->grau)) {
            ant = atual;
            atual = prox;
        } else {
            if (atual->chave <= prox->chave) {
                atual->irmao = prox->irmao;
                atual = combina(atual, prox);
            } else {
                if (ant == NULL) {
                    nova_heap->cabeca = prox;
                } else {
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

void insert(Heap* heap, int chave) {
    Heap* h = (Heap*) malloc(sizeof(Heap));
    h->cabeca = criarNo(chave);
    heap->cabeca = uniao(heap, h)->cabeca;
}

int extractMin(Heap* heap) {
    if (heap->cabeca == NULL) return -1;

    No* menorNo = heap->cabeca;
    No* anteriorAomenor = NULL;
    No* atual = heap->cabeca;
    No* ant = NULL;

    int menorValor = menorNo->chave;

    while (atual != NULL) {
        if (atual->chave < menorValor) {
            menorValor = atual->chave;
            anteriorAomenor = ant;
            menorNo = atual;
        }
        ant = atual;
        atual = atual->irmao;
    }

    if (anteriorAomenor != NULL) {
        anteriorAomenor->irmao = menorNo->irmao;
    } else {
        heap->cabeca = menorNo->irmao;
    }

    Heap* nova_heap = (Heap*) malloc(sizeof(Heap));
    nova_heap->cabeca = NULL;
    No* n = menorNo->filho;
    while (n != NULL) {
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

void inicializaConsultorios() {
    for (int i = 0; i < MAX_CONSULTORIOS; i++) {
        consultorios[i].fila_prioritaria = (Heap*) malloc(sizeof(Heap));
        consultorios[i].fila_nao_prioritaria = (Heap*) malloc(sizeof(Heap));
        consultorios[i].fila_prioritaria->cabeca = NULL;
        consultorios[i].fila_nao_prioritaria->cabeca = NULL;
    }
}

void exibirEstadoConsultorios() {
    printf("\nEstado Atual dos Consultorios:\n");
    for (int i = 0; i < num_consultorios; i++) {
        printf("Consultório %d:\n", i + 1);
        
        if (consultorios[i].fila_prioritaria->cabeca != NULL) {
            printf("  Proximo na fila prioritaria: %d\n", consultorios[i].fila_prioritaria->cabeca->chave);
        } else {
            printf("  Nenhum paciente na fila prioritaria\n");
        }

        if (consultorios[i].fila_nao_prioritaria->cabeca != NULL) {
            printf("  Proximo na fila nao prioritaria: %d\n", consultorios[i].fila_nao_prioritaria->cabeca->chave);
        } else {
            printf("  Nenhum paciente na fila não prioritaria\n");
        }
    }
}

void adicionarPaciente(int chave, int prioridade, int consultorio) {
    if (consultorio < 1 || consultorio > num_consultorios) {
        printf("Consultorio invalido!\n");
        return;
    }

    if (prioridade) {
        insert(consultorios[consultorio - 1].fila_prioritaria, chave);
        printf("Paciente prioritario %d adicionado a fila do consultorio %d.\n", chave, consultorio);
        // Registro no arquivo de histórico para paciente prioritário
        fprintf(historico, "INC P %d - %d\n", chave, consultorio);
    } else {
        insert(consultorios[consultorio - 1].fila_nao_prioritaria, chave);
        printf("Paciente nao prioritario %d adicionado a fila do consultorio %d.\n", chave, consultorio);
        // Registro no arquivo de histórico para paciente não prioritário
        fprintf(historico, "INC N %d - %d\n", chave, consultorio);
    }
}

void atenderPaciente(int consultorio) {
    if (consultorio < 1 || consultorio > num_consultorios) {
        printf("Consultorio invalido!\n");
        return;
    }

    int paciente = -1;
    if (consultorios[consultorio - 1].fila_prioritaria->cabeca != NULL) {
        paciente = extractMin(consultorios[consultorio - 1].fila_prioritaria);
    } else if (consultorios[consultorio - 1].fila_nao_prioritaria->cabeca != NULL) {
        paciente = extractMin(consultorios[consultorio - 1].fila_nao_prioritaria);
    }

    if (paciente != -1) {
        printf("Consultorio %d atendeu o paciente: %d\n", consultorio, paciente);
        fprintf(historico, "ATENDIMENTO %d - %d\n", paciente, consultorio);
    } else {
        printf("Nenhum paciente na fila do consultorio %d\n", consultorio);
    }
}

void abrirConsultorio() {
    if (num_consultorios < MAX_CONSULTORIOS) {
        num_consultorios++;
        printf("Consultorio %d aberto!\n", num_consultorios);
        fprintf(historico, "ABERTO CONSULTORIO %d\n", num_consultorios);
    } else {
        printf("Numero maximo de consultorio atingido!\n");
    }
}

void fecharConsultorio() {
    if (num_consultorios > MIN_CONSULTORIOS) {
        printf("Consultorio fechado!\n");
        fprintf(historico, "FECHADO CONSULTORIO \n");
        
        // Realocação dos pacientes para outro consultório
        for (int i = 0; i < num_consultorios; i++) {
            if (i != num_consultorios - 1) {
                Heap* filaP = consultorios[num_consultorios - 1].fila_prioritaria;
                Heap* filaNP = consultorios[num_consultorios - 1].fila_nao_prioritaria;
                
                if (filaP->cabeca != NULL) {
                    consultorios[i].fila_prioritaria->cabeca = uniao(consultorios[i].fila_prioritaria, filaP)->cabeca;
                }
                if (filaNP->cabeca != NULL) {
                    consultorios[i].fila_nao_prioritaria->cabeca = uniao(consultorios[i].fila_nao_prioritaria, filaNP)->cabeca;
                }
                break;
            }
        }
        num_consultorios--;
    } else {
        printf("Nao e possivel fechar mais consultorios!\n");
    }
}
