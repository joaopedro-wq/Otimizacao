#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Main.h"  


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

    for (int t = 0; t < instancia->num_tarefas; t++) {
        tarefas_ordenadas[t] = t;
    }

    for (int i = 0; i < instancia->num_tarefas - 1; i++) {
        for (int j = i + 1; j < instancia->num_tarefas; j++) {
            if (instancia->custo_tarefas[tarefas_ordenadas[i]] < instancia->custo_tarefas[tarefas_ordenadas[j]]) {
                int temp = tarefas_ordenadas[i];
                tarefas_ordenadas[i] = tarefas_ordenadas[j];
                tarefas_ordenadas[j] = temp;
            }
        }
    }

    for (int i = 0; i < instancia->num_tarefas; i++) {
        int tarefa = tarefas_ordenadas[i];
        int estacao_valida = 1;

        for (int j = 0; j < instancia->num_tarefas; j++) {
            if (instancia->precedencias[tarefa][j] && solucao->estacao[j] != estacao_atual) {
                estacao_valida = 0;
                break;
            }
        }

        if (estacao_valida) {
            solucao->estacao[tarefa] = estacao_atual;
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

        if (tempo_estacao + custo_tarefa > makespan) {
            makespan = tempo_estacao + custo_tarefa;
        }

        tempos_estacoes[estacao_tarefa] += custo_tarefa;
    }

    return makespan;
}

void resolver_instancia(const Instancia *instancia, Solucao *solucao) {
    solucao->makespan = 0;

    atribuir_tarefas(instancia, solucao);

    solucao->makespan = calcular_makespan(instancia, solucao);
}

void busca_local(const Instancia *instancia, Solucao *solucao) {
    int melhor_makespan = solucao->makespan;
    Solucao melhor_solucao = *solucao;
    int num_iteracoes = 0;
    int num_iteracoes_sem_melhoria = 0;
    const int max_iteracoes_sem_melhoria = 9;

    while (num_iteracoes < 10 && num_iteracoes_sem_melhoria < max_iteracoes_sem_melhoria) {
        int vizinho_encontrado = 0;

        // Trocar as estações das tarefas t1 e t2
        for (int t1 = 0; t1 < instancia->num_tarefas - 1; t1++) {
            for (int t2 = t1 + 1; t2 < instancia->num_tarefas; t2++) {
                Solucao vizinho = *solucao;
                int temp = vizinho.estacao[t1];
                vizinho.estacao[t1] = vizinho.estacao[t2];
                vizinho.estacao[t2] = temp;
                int makespan_vizinho = calcular_makespan(instancia, &vizinho);

                if (makespan_vizinho < melhor_makespan) {
                    melhor_makespan = makespan_vizinho;
                    melhor_solucao = vizinho;
                    vizinho_encontrado = 1;
                    num_iteracoes_sem_melhoria = 0;
                }
            }
        }

        if (!vizinho_encontrado) {
            num_iteracoes_sem_melhoria++;
        }

        num_iteracoes++;
    }

    *solucao = melhor_solucao;

    printf("Número de iterações: %d\n", num_iteracoes);
}
void imprimir_solucao(const Instancia *instancia, const Solucao *solucao) {
    printf("Solução Final:\n");
    for (int m = 0; m < instancia->num_estacoes; m++) {
        printf("Máquina %d: ", m + 1);

        for (int t = 0; t < instancia->num_tarefas; t++) {
            if (solucao->estacao[t] == m) {
                printf("%d,", t + 1);
            }
        }

        printf("\n");
    }
    printf("FO Inicial: %d\n", solucao->makespan);
    printf("FO Final: %d\n", calcular_makespan(instancia, solucao));
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

    // Imprimir as máquinas e suas funcionalidades
    
    for (int num_maquinas = 3; num_maquinas <= 11; num_maquinas++) {
        printf("estacao: %d\n", num_maquinas);

        // Definir o número de máquinas
        instancia.num_estacoes = num_maquinas;

        // Resolver a instância
        resolver_instancia(&instancia, &solucao);

        // Realizar a busca local
        busca_local(&instancia, &solucao);

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
