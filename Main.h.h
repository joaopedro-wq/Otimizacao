#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

#define MAX_TAREFAS 1000
#define MAX_MAQUINAS 11

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

void lerInstancias(const char *arquivo);
void imprimirPrecedencias(const Tarefa *tarefas, int num_tarefas);
int *obterPrecedencias(int tarefa, int *num_precedencias);
void imprimirPrecedenciasDiretasEIndiretas(const Tarefa *tarefas, int num_tarefas);
bool todasPrecedenciasAlocadas(int tarefa, const int *precedencias, int num_precedencias, const bool *alocada);
void atribuirTarefa(int tarefa, const int *precedencias, int num_precedencias, bool *alocada, int *ordem, int *indice_ordem, int *makespan);
int *criarOrdemAtribuicao(const Tarefa *tarefas, int num_tarefas, int *makespan);
void imprimirOrdemAtribuicao(const int *ordem, int num_tarefas);
void atribuirTarefasPorMaquinas(const int *ordem, int num_tarefas, int num_maquinas);
void imprimirTarefasPorMaquina(const int *ordem, int num_tarefas, int num_maquinas);

#endif /* MAIN_H */
