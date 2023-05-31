#include <stdio.h>
#include <stdlib.h>

#define MAX_TAREFAS 100
#define MAX_ESTACOES 100
#define MAX_CUSTO 1000000

typedef struct {
    int num_tarefas;
    int num_estacoes;
    int custo_tarefas[MAX_TAREFAS];
    int precedencias[MAX_TAREFAS][MAX_TAREFAS];
} Instancia;

typedef struct {
    int estacao[MAX_TAREFAS];
    int makespan;
} Solucao;

// Função para ler a instância de um arquivo
void ler_instancia(const char *nome_arquivo, Instancia *instancia) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        exit(1);
    }

    fscanf(arquivo, "%d", &instancia->num_tarefas);

    for (int i = 0; i < instancia->num_tarefas; i++) {
        fscanf(arquivo, "%d", &instancia->custo_tarefas[i]);
    }

    for (int i = 0; i < instancia->num_tarefas; i++) {
        for (int j = 0; j < instancia->num_tarefas; j++) {
            fscanf(arquivo, "%d", &instancia->precedencias[i][j]);
        }
    }

    fclose(arquivo);
}

// Função para imprimir a solução
void imprimir_solucao(const Instancia *instancia, const Solucao *solucao) {
    printf("Solução Inicial:\n");
    for (int t = 0; t < instancia->num_tarefas; t++) {
        printf("Tarefa %d: Estação %d\n", t + 1, solucao->estacao[t] + 1);
    }
    printf("Makespan: %d\n", solucao->makespan);
    printf("\n");

    printf("Solução Final:\n");
    for (int t = 0; t < instancia->num_tarefas; t++) {
        printf("Tarefa %d: Estação %d\n", t + 1, solucao->estacao[t] + 1);
    }
    printf("Makespan Final: %d\n", solucao->makespan);
}

// Função para calcular o makespan de uma solução
void calcular_makespan(const Instancia *instancia, const Solucao *solucao, int *makespan) {
    int tempos_estacoes[MAX_ESTACOES] = {0};

    for (int t = 0; t < instancia->num_tarefas; t++) {
        int estacao_tarefa = solucao->estacao[t];
        int custo_tarefa = instancia->custo_tarefas[t];
        int tempo_estacao = tempos_estacoes[estacao_tarefa];

        if (tempo_estacao > *makespan) {
            *makespan = tempo_estacao;
        }

        tempos_estacoes[estacao_tarefa] += custo_tarefa;
    }
}

// Função para resolver a instância
void resolver_instancia(const Instancia *instancia, Solucao *solucao) {
    solucao->makespan = 0;

    // Lógica para atribuir as tarefas às estações (solução inicial)
    for (int t = 0; t < instancia->num_tarefas; t++) {
        solucao->estacao[t] = t % instancia->num_estacoes;
    }

    calcular_makespan(instancia, solucao, &solucao->makespan);

    // Implementar lógica de melhoria da solução aqui

    calcular_makespan(instancia, solucao, &solucao->makespan);
}

// Função para escrever a solução em um arquivo
void escrever_solucao(const char *nome_arquivo, const Instancia *instancia, const Solucao *solucao) {
    FILE *arquivo = fopen(nome_arquivo, "w");
    if (arquivo == NULL) {
        printf("Erro ao criar o arquivo de saída.\n");
        return;
    }

    fprintf(arquivo, "Solução Inicial:\n");
    for (int t = 0; t < instancia->num_tarefas; t++) {
        fprintf(arquivo, "Tarefa %d: Estação %d\n", t + 1, solucao->estacao[t] + 1);
    }
    fprintf(arquivo, "Makespan: %d\n", solucao->makespan);
    fprintf(arquivo, "\n");

    fprintf(arquivo, "Solução Final:\n");
    for (int t = 0; t < instancia->num_tarefas; t++) {
        fprintf(arquivo, "Tarefa %d: Estação %d\n", t + 1, solucao->estacao[t] + 1);
    }
    fprintf(arquivo, "Makespan Final: %d\n", solucao->makespan);

    fclose(arquivo);
}

int main() {
    Instancia instancia;
    Solucao solucao;

    const char *nome_arquivo = "KILBRID.IN2";
    ler_instancia(nome_arquivo, &instancia);

    printf("Informe o número de MAQUINAS (entre 3 e 11): ");
    scanf("%d", &instancia.num_estacoes);

    if (instancia.num_estacoes < 3 || instancia.num_estacoes > 11) {
        printf("Número de estações inválido. O valor deve estar entre 3 e 11.\n", instancia.num_tarefas);
        return 1;
    }

    resolver_instancia(&instancia, &solucao);

    imprimir_solucao(&instancia, &solucao);
    escrever_solucao("solucao.txt", &instancia, &solucao);

    return 0;
}

