#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_TAREFAS 1000
#define MAX_MACHINES 100

typedef struct {
    int custo;
    int precedencia[MAX_TAREFAS];
    int num_predecessores;
} Tarefa;

typedef struct {
    int tarefas[MAX_TAREFAS];
    int num_tarefas;
} Maquina;

Tarefa tarefas[MAX_TAREFAS];
Maquina maquinas[MAX_MACHINES];
int num_tarefas;
int num_maquinas;

void lerInstancias(char* arquivo) {
    FILE* file = fopen(arquivo, "r");
    if (file == NULL) {
        printf("Erro ao abrir arquivo\n");
        exit(1);
    }

    fscanf(file, "%d", &num_tarefas);

    for (int i = 0; i < num_tarefas; i++) {
        fscanf(file, "%d", &tarefas[i].custo);
    }

    int tarefa1, tarefa2;
    while (fscanf(file, "%d,%d", &tarefa1, &tarefa2) == 2) {
        if (tarefa1 == -1 && tarefa2 == -1)
            break;
        tarefas[tarefa2 - 1].precedencia[tarefas[tarefa2 - 1].num_predecessores++] = tarefa1 - 1;
    }

    fclose(file);
}

void criarSolucaoInicial() {
    int tarefasRestantes[num_tarefas];
    int tarefasEmOrdemPrecedencia[num_tarefas];
    int num_tarefasRestantes = num_tarefas;
    int count = 0;

    memset(tarefasRestantes, 1, sizeof(tarefasRestantes));
    memset(tarefasEmOrdemPrecedencia, 0, sizeof(tarefasEmOrdemPrecedencia));

    int cargaMaquinas[MAX_MACHINES] = {0};

    for (int i = 0; i < num_maquinas; i++) {
        maquinas[i].num_tarefas = 0;
    }

    while (num_tarefasRestantes > 0) {
        int tarefaSelecionada = -1;
        int menorCustoMaquina = INT_MAX;

        for (int i = 0; i < num_tarefas; i++) {
            if (tarefasRestantes[i]) {
                int podeAtribuir = 1;

                for (int j = 0; j < tarefas[i].num_predecessores; j++) {
                    int tarefaPredecessora = tarefas[i].precedencia[j];
                    if (tarefasRestantes[tarefaPredecessora]) {
                        podeAtribuir = 0;
                        break;
                    }
                }

                if (podeAtribuir) {
                    int custoTarefa = tarefas[i].custo;
                    for (int k = 0; k < num_maquinas; k++) {
                        if (cargaMaquinas[k] + custoTarefa < menorCustoMaquina) {
                            menorCustoMaquina = cargaMaquinas[k] + custoTarefa;
                            tarefaSelecionada = i;
                        }
                    }
                }
            }
        }
        
        if (tarefaSelecionada != -1) {
            tarefasEmOrdemPrecedencia[count] = tarefaSelecionada + 1;
            tarefasRestantes[tarefaSelecionada] = 0;
            num_tarefasRestantes--;
            count++;
            int custoTarefaSelecionada = tarefas[tarefaSelecionada].custo;
            for (int k = 0; k < num_maquinas; k++) {
                if (cargaMaquinas[k] + custoTarefaSelecionada < menorCustoMaquina) {
                    menorCustoMaquina = cargaMaquinas[k] + custoTarefaSelecionada;
                }
            }
            cargaMaquinas[tarefaSelecionada % num_maquinas] += custoTarefaSelecionada;
        }
    }

    int minTarefasPorMaquina = num_tarefas / num_maquinas;

    for (int i = 0; i < num_maquinas; i++) {
        int num_tarefasMaquina = minTarefasPorMaquina;

        if (i == num_maquinas - 1) {
            num_tarefasMaquina += num_tarefas % num_maquinas;
        }

        for (int j = 0; j < num_tarefasMaquina; j++) {
            int tarefaAtual = tarefasEmOrdemPrecedencia[i * minTarefasPorMaquina + j] - 1;
            maquinas[i].tarefas[maquinas[i].num_tarefas] = tarefaAtual;
            maquinas[i].num_tarefas++;

            cargaMaquinas[i] += tarefas[tarefaAtual].custo;
        }
    }

}

int calcularMakespan() {
    int makespan = 0;

    for (int i = 0; i < num_maquinas; i++) {
        int carga = 0;

        for (int j = 0; j < maquinas[i].num_tarefas; j++) {
            int tarefa = maquinas[i].tarefas[j];
            carga += tarefas[tarefa].custo;
        }

        if (carga > makespan)
            makespan = carga;
    }
    return makespan;
}

void escreverSolucao() {
    for (int i = 0; i < num_maquinas; i++) {
        printf("Máquina %d:", i + 1);
        for (int j = 0; j < maquinas[i].num_tarefas; j++) {
            int tarefa = maquinas[i].tarefas[j];
            printf(" %d", tarefa + 1);
        }
        printf(" (tarefas atendidas)\n");
    }

    int makespan = calcularMakespan();
    printf("FO: %d \n", makespan);
}

void aplicarRefinamento() {
    int makespan = calcularMakespan();

    for (int i = 0; i < num_maquinas; i++) {
        int tarefasMaquinaInicial = maquinas[i].num_tarefas;

        for (int j = 0; j < tarefasMaquinaInicial; j++) {
            int tarefa = maquinas[i].tarefas[j];
            maquinas[i].num_tarefas--;

            int melhorMaquina = -1;
            int menorMakespan = makespan;

            for (int k = 0; k < num_maquinas; k++) {
                if (k != i) {
                    maquinas[k].tarefas[maquinas[k].num_tarefas] = tarefa;
                    maquinas[k].num_tarefas++;

                    int novoMakespan = calcularMakespan();

                    if (novoMakespan < menorMakespan) {
                        menorMakespan = novoMakespan;
                        melhorMaquina = k;
                    }

                    maquinas[k].num_tarefas--;
                }
            }

            if (melhorMaquina != -1) {
                maquinas[melhorMaquina].tarefas[maquinas[melhorMaquina].num_tarefas] = tarefa;
                maquinas[melhorMaquina].num_tarefas++;

                makespan = menorMakespan;
            }
            else {
                maquinas[i].tarefas[maquinas[i].num_tarefas] = tarefa;
                maquinas[i].num_tarefas++;
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Uso: %s <arquivo_instancias>\n", argv[0]);
        return 1;
    }

    lerInstancias(argv[1]);
    num_maquinas = num_tarefas / 2;

    criarSolucaoInicial();

    printf("Solução inicial:\n");
    escreverSolucao();

    aplicarRefinamento();

    printf("\nSolução refinada:\n");
    escreverSolucao();

    return 0;
}
