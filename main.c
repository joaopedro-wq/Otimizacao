#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

#define MAX_TAREFAS 1000
#define MAX_MAQUINAS 11
#define MAX_ITERACOES_SEM_MELHORIA 1000

typedef struct
{
    int custo;
    int precedencia[MAX_TAREFAS];
    int num_predecessores;
} Tarefa;

typedef struct Precedencia
{
    int tarefa;
    struct Precedencia *proxima;
} Precedencia;

typedef struct
{
    Precedencia *primeira;
    Precedencia *ultima;
} OrdemPrecedencia;

extern Tarefa tarefas[MAX_TAREFAS];
extern int num_tarefas;

Tarefa tarefas[MAX_TAREFAS];
int num_tarefas;

void lerInstancias(const char *arquivo)
{
    FILE *file = fopen(arquivo, "r");
    if (file == NULL)
    {
        printf("Erro ao abrir arquivo\n");
        exit(1);
    }

    fscanf(file, "%d", &num_tarefas);

    for (int i = 0; i < num_tarefas; i++)
    {
        fscanf(file, "%d", &tarefas[i].custo);
    }

    int tarefa1, tarefa2;
    while (fscanf(file, "%d,%d", &tarefa1, &tarefa2) == 2)
    {
        if (tarefa1 == -1 && tarefa2 == -1)
            break;
        tarefas[tarefa2 - 1].precedencia[tarefas[tarefa2 - 1].num_predecessores++] = tarefa1 - 1;
    }

    fclose(file);
}

void imprimirPrecedencias(const Tarefa *tarefas, int num_tarefas)
{
    for (int i = 0; i < num_tarefas; i++)
    {
        printf("Tarefa %d: Precedências [", i + 1);
        for (int j = 0; j < tarefas[i].num_predecessores; j++)
        {
            printf("%d", tarefas[i].precedencia[j] + 1);
            if (j < tarefas[i].num_predecessores - 1)
            {
                printf(", ");
            }
        }
        printf("]\n");
    }
}

int *obterPrecedencias(int tarefa, int *num_precedencias)
{
    int *precedencias = malloc(MAX_TAREFAS * sizeof(int));
    *num_precedencias = 0;

    // Verificar as precedências diretas
    for (int i = 0; i < tarefas[tarefa].num_predecessores; i++)
    {
        int precedencia = tarefas[tarefa].precedencia[i];

        // Verificar se a precedência já está no vetor
        int repetido = 0;
        for (int j = 0; j < *num_precedencias; j++)
        {
            if (precedencias[j] == precedencia)
            {
                repetido = 1;
                break;
            }
        }

        // Adicionar a precedência somente se não estiver repetida
        if (!repetido)
        {
            precedencias[*num_precedencias] = precedencia;
            (*num_precedencias)++;

            // Chamar recursivamente para obter as precedências indiretas
            int indiretas_num_precedencias;
            int *indiretas = obterPrecedencias(precedencia, &indiretas_num_precedencias);
            for (int j = 0; j < indiretas_num_precedencias; j++)
            {
                int precedencia_indireta = indiretas[j];
                // Verificar se a precedência indireta já está no vetor
                int repetido_indireto = 0;
                for (int k = 0; k < *num_precedencias; k++)
                {
                    if (precedencias[k] == precedencia_indireta)
                    {
                        repetido_indireto = 1;
                        break;
                    }
                }

                // Adicionar a precedência indireta somente se não estiver repetida
                if (!repetido_indireto)
                {
                    precedencias[*num_precedencias] = precedencia_indireta;
                    (*num_precedencias)++;
                }
            }
            free(indiretas);
        }
    }

    return precedencias;
}

void imprimirPrecedenciasDiretasEIndiretas(const Tarefa *tarefas, int num_tarefas)
{
    for (int i = 0; i < num_tarefas; i++)
    {
        int num_precedencias;
        int *precedencias = obterPrecedencias(i, &num_precedencias);

        printf("Tarefa %d: Precedências [", i + 1);
        for (int j = 0; j < num_precedencias; j++)
        {
            printf("%d", precedencias[j] + 1);
            if (j < num_precedencias - 1)
            {
                printf(", ");
            }
        }
        printf("]\n");

        free(precedencias);
    }
}

bool todasPrecedenciasAlocadas(int tarefa, const int *precedencias, int num_precedencias, const bool *alocada)
{
    for (int i = 0; i < num_precedencias; i++)
    {
        if (!alocada[precedencias[i]])
        {
            return false;
        }
    }
    return true;
}

