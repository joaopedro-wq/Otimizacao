#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_TAREFAS 10000
#define MAX_ESTACOES 10

typedef struct {
    int id;             // Identificador da tarefa
    int tempoProcessamento;  // Tempo necessário para processar a tarefa
    
} Tarefa;

typedef struct {
    int id;             // Identificador da estação de trabalho
    
} EstacaoTrabalho;

typedef struct {
    int numEstacoes;                // Número de estações de trabalho
    int numTarefas;                 // Número de tarefas
    Tarefa tarefas[MAX_TAREFAS];    // Vetor de tarefas
    EstacaoTrabalho estacoes[MAX_ESTACOES];  // Vetor de estações de trabalho
    
} ProblemaBalanceamento;

typedef struct {
    int atribuicaoTarefas[MAX_TAREFAS];  // Vetor de atribuição de tarefas para as estações de trabalho
} Solucao;

ProblemaBalanceamento lerInstancia(const char* nomeArquivo) {
  ProblemaBalanceamento problema;
    FILE* arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        exit(1);
    }

    // Leitura do número de estações de trabalho e número de tarefas
    fscanf(arquivo, "%d %d", &problema.numEstacoes, &problema.numTarefas);

    // Leitura das informações das tarefas
    for (int i = 0; i < problema.numTarefas; i++) {
        fscanf(arquivo, "%d %d", &problema.tarefas[i].id, &problema.tarefas[i].tempoProcessamento);
    }

    // Leitura das informações das estações de trabalho
    for (int i = 0; i < problema.numEstacoes; i++) {
        fscanf(arquivo, "%d", &problema.estacoes[i].id);
    }

    fclose(arquivo);
    return problema;
    
}


void criarSolucaoInicial(ProblemaBalanceamento problema, int sequenciaTarefas[]) {
    int numTarefas = problema.numTarefas;

    // Criar um vetor temporário com os índices das tarefas
    int indicesTarefas[MAX_TAREFAS];
    for (int i = 0; i < numTarefas; i++) {
        indicesTarefas[i] = i;
    }

    // Embaralhar os índices das tarefas usando o algoritmo Fisher-Yates
    for (int i = numTarefas - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = indicesTarefas[i];
        indicesTarefas[i] = indicesTarefas[j];
        indicesTarefas[j] = temp;
    }

    // Copiar a sequência de tarefas embaralhada para o vetor de sequência de tarefas
    for (int i = 0; i < numTarefas; i++) {
        sequenciaTarefas[i] = indicesTarefas[i];
    }
}


int calcularMakespanSequencia(ProblemaBalanceamento problema, int sequenciaTarefas[]) {
    int makespan = 0;
    int tempoEstacao[problema.numEstacoes];  // Armazena o tempo total de processamento em cada estação

    // Inicializa o tempo das estações como zero
    for (int i = 0; i < problema.numEstacoes; i++) {
        tempoEstacao[i] = 0;
    }

    // Processa cada tarefa na sequência e atualiza os tempos das estações
    for (int i = 0; i < problema.numTarefas; i++) {
        int idTarefa = sequenciaTarefas[i];
        int idEstacao = i % problema.numEstacoes;
        int tempoTarefa = problema.tarefas[idTarefa].tempoProcessamento;

        // Atualiza o tempo da estação
        tempoEstacao[idEstacao] += tempoTarefa;

        // Atualiza o makespan se o tempo da estação for maior que o makespan atual
        if (tempoEstacao[idEstacao] > makespan) {
            makespan = tempoEstacao[idEstacao];
        }
    }

    return makespan;
}

void buscaLocal(ProblemaBalanceamento problema, int sequenciaTarefas[]) {
    int melhorMakespan = calcularMakespanSequencia(problema, sequenciaTarefas);

    // Variável para verificar se houve alguma melhoria na iteração atual
    int melhorou = 1;

    while (melhorou) {
        melhorou = 0;

        for (int i = 0; i < problema.numTarefas; i++) {
            for (int j = i + 1; j < problema.numTarefas; j++) {
                // Realiza a troca de duas tarefas na sequência
                int temp = sequenciaTarefas[i];
                sequenciaTarefas[i] = sequenciaTarefas[j];
                sequenciaTarefas[j] = temp;

                // Calcula o makespan da nova sequência
                int novoMakespan = calcularMakespanSequencia(problema, sequenciaTarefas);

                // Verifica se houve melhoria
                if (novoMakespan < melhorMakespan) {
                    melhorMakespan = novoMakespan;
                    melhorou = 1;
                } else {
                    // Desfaz a troca se não houve melhoria
                    temp = sequenciaTarefas[i];
                    sequenciaTarefas[i] = sequenciaTarefas[j];
                    sequenciaTarefas[j] = temp;
                }
            }
        }
    }
}
void imprimirSequenciaTarefas(ProblemaBalanceamento problema, int sequenciaTarefas[]) {
    for (int i = 0; i < problema.numTarefas; i++) {
        printf("%d ", sequenciaTarefas[i]);
    }
    printf("\n");
}


int main() {
    ProblemaBalanceamento problema = lerInstancia("instancia1.txt");
    if (problema.numEstacoes == 0 || problema.numTarefas == 0) {
        printf("Erro ao ler a instância.\n");
        return 0;
    }

    int solucaoInicial[problema.numTarefas];
    criarSolucaoInicial(problema, solucaoInicial);
    int makespanInicial = calcularMakespanSequencia(problema, solucaoInicial);

    printf("Solução Inicial:\n");
    imprimirSequenciaTarefas(problema, solucaoInicial);
    printf("Makespan Inicial: %d\n", makespanInicial);

    // Busca local
    int melhorMakespan = makespanInicial;
    int melhorSolucao[problema.numTarefas];
    memcpy(melhorSolucao, solucaoInicial, sizeof(solucaoInicial));

    int iteracoes = 100; // Número de iterações da busca local
    for (int i = 0; i < iteracoes; i++) {
        int vizinho[problema.numTarefas];
        memcpy(vizinho, melhorSolucao, sizeof(melhorSolucao));

        // Gere um vizinho trocando duas tarefas adjacentes
        int posicao = rand() % (problema.numTarefas - 1);
        int temp = vizinho[posicao];
        vizinho[posicao] = vizinho[posicao + 1];
        vizinho[posicao + 1] = temp;

        int makespanVizinho = calcularMakespanSequencia(problema, vizinho);
        if (makespanVizinho < melhorMakespan) {
            melhorMakespan = makespanVizinho;
            memcpy(melhorSolucao, vizinho, sizeof(vizinho));
        }
    }

    printf("Melhor Solução:\n");
    imprimirSequenciaTarefas(problema, melhorSolucao);
    printf("Melhor Makespan: %d\n", melhorMakespan);

    // Liberar memória alocada para problema.tarefas e problema.estacoes
    free(problema.tarefas);
    free(problema.estacoes);

    return 0;
}

