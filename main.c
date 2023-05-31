#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  

#define MAX_TAREFAS 100
#define MAX_ESTACOES 10

typedef struct {
    int numTarefas;
    int numEstacoes;
    int tempoTarefas[MAX_TAREFAS];
    int capacidades[MAX_ESTACOES];
} ProblemaBalanceamento;

typedef struct {
    int estacoes[MAX_TAREFAS];
} SolucaoInicial;

ProblemaBalanceamento lerInstancia(const char* nomeArquivo) {
    ProblemaBalanceamento problema;
    FILE* arquivo = fopen(nomeArquivo, "r");
    
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de instância.\n");
        exit(1);
    }
    
    fscanf(arquivo, "%d", &problema.numTarefas);
    fscanf(arquivo, "%d", &problema.numEstacoes);
    
    for (int i = 0; i < problema.numTarefas; i++) {
        fscanf(arquivo, "%d", &problema.tempoTarefas[i]);
    }
    
    for (int i = 0; i < problema.numEstacoes; i++) {
        fscanf(arquivo, "%d", &problema.capacidades[i]);
    }
    
    fclose(arquivo);
    
    return problema;
}

void criarSolucaoInicial(ProblemaBalanceamento problema, int solucao[]) {
  int numTarefas = problema.numTarefas;
  int numEstacoes = problema.numEstacoes;

  // Inicializar todas as tarefas na estação 0
  for (int i = 0; i < numTarefas; i++) {
    solucao[i] = 0;
  }

  // Atribuir as tarefas às estações
  for (int i = 0; i < numTarefas; i++) {
    // Gerar um número aleatório entre 1 e o número de estações
    int estacao = (rand() % numEstacoes) + 1;
    solucao[i] = estacao;
  }
}

int calcularMakespan(ProblemaBalanceamento problema, int solucao[]) {
    int makespan = 0;
    int tempoEstacao[problema.numEstacoes];

    // Inicializar os tempos das estações como zero
    for (int i = 0; i < problema.numEstacoes; i++) {
        tempoEstacao[i] = 0;
    }

    // Calcular o tempo de conclusão de cada tarefa
    for (int i = 0; i < problema.numTarefas; i++) {
        int tarefa = i + 1;
        int estacao = solucao[i];
        int tempoTarefa = problema.tempoTarefas[tarefa];

        // Verificar se a tarefa pode ser executada imediatamente na estação atual
        if (tempoEstacao[estacao] > 0) {
            // Caso contrário, adicionar o tempo de espera à tarefa
            tempoTarefa += tempoEstacao[estacao];
        }

        // Atualizar o tempo de conclusão da estação
        tempoEstacao[estacao] = tempoTarefa;

        // Atualizar o makespan, se necessário
        if (tempoTarefa > makespan) {
            makespan = tempoTarefa;
        }
    }

    return makespan;
}

void buscaLocal(ProblemaBalanceamento problema, int solucao[]) {
    int melhorMakespan = calcularMakespan(problema, solucao);
    int atualMakespan = melhorMakespan;
    int melhorSolucao[MAX_TAREFAS];
    memcpy(melhorSolucao, solucao, sizeof(solucao));

    int iteracoes = 0;
    while (iteracoes < MAX_TAREFAS) {
        int i = rand() % problema.numTarefas;
        int j = (rand() % 2 == 0) ? i + 1 : i - 1;

        if (j < 0 || j >= problema.numTarefas) {
            continue; // Ignorar movimento inválido
        }

        // Realizar o movimento de troca entre tarefas i e j
        int temp = solucao[i];
        solucao[i] = solucao[j];
        solucao[j] = temp;

        // Calcular o makespan da nova solução
        int novoMakespan = calcularMakespan(problema, solucao);

        if (novoMakespan < melhorMakespan) {
            // Atualizar a melhor solução encontrada
            melhorMakespan = novoMakespan;
            memcpy(melhorSolucao, solucao, sizeof(solucao));
        } else {
            // Desfazer o movimento de troca
            temp = solucao[i];
            solucao[i] = solucao[j];
            solucao[j] = temp;
        }

        iteracoes++;
    }

    // Atualizar a solução com a melhor encontrada
    memcpy(solucao, melhorSolucao, sizeof(solucao));
}



int main() {
    srand(time(NULL));

    ProblemaBalanceamento problema = lerInstancia("instancia1.txt");
    int solucao[MAX_TAREFAS];

    // Criar a solução inicial
    criarSolucaoInicial(problema, solucao);

    // Imprimir a solução inicial
    printf("Solução Inicial:\n");
    for (int i = 0; i < problema.numTarefas; i++) {
        printf("Tarefa %d: Estação %d\n", i + 1, solucao[i]);
    }

    // Aplicar busca local
    buscaLocal(problema, solucao);

    // Imprimir a solução final
    printf("Solução Final:\n");
    for (int i = 0; i < problema.numTarefas; i++) {
        printf("Tarefa %d: Estação %d\n", i + 1, solucao[i]);
    }

    // Calcular e imprimir o makespan final
    int makespanFinal = calcularMakespan(problema, solucao);
    printf("Makespan Final: %d\n", makespanFinal);

    return 0;
}
