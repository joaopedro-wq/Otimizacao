#ifndef MAIN_H
#define MAIN_H

#define MAX_TAREFAS 100
#define MAX_ESTACOES 11

typedef struct {
    int num_tarefas;
    int num_estacoes;
    int custo_tarefas[MAX_TAREFAS];
    int precedencias[MAX_TAREFAS][MAX_TAREFAS];
} Instancia;

typedef struct {
    int estacao[MAX_TAREFAS];
    int tempo_conclusao[MAX_TAREFAS];
    int makespan;
} Solucao;

void ler_instancia(const char *nome_arquivo, Instancia *instancia);
int verificar_precedencia(const Instancia *instancia, const Solucao *solucao, int tarefa, int estacao);
void atribuir_tarefas(const Instancia *instancia, Solucao *solucao);

int calcular_makespan(const Instancia *instancia, const Solucao *solucao);

void busca_local(const Instancia *instancia, Solucao *solucao) ;
void resolver_instancia(const Instancia *instancia, Solucao *solucao);
void busca_local(const Instancia *instancia, Solucao *solucao);
void escrever_solucao(const Instancia *instancia, const Solucao *solucao, const char *nome_arquivo_saida);
void encontrar_menor_makespan(const Instancia *instancia, Solucao *solucao);
void imprimir_solucao(const Instancia *instancia, const Solucao *solucao);
double calcular_tempo_execucao(clock_t inicio, clock_t fim);

#endif  // SCHEDULING_H