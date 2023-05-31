#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_TAREFAS 100
#define MAX_MAQUINAS 11

typedef struct {
  int num_tarefas;
  int tempos[MAX_TAREFAS];
  int precedencias[MAX_TAREFAS][2];
} Instancia;

typedef struct {
  int num_estacoes;
  int maquinas[MAX_TAREFAS];
} Solucao;

Instancia lerInstancia(const char *nome_arquivo) {
  FILE *arquivo = fopen(nome_arquivo, "r");
  Instancia instancia;

  if (arquivo == NULL) {
    printf("Erro ao abrir o arquivo %s\n", nome_arquivo);
    exit(1);
  }

  fscanf(arquivo, "%d", &instancia.num_tarefas);

  for (int i = 0; i < instancia.num_tarefas; i++) {
    fscanf(arquivo, "%d", &instancia.tempos[i]);
  }

  int i = 0;
  while (fscanf(arquivo, "%d,%d", &instancia.precedencias[i][0],
                &instancia.precedencias[i][1]) != EOF) {
    i++;
  }

  fclose(arquivo);
  return instancia;
}

Solucao criarSolucaoInicial(const Instancia *instancia, int num_estacoes) {
  Solucao solucao;
  solucao.num_estacoes = num_estacoes;

  for (int i = 0; i < instancia->num_tarefas; i++) {
    solucao.maquinas[i] = rand() % num_estacoes;
  }

  return solucao;
}

void escreverSolucao(const Solucao *solucao, const Instancia *instancia) {
  for (int e = 0; e < solucao->num_estacoes; e++) {
    printf("Máquina %d:", e + 1);
    for (int t = 0; t < instancia->num_tarefas; t++) {
      if (solucao->maquinas[t] == e) {
        printf(" %d", t + 1);
      }
    }
    printf("\n");
  }
}

int calcularMakespan(const Instancia *instancia, const Solucao *solucao) {
  int makespan = 0;
  int finalizacao[MAX_MAQUINAS] = {0};

  for (int t = 0; t < instancia->num_tarefas; t++) {
    int maquina = solucao->maquinas[t];
    int inicio = finalizacao[maquina];
    int fim = inicio + instancia->tempos[t];

    for (int i = 0; i < instancia->num_tarefas; i++) {
      if (instancia->precedencias[i][1] == t + 1 &&
          solucao->maquinas[i] == maquina) {
        inicio =
            (inicio > finalizacao[maquina]) ? inicio : finalizacao[maquina];
      }
    }

    finalizacao[maquina] = fim;
    makespan = (fim > makespan) ? fim : makespan;
  }

  return makespan;
}

int main() {
  srand(time(NULL));

  Instancia instancia = lerInstancia("KILBRID.IN2");
  int num_estacoes;

  printf("Digite o número de estações (entre 3 e 11): ");
  scanf("%d", &num_estacoes);

  Solucao melhor_solucao;
  int melhor_makespan = INT_MAX;

  for (int iteracao = 0; iteracao < 1000; iteracao++) {
    Solucao solucao = criarSolucaoInicial(&instancia, num_estacoes);
    int makespan = calcularMakespan(&instancia, &solucao);

    if (makespan < melhor_makespan) {
      melhor_makespan = makespan;
      melhor_solucao = solucao;
    }
  }

  printf("Melhor Solução Encontrada:\n");
  escreverSolucao(&melhor_solucao, &instancia);
  printf("Makespan: %d\n", melhor_makespan);

  return 0;
}