void atribuirTarefa(int tarefa, const int *precedencias, int num_precedencias, bool *alocada, int *ordem, int *indice_ordem, int *makespan)
{
    if (alocada[tarefa])
    {
        return;
    }

    if (!todasPrecedenciasAlocadas(tarefa, precedencias, num_precedencias, alocada))
    {
        return;
    }

    alocada[tarefa] = true;
    ordem[(*indice_ordem)++] = tarefa;

    // Atualizar makespan da máquina correspondente
    int maquina = (*indice_ordem - 1) % MAX_MAQUINAS;
    *makespan += tarefas[tarefa].custo;

    for (int i = 0; i < num_tarefas; i++)
    {
        if (tarefas[i].num_predecessores > 0)
        {
            int *precedencias_indiretas = obterPrecedencias(i, &num_precedencias);
            atribuirTarefa(i, precedencias_indiretas, num_precedencias, alocada, ordem, indice_ordem, makespan);
            free(precedencias_indiretas);
        }
    }
}

int* criarOrdemAtribuicao(const Tarefa* tarefas, int num_tarefas, int* makespan) {
    bool alocada[MAX_TAREFAS] = {false};
    int* ordem = malloc(num_tarefas * sizeof(int));
    int indiceOrdem = 0;
    *makespan = 0;

    // Passo 1: Construir uma lista restrita de candidatos (CRCL)
    int crclSize = num_tarefas * 0.3; // Tamanho da CRCL (30% do número de tarefas)
    if (crclSize < 1) {
        crclSize = 1; // Pelo menos 1 candidato na CRCL
    }
    int* crcl = malloc(crclSize * sizeof(int));
    int crclIndex = 0;

    // Embaralhar os índices das tarefas
    int indices_embaralhados[MAX_TAREFAS];
    for (int i = 0; i < num_tarefas; i++) {
        indices_embaralhados[i] = i;
    }
    srand(time(NULL));
    for (int i = num_tarefas - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = indices_embaralhados[i];
        indices_embaralhados[i] = indices_embaralhados[j];
        indices_embaralhados[j] = temp;
    }

    for (int i = 0; i < num_tarefas; i++) {
        int tarefa = indices_embaralhados[i];
        int num_precedencias;
        int* precedencias = obterPrecedencias(tarefa, &num_precedencias);

        // Verificar se a tarefa está na CRCL
        if (crclIndex < crclSize) {
            crcl[crclIndex++] = tarefa;
        } else {
            // Verificar se a tarefa tem um makespan menor que alguma tarefa na CRCL
            int tarefaMakespan = tarefas[tarefa].custo;
            int maxMakespan = tarefas[crcl[crclSize - 1]].custo;
            if (tarefaMakespan < maxMakespan) {
                crcl[crclSize - 1] = tarefa;
                // Ordenar a CRCL em ordem crescente de makespan
                for (int j = crclSize - 1; j > 0; j--) {
                    if (tarefas[crcl[j]].custo < tarefas[crcl[j - 1]].custo) {
                        int temp = crcl[j];
                        crcl[j] = crcl[j - 1];
                        crcl[j - 1] = temp;
                    }
                }
            }
        }

        atribuirTarefa(tarefa, precedencias, num_precedencias, alocada, ordem, &indiceOrdem, makespan);
        free(precedencias);
    }

    free(crcl);
    return ordem;
}


void imprimirOrdemAtribuicao(const int *ordem, int num_tarefas)
{
    printf("Ordem de atribuição das tarefas:\n");
    for (int i = 0; i < num_tarefas; i++)
    {
        printf("%d ", ordem[i] + 1);
    }
    printf("\n");
}

