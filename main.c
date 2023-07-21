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
        int **tarefas_por_maquina; 
        int num_maquinas;          
        int makespan;             
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

   int *obterPrecedencias(int tarefa, int *num_precedencias)
{
    int *precedencias = malloc(MAX_TAREFAS * sizeof(int));
    *num_precedencias = 0;

    // Função auxiliar para adicionar a tarefa ao vetor de precedências
    void adicionarPrecedencia(int tarefa)
    {
        // Verifica se a tarefa já está no vetor de precedências
        for (int i = 0; i < *num_precedencias; i++)
        {
            if (precedencias[i] == tarefa)
            {
                return;
            }
        }

        // Adiciona a tarefa ao vetor de precedências
        precedencias[*num_precedencias] = tarefa;
        (*num_precedencias)++;
    }

    // Função recursiva para obter as precedências diretas e indiretas
    void obterPrecedenciasRecursivo(int tarefa)
    {
        for (int i = 0; i < tarefas[tarefa].num_predecessores; i++)
        {
            int precedencia = tarefas[tarefa].precedencia[i];
            adicionarPrecedencia(precedencia);
            obterPrecedenciasRecursivo(precedencia);
        }
    }

    // Chama a função recursiva para obter as precedências
    obterPrecedenciasRecursivo(tarefa);

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

   // Atualização da função atribuirTarefa
void atribuirTarefa(int tarefa, const int *precedencias, int num_precedencias, bool *alocada, int *ordem, int *indice_ordem, int *makespan, double alpha) {
    if (alocada[tarefa]) {
        return;
    }

    if (!todasPrecedenciasAlocadas(tarefa, precedencias, num_precedencias, alocada)) {
        return;
    }

    alocada[tarefa] = true;
    ordem[(*indice_ordem)++] = tarefa;

    // Atualizar makespan da máquina correspondente usando o valor alpha
    int maquina = (*indice_ordem - 1) % MAX_MAQUINAS;
    *makespan += tarefas[tarefa].custo * alpha;

    for (int i = 0; i < num_tarefas; i++) {
        if (tarefas[i].num_predecessores > 0) {
            int *precedencias_indiretas = obterPrecedencias(i, &num_precedencias);
            atribuirTarefa(i, precedencias_indiretas, num_precedencias, alocada, ordem, indice_ordem, makespan, alpha);
            free(precedencias_indiretas);
        }
    }
}

// Atualização da função criarOrdemAtribuicao
int *criarOrdemAtribuicao(const Tarefa *tarefas, int num_tarefas, int *makespan, double alpha) {
    bool alocada[MAX_TAREFAS] = {false};
    int *ordem = (int *)malloc(num_tarefas * sizeof(int));
    int indice_ordem = 0;
    *makespan = 0;

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

    // Atribuir as tarefas às máquinas usando a função atribuirTarefa com alpha
    for (int i = 0; i < num_tarefas; i++) {
        int tarefa = indices_embaralhados[i];
        int num_precedencias;
        int *precedencias = obterPrecedencias(tarefa, &num_precedencias);

        // Chamar a função atribuirTarefa com os argumentos corretos, incluindo alpha
        atribuirTarefa(tarefa, precedencias, num_precedencias, alocada, ordem, &indice_ordem, makespan, alpha);

        free(precedencias);
    }

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

    void removerTarefa(int *ordem, int indice, int *num_tarefas)
    {
        for (int i = indice; i < (*num_tarefas) - 1; i++)
        {
            ordem[i] = ordem[i + 1];
        }
        (*num_tarefas)--;
    }

    int **atribuirTarefasPorMaquinas(const int *ordem, int num_tarefas, int num_maquinas)
    {
        // Aloca a matriz para armazenar os índices das tarefas por máquina
        int **tarefas_por_maquina_matriz = (int **)malloc(num_maquinas * sizeof(int *));
        for (int i = 0; i < num_maquinas; i++)
        {
            tarefas_por_maquina_matriz[i] = (int *)malloc(num_tarefas * sizeof(int));
        }

        // Calcula a quantidade de tarefas que cada máquina deve receber
       int tarefas_por_maquina = num_tarefas / num_maquinas;
        int tarefas_extras = num_tarefas % num_maquinas;

        int tarefa_inicio = 0;
        for (int i = 0; i < num_maquinas; i++)
        {
            int tarefa_fim = tarefa_inicio + tarefas_por_maquina + (i < tarefas_extras ? 1 : 0);

            for (int j = tarefa_inicio; j < tarefa_fim; j++)
            {
                // Armazena o índice da tarefa na matriz da máquina i
                tarefas_por_maquina_matriz[i][j - tarefa_inicio] = ordem[j];
            }

            tarefa_inicio = tarefa_fim;
        }

        return tarefas_por_maquina_matriz;
    }



    void imprimirTarefasPorMaquina(int **tarefas_por_maquina_matriz, int num_tarefas, int num_maquinas)
    {
        printf("Tarefas alocadas por máquina:\n");
        for (int i = 0; i < num_maquinas; i++)
        {
            printf("Máquina %d:", i + 1);
            int num_tarefas_maquina = num_tarefas / num_maquinas + (i < num_tarefas % num_maquinas ? 1 : 0);
            for (int j = 0; j < num_tarefas_maquina; j++)
            {
                printf(" %d", tarefas_por_maquina_matriz[i][j] + 1);
            }
            printf("\n");
        }
    }

    int calcularMakespan(int **tarefas_por_maquina_matriz, int num_tarefas, int num_maquinas)
    {
        int makespan_total = 0;

        
        for (int i = 0; i < num_maquinas; i++)
        {
            int makespan_maquina = 0;
            int num_tarefas_maquina = num_tarefas / num_maquinas + (i < num_tarefas % num_maquinas ? 1 : 0);

            // Loop para somar os tempos de execução das tarefas na máquina
            for (int j = 0; j < num_tarefas_maquina; j++)
            {
                int indice_tarefa = tarefas_por_maquina_matriz[i][j];
                makespan_maquina += tarefas[indice_tarefa].custo;
            }

            // Atualiza o makespan total se o makespan da máquina for maior
            if (makespan_maquina > makespan_total)
            {
                makespan_total = makespan_maquina;
            }
        }

        return makespan_total;
    }

bool trocaValida(int tarefa1, int tarefa2, int maquina1, int maquina2, int **tarefas_por_maquina_matriz, int num_tarefas, int num_maquinas, int *precedencias) {
    // Verifica se as tarefas estão em máquinas diferentes
    if (maquina1 == maquina2) {
        return false;
    }

    // Verifica se a troca quebra as precedências
    int tarefa1_dependencia = precedencias[tarefa1];
    int tarefa2_dependencia = precedencias[tarefa2];

    // Verifica se a troca quebra a precedência da tarefa1 (tarefa1 deve ser feita após tarefa2_dependencia)
    if (tarefa1_dependencia != -1 && tarefa1_dependencia == tarefa2) {
        return false;
    }

    // Verifica se a troca quebra a precedência da tarefa2 (tarefa2 deve ser feita após tarefa1_dependencia)
    if (tarefa2_dependencia != -1 && tarefa2_dependencia == tarefa1) {
        return false;
    }

    // Se não quebrar nenhuma precedência, a troca é válida
    return true;
}


bool verificarPrecedencias(int **tarefas_por_maquina_matriz, int num_tarefas, int num_maquinas) {
    for (int i = 0; i < num_tarefas; i++) {
        for (int j = 0; j < tarefas[i].num_predecessores; j++) {
            int precedencia = tarefas[i].precedencia[j];

            int maquina_precedencia = -1;
            int maquina_tarefa = -1;

            // Encontra a máquina da tarefa precedente
            for (int m = 0; m < num_maquinas; m++) {
                for (int k = 0; k < num_tarefas / num_maquinas + (m < num_tarefas % num_maquinas ? 1 : 0); k++) {
                    if (tarefas_por_maquina_matriz[m][k] == precedencia) {
                        maquina_precedencia = m;
                        break;
                    }
                }
                if (maquina_precedencia != -1) {
                    break;
                }
            }

            // Encontra a máquina da tarefa atual
            for (int m = 0; m < num_maquinas; m++) {
                for (int k = 0; k < num_tarefas / num_maquinas + (m < num_tarefas % num_maquinas ? 1 : 0); k++) {
                    if (tarefas_por_maquina_matriz[m][k] == i) {
                        maquina_tarefa = m;
                        break;
                    }
                }
                if (maquina_tarefa != -1) {
                    break;
                }
            }

            // Verifica se a precedência é respeitada
            if (maquina_precedencia > maquina_tarefa) {
                return false;
            }
        }
    }

    return true;
}




bool vizinhancaTrocaTarefas(int **tarefas_por_maquina_matriz, int num_tarefas, int num_maquinas, int *makespan, int tamanho_vizinhanca, int *precedencias) {
    bool melhoria_encontrada = false;

    // Percorre todas as máquinas e tarefas para encontrar uma troca válida
    for (int maquina1 = 0; maquina1 < num_maquinas; maquina1++) {
        int num_tarefas_maquina1 = num_tarefas / num_maquinas + (maquina1 < num_tarefas % num_maquinas ? 1 : 0);

        for (int i = 0; i < num_tarefas_maquina1; i++) {
            int tarefa1 = tarefas_por_maquina_matriz[maquina1][i];

            for (int maquina2 = 0; maquina2 < num_maquinas; maquina2++) {
                if (maquina1 != maquina2) {
                    int num_tarefas_maquina2 = num_tarefas / num_maquinas + (maquina2 < num_tarefas % num_maquinas ? 1 : 0);
                    for (int j = 0; j < num_tarefas_maquina2; j++) {
                        int tarefa2 = tarefas_por_maquina_matriz[maquina2][j];
                        // Verifica se a troca é válida (respeita precedências e não repete tarefas)
                        if (trocaValida(tarefa1, tarefa2, maquina1, maquina2, tarefas_por_maquina_matriz, num_tarefas, num_maquinas, precedencias)) {
                            // Realiza a troca de tarefas
                            int temp = tarefas_por_maquina_matriz[maquina1][i];
                            tarefas_por_maquina_matriz[maquina1][i] = tarefas_por_maquina_matriz[maquina2][j];
                            tarefas_por_maquina_matriz[maquina2][j] = temp;
                            // Recalcula o makespan após as trocas
                            int novo_makespan = calcularMakespan(tarefas_por_maquina_matriz, num_tarefas, num_maquinas);
                            // Verifica se a troca respeitou as precedências
                            bool precedencias_respeitadas = verificarPrecedencias(tarefas_por_maquina_matriz, num_tarefas, num_maquinas);
                            if (novo_makespan < *makespan && precedencias_respeitadas) {
                                *makespan = novo_makespan;
                                melhoria_encontrada = true;
                                printf("Novo makespan após a troca: %d\n", novo_makespan);
                               } else {
                                // Desfaz a troca se não houve melhoria ou as precedências não foram respeitadas
                                temp = tarefas_por_maquina_matriz[maquina1][i];
                                tarefas_por_maquina_matriz[maquina1][i] = tarefas_por_maquina_matriz[maquina2][j];
                                tarefas_por_maquina_matriz[maquina2][j] = temp;
                            }
                        }
                    }
                }
            }
        }
    }

    return melhoria_encontrada;
}

int main()
{
   
    lerInstancias("KILBRID.IN2");

    time_t start_time = time(NULL);
    
    int melhor_makespan_total = INT_MAX;
   
    time_t tempo_melhor_solucao = 0;
   
    float tempo_busca_local = 0;
   
    imprimirPrecedenciasDiretasEIndiretas(tarefas, num_tarefas);

    int num_maquinas = 3;

    double melhorTempo = 0;

    // Número de execuções do GRASP
    int num_execucoes = 0;
    int **precedencias = (int **)malloc(num_tarefas * sizeof(int *));
    int **melhor_tarefas_por_maquina_matriz = NULL;
    for (int i = 0; i < num_tarefas; i++)
    {
        int num_precedencias;
        precedencias[i] = obterPrecedencias(i, &num_precedencias);
    }
    
    int tempo_limite_segundos = 120;

     while (difftime(time(NULL), start_time) < tempo_limite_segundos)
    {
        clock_t start_time_iteracao = clock();
        num_execucoes++;
        printf("\nExecutando GRASP - Execução %d\n", num_execucoes);

        double alpha = 0.9 / num_execucoes;
        // Criar ordem inicial de atribuição usando construção gulosa com aleatoriedade (alpha = 0.9)
        int makespan;
        int *ordem = criarOrdemAtribuicao(tarefas, num_tarefas, &makespan, 0.9);

       
        imprimirOrdemAtribuicao(ordem, num_tarefas);

        int **tarefas_por_maquina_matriz = atribuirTarefasPorMaquinas(ordem, num_tarefas, num_maquinas);
        imprimirTarefasPorMaquina(tarefas_por_maquina_matriz, num_tarefas, num_maquinas);

        // Executar busca local para melhorar a solução aleatoriedade
        int makespan_total = calcularMakespan(tarefas_por_maquina_matriz, num_tarefas, num_maquinas);
        printf("Makespan Inicial: %d\n", makespan_total);
        
        // Combinação de alfa para maior 
        int tamanho_vizinhanca = num_tarefas * 0.2;
        for (int iteracao = 1; iteracao <= 10; iteracao++){

            clock_t start_time_busca_local = clock();
            printf("\nExecutando Vizinhança - Iteração %d\n", iteracao);
            bool melhoria_encontrada = vizinhancaTrocaTarefas(tarefas_por_maquina_matriz, num_tarefas, num_maquinas, &makespan_total, tamanho_vizinhanca, precedencias);

            
            if (melhoria_encontrada){
                
            printf("Melhoria encontrada na Iteração  %d - Novo Makespan: %d\n", iteracao, calcularMakespan(tarefas_por_maquina_matriz, num_tarefas, num_maquinas));
            clock_t end_time_busca_local = clock();
            tempo_busca_local = (tempo_busca_local + (float)(end_time_busca_local - start_time_busca_local) / CLOCKS_PER_SEC);
            imprimirTarefasPorMaquina(tarefas_por_maquina_matriz, num_tarefas, num_maquinas);
            printf("Tempo da Iteração de Busca Local: %.3f segundos\n", tempo_busca_local);
            }
            else
            {
                printf("Nenhuma melhoria encontrada na Iteração %d\n", iteracao);
              
                // Encerra o laço da busca local se nenhuma melhoria for encontrada
            }
        }

        // Verificar se a solução atual é a melhor encontrada até o momento
    if (makespan_total < melhor_makespan_total)
    {
        melhor_makespan_total = makespan_total;
        tempo_melhor_solucao = time(NULL);
        
        if (melhor_tarefas_por_maquina_matriz != NULL)
        {
            for (int i = 0; i < num_maquinas; i++)
            {
                free(melhor_tarefas_por_maquina_matriz[i]);
            }
            free(melhor_tarefas_por_maquina_matriz);
        }

       
        melhor_tarefas_por_maquina_matriz = (int **)malloc(num_maquinas * sizeof(int *));
        for (int i = 0; i < num_maquinas; i++)
        {
            melhor_tarefas_por_maquina_matriz[i] = (int *)malloc(num_tarefas * sizeof(int));
            memcpy(melhor_tarefas_por_maquina_matriz[i], tarefas_por_maquina_matriz[i], num_tarefas * sizeof(int));
        }
    }
        clock_t end_time_iteracao = clock();
        float tempo_iteracao = (float)(end_time_iteracao - start_time_iteracao) / CLOCKS_PER_SEC;
        printf("Tempo da Iteração %d: %.3f segundos\n", num_execucoes, tempo_iteracao);
        
        for (int i = 0; i < num_maquinas; i++)
        {
            free(tarefas_por_maquina_matriz[i]);
        }
        free(tarefas_por_maquina_matriz);
        free(ordem);
    }

    
    printf("\nMelhor Solução:\n");
    imprimirTarefasPorMaquina(melhor_tarefas_por_maquina_matriz, num_tarefas, num_maquinas);
    printf("Makespan total da Melhor Solução: %d\n", melhor_makespan_total);
    double tempo_para_melhor_solucao = difftime(tempo_melhor_solucao, start_time);
    printf("Tempo para atingir a Melhor Solução: %.2f segundos\n", tempo_para_melhor_solucao);
        
    for (int i = 0; i < num_maquinas; i++)
    {
        free(melhor_tarefas_por_maquina_matriz[i]);
    }
    free(melhor_tarefas_por_maquina_matriz);


    return 0;
}