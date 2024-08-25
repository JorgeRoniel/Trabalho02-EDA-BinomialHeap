#include <stdio.h>
#include <stdlib.h>
#include "heap_binomial.c"  // Incluindo a lógica da heap binomial e operações do consultório

int main() {
    int opcao, consultorio, prioridade, chave;

    inicializaConsultorios();  // Inicializa os consultórios e filas
    historico = fopen("output.txt", "w");  // Arquivo de histórico

    if (historico == NULL) {
        printf("Erro ao abrir o arquivo de historico.\n");
        return 1;
    }

    while (1) {
        printf("\n=== MENU PRINCIPAL ===\n");
        printf("1. Chegada de novo paciente\n");
        printf("2. Atendimento de paciente\n");
        printf("3. Abrir novo consultorio\n");
        printf("4. Fechar consultorio\n");
        printf("5. Exibir estado dos consultorio\n");
        printf("6. Sair\n");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                printf("Informe o numero do consultorio (1-%d): ", num_consultorios);
                scanf("%d", &consultorio);
                printf("Informe a prioridade (1 para prioritario, 0 para nao prioritario): ");
                scanf("%d", &prioridade);
                printf("Informe o numero do paciente: ");
                scanf("%d", &chave);
                adicionarPaciente(chave, prioridade, consultorio);
                break;
            case 2:
                printf("Informe o numero do consultorio para atendimento (1-%d): ", num_consultorios);
                scanf("%d", &consultorio);
                atenderPaciente(consultorio);
                break;
            case 3:
                abrirConsultorio();
                break;
            case 4:
                fecharConsultorio();
                break;
            case 5:
                exibirEstadoConsultorios();
                break;
            case 6:
                fclose(historico);  // Fecha o arquivo de histórico
                printf("Saindo...\n");
                return 0;
            default:
                printf("Opcao invalida. Tente novamente.\n");
                break;
        }
    }
}