void atribuirTarefasPorMaquinas(const int *ordem, int num_tarefas, int num_maquinas)
{

    int *makespanDosCria = 0;
    int tarefas_por_maquina = num_tarefas / num_maquinas;
    int tarefas_extras = num_tarefas % num_maquinas;

    FILE *file = fopen("resultado.txt", "a");
    if (file == NULL)
    {
        printf("Erro ao criar arquivo\n");
        return;
    }

    int tempos_maquinas[MAX_MAQUINAS] = {0}; // Array para armazenar os tempos de cada máquina

    fprintf(file, "Máquina 1:");
    for (int i = 0; i < tarefas_por_maquina + tarefas_extras; i++)
    {
        int tarefa = ordem[i];
        fprintf(file, " %d", tarefa + 1);
        tempos_maquinas[0] += tarefas[tarefa].custo; // Adiciona o tempo da tarefa à máquina 1
    }
    fprintf(file, "\n");

    int tarefa_inicio = tarefas_por_maquina + tarefas_extras;
    for (int i = 1; i < num_maquinas; i++)
    {
        fprintf(file, "Máquina %d:", i + 1);
        for (int j = 0; j < tarefas_por_maquina; j++)
        {
            int tarefa = ordem[tarefa_inicio];
            fprintf(file, " %d", tarefa + 1);
            tempos_maquinas[i] += tarefas[tarefa].custo; // Adiciona o tempo da tarefa à máquina i+1
            tarefa_inicio++;
        }
        fprintf(file, "\n");
    }

    int makespan = 0;
    for (int i = 0; i < num_maquinas; i++)
    {
        if (tempos_maquinas[i] > makespan)
        {
            makespan = tempos_maquinas[i]; // Encontra o maior tempo entre as máquinas
        }
    }

    fprintf(file, " \n");
    printf("FO Inicial: com %d Maquinas:  %d\n", num_maquinas, makespan);
    fprintf(file, "Makespan: %d\n", makespan);

    fclose(file);
}

int calcularMakespan(const int* ordem, int num_tarefas, int num_maquinas, const Tarefa* tarefas)
{
    int tempos_maquinas[MAX_MAQUINAS] = {0}; // Array para armazenar os tempos de cada máquina

    for (int i = 0; i < num_tarefas; i++) {
        int tarefa = ordem[i];
        int maquina = i % num_maquinas;

        // Adiciona o tempo da tarefa à máquina correspondente
        tempos_maquinas[maquina] += tarefas[tarefa].custo;
    }

    int makespan = 0;
    for (int i = 0; i < num_maquinas; i++) {
        if (tempos_maquinas[i] > makespan) {
            makespan = tempos_maquinas[i]; // Encontra o maior tempo entre as máquinas
        }
    }

    return makespan;
}


void imprimirTarefasPorMaquina(const int *ordem, int num_tarefas, int num_maquinas)
{
    int tarefas_por_maquina = num_tarefas / num_maquinas;
    int tarefas_extras = num_tarefas % num_maquinas;

    printf("Solucao por Maquinas:\n");
    for (int i = 0; i < num_maquinas; i++)
    {
        printf("Maquina %d:", i + 1);
        int tarefa_inicio = i * tarefas_por_maquina + (i < tarefas_extras ? i : tarefas_extras);
        int tarefa_fim = tarefa_inicio + tarefas_por_maquina + (i < tarefas_extras ? 1 : 0);
        for (int j = tarefa_inicio; j < tarefa_fim; j++)
        {
            printf(" %d", ordem[j] + 1);
        }
        printf("\n");
    }
}




/*

VERSAO FUNCIONANDO
void aplicarVizinhanca(int* solucao_atual, int num_tarefas, const Tarefa* tarefas) {
    int posicao1 = rand() % (num_tarefas - 1); // Escolhe a primeira posição aleatória na solução
    int posicao2 = rand() % (num_tarefas - 1); // Escolhe a segunda posição aleatória na solução

    // Troca de tarefas adjacentes
    int tarefa1 = solucao_atual[posicao1];
    int tarefa2 = solucao_atual[posicao1 + 1];

    // Verifica se a troca viola as precedências
    for (int i = 0; i < tarefas[tarefa2].num_predecessores; i++) {
        int precedencia = tarefas[tarefa2].precedencia[i];
        if ((precedencia == tarefa1 && posicao1 - 1 >= 0) ||
            (precedencia == solucao_atual[posicao1 - 1] && posicao1 - 2 >= 0)) {
            return; // A troca viola a precedência, portanto, não será realizada
        }
    }

    // Realiza a troca de tarefas adjacentes
    int temp = solucao_atual[posicao1];
    solucao_atual[posicao1] = solucao_atual[posicao1 + 1];
    solucao_atual[posicao1 + 1] = temp;

    // Verifica se a troca de tarefas adjacentes violou as precedências na segunda posição
    if (posicao2 == posicao1 || posicao2 == posicao1 + 1) {
        posicao2 = (posicao2 + 2) % num_tarefas; // Evita que a segunda posição seja a mesma ou adjacente à primeira
    }

    // Troca de tarefas não adjacentes
    int tarefa3 = solucao_atual[posicao2];
    int tarefa4 = solucao_atual[posicao2 + 1];

    // Verifica se a troca viola as precedências
    for (int i = 0; i < tarefas[tarefa4].num_predecessores; i++) {
        int precedencia = tarefas[tarefa4].precedencia[i];
        if ((precedencia == tarefa3 && posicao2 - 1 >= 0) ||
            (precedencia == solucao_atual[posicao2 - 1] && posicao2 - 2 >= 0)) {
            return; // A troca viola a precedência, portanto, não será realizada
        }
    }

    // Realiza a troca de tarefas não adjacentes
    temp = solucao_atual[posicao2];
    solucao_atual[posicao2] = solucao_atual[posicao2 + 1];
    solucao_atual[posicao2 + 1] = temp;
}


*/


