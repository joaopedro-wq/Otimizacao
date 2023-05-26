#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_TAREFAS 10000
#define MAX_ESTACOES 10000

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

void criarSolucaoInicial(ProblemaBalanceamento problema, int solucao[]) {
    int numTarefas = problema.numTarefas;
    int numEstacoes = problema.numEstacoes;

    // Inicializar vetor de solução
    for (int i = 0; i < numTarefas; i++) {
        solucao[i] = i % numEstacoes;  // Atribuir tarefa à estação de trabalho
    }
}


int calcularMakespan(ProblemaBalanceamento problema, int solucao[]) {
    int numEstacoes = problema.numEstacoes;
    int temposEstacoes[numEstacoes];

    // Inicializar os tempos de processamento das estações de trabalho como zero
    for (int i = 0; i < numEstacoes; i++) {
        temposEstacoes[i] = 0;
    }

    int makespan = 0;

    for (int i = 0; i < problema.numTarefas; i++) {
        int estacao = solucao[i] - 1;  // Ajustar índice da estação de trabalho (inicia em 0)

        // Atualizar o tempo da estação de trabalho com o tempo de processamento da             tarefa           atual
        temposEstacoes[estacao] += problema.tarefas[i].tempoProcessamento;

        // Verificar se o tempo da estação atual é maior que o makespan atual
        if (temposEstacoes[estacao] > makespan) {
            makespan = temposEstacoes[estacao];
        }
    }

    return makespan;
}


int main() {
    ProblemaBalanceamento problema;
    int *solucao;

    // Ler instância do problema
    problema = lerInstancia("KILBRID.IN2");

    // Alocar memória para o vetor de solução
    solucao = (int *)malloc(problema.numTarefas * sizeof(int));

    // Verificar se a alocação de memória foi bem sucedida
    if (solucao == NULL) {
        printf("Erro ao alocar memória para o vetor de solução.\n");
        return 1;
    }

    // Criar solução inicial
    criarSolucaoInicial(problema, solucao);

    // Imprimir a solução inicial
    printf("Solução Inicial:\n");
    for (int i = 0; i < problema.numTarefas; i++) {
        printf("Tarefa %d -> Estação %d\n", i+1, solucao[i]);
    }
  
   int makespan = calcularMakespan(problema, solucao);
    printf("Makespan da solução inicial: %d\n", makespan);

  

    // Liberar memória alocada
    free(problema.tarefas);
    free(problema.estacoes);
    free(solucao);

    return 0;
}
