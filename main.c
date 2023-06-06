#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "main.h"  

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

void atribuir_tarefas(const Instancia *instancia, Solucao *solucao) {
    int estacao_atual = 0;
    int tarefas_ordenadas[MAX_TAREFAS];

    // Armazena todos os índices das tarefas
    for (int t = 0; t < instancia->num_tarefas; t++) {
        tarefas_ordenadas[t] = t;
    }

    // Bubble Sort - ordenar decrescente
    for (int i = 0; i < instancia->num_tarefas - 1; i++) {
        for (int j = i + 1; j < instancia->num_tarefas; j++) {
            if (instancia->custo_tarefas[tarefas_ordenadas[i]] < instancia->custo_tarefas[tarefas_ordenadas[j]]) {
                int temp = tarefas_ordenadas[i];
                tarefas_ordenadas[i] = tarefas_ordenadas[j];
                tarefas_ordenadas[j] = temp;
            }
        }
    }

    // Itera sobre as tarefas ordenadas
    for (int i = 0; i < instancia->num_tarefas; i++) {
        int tarefa = tarefas_ordenadas[i];
        int estacao_valida = 1;

        // Verifica as precedências da tarefa com as tarefas anteriores
        for (int j = 0; j < instancia->num_tarefas; j++) {
            if (instancia->precedencias[tarefa][j]) {
                // Verifica se a tarefa possui precedência com alguma tarefa anterior não atribuída à mesma estação
                if (solucao->estacao[j] != -1 && solucao->estacao[j] != estacao_atual) {
                    estacao_valida = 0;
                    break;
                }
            }
        }

        // Atribui a tarefa à estação válida
        if (estacao_valida) {
            solucao->estacao[tarefa] = estacao_atual;
            solucao->tempo_conclusao[tarefa] = solucao->tempo_conclusao[tarefa] + instancia->custo_tarefas[tarefa];
            estacao_atual = (estacao_atual + 1) % instancia->num_estacoes;
        }
    }
}

int calcular_makespan(const Instancia *instancia, const Solucao *solucao) {
    int tempos_estacoes[MAX_ESTACOES] = {0};
    int makespan = 0;

    for (int t = 0; t < instancia->num_tarefas; t++) {
        int estacao_tarefa = solucao->estacao[t];
        int custo_tarefa = instancia->custo_tarefas[t];
        int tempo_estacao = tempos_estacoes[estacao_tarefa];
        int tempo_conclusao = tempo_estacao + custo_tarefa;

        // Verifica as precedências da tarefa com as tarefas anteriores na mesma estação
        for (int j = 0; j < instancia->num_tarefas; j++) {
            if (solucao->estacao[j] == estacao_tarefa && instancia->precedencias[j][t]) {
                int tempo_predecessora = tempos_estacoes[estacao_tarefa] + instancia->custo_tarefas[j];
                if (tempo_predecessora > tempo_estacao) {
                    tempo_estacao = tempo_predecessora;
                }
            }
        }

        // Verifica se o tempo de conclusão da tarefa é maior que o makespan atual
        if (tempo_conclusao > makespan) {
            makespan = tempo_conclusao;
        }

        // Atualiza o tempo da estação após a execução da tarefa
        tempos_estacoes[estacao_tarefa] = tempo_conclusao;
    }

    return makespan;
}

void resolver_instancia(const Instancia *instancia, Solucao *solucao) {
    solucao->makespan = 0;

    atribuir_tarefas(instancia, solucao);

    solucao->makespan = calcular_makespan(instancia, solucao);
}

void imprimir_solucao(const Instancia *instancia, const Solucao *solucao) {
    printf("Solucao Final:\n");
    for (int m = 0; m < instancia->num_estacoes; m++) {
        printf("Maquina %d: ", m + 1);

        for (int t = 0; t < instancia->num_tarefas; t++) {
            if (solucao->estacao[t] == m) {
                printf("%d,", t + 1);
            }
        }

        printf("\n");
    }
    printf("FO Inicial: %d\n", solucao->makespan);
    //printf("FO Final: %d\n", calcular_makespan(instancia, solucao));
}

void escrever_solucao(const Instancia *instancia, const Solucao *solucao, const char *nome_arquivo_saida) {
    FILE *arquivo = fopen(nome_arquivo_saida, "a");  // Abre o arquivo no modo de apêndice (append)

    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de saída.\n");
        return;
    }

    fprintf(arquivo, "Solução Final para %d Máquinas:\n", instancia->num_estacoes);

    for (int m = 0; m < instancia->num_estacoes; m++) {
        fprintf(arquivo, "Máquina %d: ", m + 1);

        for (int t = 0; t < instancia->num_tarefas; t++) {
            if (solucao->estacao[t] == m) {
                fprintf(arquivo, "%d,", t + 1);
            }
        }

        fprintf(arquivo, "\n");
    }

    fprintf(arquivo, "FO Inicial: %d\n", solucao->makespan);

    int fo_final = calcular_makespan(instancia, solucao);
    fprintf(arquivo, "FO Final: %d\n\n", fo_final);

    fclose(arquivo);
}

double calcular_tempo_execucao(clock_t inicio, clock_t fim) {
    return (double)(fim - inicio) / CLOCKS_PER_SEC;
}

int main() {
    Instancia instancia;
    Solucao solucao;

    const char *nome_arquivo = "KILBRID.IN2";  
    ler_instancia(nome_arquivo, &instancia);

    clock_t inicio = clock();

    for (int num_maquinas = 3; num_maquinas <= 11; num_maquinas++) {
        printf("estacao: %d\n", num_maquinas);

        // Definir o número de estacoes
        instancia.num_estacoes = num_maquinas;

        // Resolver a instância
        resolver_instancia(&instancia, &solucao);

        // Imprimir a solução
        imprimir_solucao(&instancia, &solucao);

        // Escrever a solução em arquivo
        char nome_arquivo_saida[20];
        sprintf(nome_arquivo_saida, "solucao_%d.txt", num_maquinas);
        escrever_solucao(&instancia, &solucao, nome_arquivo_saida);

        printf("\n");
    }

    clock_t fim = clock();
    double tempo_execucao = calcular_tempo_execucao(inicio, fim);

    printf("Tempo de execução: %.2f segundos\n", tempo_execucao);

    return 0;
}