//NAO TESTEI TOTALMENTE AINDA
void aplicarVizinhanca(int* solucao_atual, int num_tarefas, const Tarefa* tarefas) {
    int num_trocas = rand() % (num_tarefas / 2) + 1; // Número aleatório de trocas (pelo menos 1 troca)

    for (int i = 0; i < num_trocas; i++) {
        int posicao1 = rand() % (num_tarefas - 1); // Escolhe a primeira posição aleatória na solução
        int posicao2 = rand() % (num_tarefas - 1); // Escolhe a segunda posição aleatória na solução

        // Troca de tarefas adjacentes
        int tarefa1 = solucao_atual[posicao1];
        int tarefa2 = solucao_atual[posicao1 + 1];

        // Verifica se a troca viola as precedências
        bool viola_precedencia_adjacente = false;
        for (int j = 0; j < tarefas[tarefa2].num_predecessores; j++) {
            int precedencia = tarefas[tarefa2].precedencia[j];
            if ((precedencia == tarefa1 && posicao1 - 1 >= 0) ||
                (precedencia == solucao_atual[posicao1 - 1] && posicao1 - 2 >= 0)) {
                viola_precedencia_adjacente = true;
                break;
            }
        }

        // Troca de tarefas não adjacentes
        int tarefa3 = solucao_atual[posicao2];
        int tarefa4 = solucao_atual[posicao2 + 1];

        // Verifica se a troca viola as precedências
        bool viola_precedencia_nao_adjacente = false;
        for (int j = 0; j < tarefas[tarefa4].num_predecessores; j++) {
            int precedencia = tarefas[tarefa4].precedencia[j];
            if ((precedencia == tarefa3 && posicao2 - 1 >= 0) ||
                (precedencia == solucao_atual[posicao2 - 1] && posicao2 - 2 >= 0)) {
                viola_precedencia_nao_adjacente = true;
                break;
            }
        }

        // Verifica se alguma das trocas viola as precedências
        if (viola_precedencia_adjacente || viola_precedencia_nao_adjacente) {
            continue; // A troca viola a precedência, portanto, passa para a próxima troca
        }

        // Realiza a troca de tarefas adjacentes
        int temp = solucao_atual[posicao1];
        solucao_atual[posicao1] = solucao_atual[posicao1 + 1];
        solucao_atual[posicao1 + 1] = temp;

        // Realiza a troca de tarefas não adjacentes
        temp = solucao_atual[posicao2];
        solucao_atual[posicao2] = solucao_atual[posicao2 + 1];
        solucao_atual[posicao2 + 1] = temp;
    }
}


void buscarLocalmenteGRASP(int* ordem, int num_tarefas, const Tarefa* tarefas, int num_iteracoes)
{
    int* melhor_solucao = malloc(num_tarefas * sizeof(int));
    memcpy(melhor_solucao, ordem, num_tarefas * sizeof(int));
    int melhor_makespan = calcularMakespan(melhor_solucao, num_tarefas, MAX_MAQUINAS, tarefas);

    int iteracao = 0;
    int num_iteracoes_sem_melhoria = 0;
    const int MAX_ITERACOES_GRASP_LOCAL = 1000;


    while (iteracao < num_iteracoes && num_iteracoes_sem_melhoria < MAX_ITERACOES_SEM_MELHORIA)
    {
        int* vizinho = malloc(num_tarefas * sizeof(int));
        memcpy(vizinho, ordem, num_tarefas * sizeof(int));

        aplicarVizinhanca(vizinho, num_tarefas, tarefas); // Aplica a vizinhança ao vizinho

        int vizinho_makespan = calcularMakespan(vizinho, num_tarefas, MAX_MAQUINAS, tarefas); // Calcula o makespan do vizinho

        if (vizinho_makespan < melhor_makespan)
        {
            free(melhor_solucao);
            melhor_solucao = vizinho;
            melhor_makespan = vizinho_makespan;
            num_iteracoes_sem_melhoria = 0;
        }
        else
        {
            free(vizinho);
            num_iteracoes_sem_melhoria++;
        }

        iteracao++;
    }

    // Atualiza a solução atual com a melhor solução encontrada
    memcpy(ordem, melhor_solucao, num_tarefas * sizeof(int));

    free(melhor_solucao);
}



void executarGRASP(const char *arquivo_instancia, int tempo_limite)
{
    lerInstancias(arquivo_instancia);

    double duration = 0;
    clock_t start_time = clock();

    int makespan;
    int *ordem = criarOrdemAtribuicao(tarefas, num_tarefas, &makespan);

    // Impressão das soluções iniciais para as máquinas 3, 7 e 11
    for (int i = 0; i < 3; i++)
    {
        int num_maquinas = (i == 0) ? 3 : (i == 1) ? 7 : 11;
        printf("Solução para máquina %d:\n", num_maquinas);
        atribuirTarefasPorMaquinas(ordem, num_tarefas, num_maquinas);
        imprimirTarefasPorMaquina(ordem, num_tarefas, num_maquinas);
        printf("\n");
    }

    // Aplicação do método GRASP
    int *melhor_solucao = ordem;
    int melhor_makespan = makespan;
    int iteracao = 0;
    int num_iteracoes_sem_melhoria = 0;

    while (duration < tempo_limite)
    {
        int *vizinho = malloc(num_tarefas * sizeof(int));
        memcpy(vizinho, melhor_solucao, num_tarefas * sizeof(int));

        aplicarVizinhanca(vizinho, num_tarefas, tarefas); // Aplica a vizinhança ao vizinho

        int vizinho_makespan = calcularMakespan(vizinho, num_tarefas, MAX_MAQUINAS, tarefas); // Calcula o makespan do vizinho

        if (vizinho_makespan < melhor_makespan)
        {
            free(melhor_solucao);
            melhor_solucao = vizinho;
            melhor_makespan = vizinho_makespan;
            num_iteracoes_sem_melhoria = 0;
        }
        else
        {
            free(vizinho);
            num_iteracoes_sem_melhoria++;
        }

        // Cálculo do tempo decorrido
        clock_t current_time = clock();
        duration = (double)(current_time - start_time) / CLOCKS_PER_SEC;

        buscarLocalmenteGRASP(melhor_solucao, num_tarefas, tarefas, melhor_makespan); // Chamada da busca local
    }

    printf("Melhor solução encontrada para o arquivo %s:\n", arquivo_instancia);
    for (int i = 0; i < 3; i++)
    {
        int num_maquinas = (i == 0) ? 3 : (i == 1) ? 7 : 11;
        printf("Solução para máquina %d:\n", num_maquinas);
        imprimirTarefasPorMaquina(melhor_solucao, num_tarefas, num_maquinas);
        int makespan = calcularMakespan(melhor_solucao, num_tarefas, num_maquinas, tarefas);
        printf("Solução Final: %d\n", makespan);
        printf("\n");
    }

    imprimirPrecedenciasDiretasEIndiretas(tarefas, num_tarefas);
    free(melhor_solucao);
}

int main()
{
    const char *arquivos_instancias[] = {"KILBRID.IN2", "KILBRID.IN2", "KILBRID.IN2", "KILBRID.IN2", "KILBRID.IN2"};
    const int num_instancias = sizeof(arquivos_instancias) / sizeof(arquivos_instancias[0]);
    const int num_execucoes = 5;
    const int tempo_limite = 30; // 2 minutos em segundos

    for (int instancia = 0; instancia < num_instancias; instancia++)
    {
        const char *arquivo_instancia = arquivos_instancias[instancia];

        for (int execucao = 0; execucao < num_execucoes; execucao++)
        {
            printf("Execução %d da instância %s:\n", execucao + 1, arquivo_instancia);
            executarGRASP(arquivo_instancia, tempo_limite);
            printf("\n");
        }
    }

    return 0;
}
