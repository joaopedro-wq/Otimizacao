#include <stdio.h>
#include <stdlib.h>

#define MAX_TASKS 100
#define MAX_MACHINES 100
#define MAX_COST 1000000

typedef struct {
    int num_tasks;
    int num_machines;
    int task_cost[MAX_TASKS];
    int precedences[MAX_TASKS][MAX_TASKS];
} Instance;

typedef struct {
    int machine[MAX_TASKS];
    int makespan;
} Solution;

void read_instance(const char *filename, Instance *instance) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        exit(1);
    }

    fscanf(file, "%d", &instance->num_tasks);

    for (int i = 0; i < instance->num_tasks; i++) {
        fscanf(file, "%d", &instance->task_cost[i]);
    }

    for (int i = 0; i < instance->num_tasks; i++) {
        for (int j = 0; j < instance->num_tasks; j++) {
            fscanf(file, "%d", &instance->precedences[i][j]);
        }
    }

    fclose(file);
}

void print_solution(const Instance *instance, const Solution *solution) {
    printf("Solução Inicial:\n");
    for (int t = 0; t < instance->num_tasks; t++) {
        printf("Tarefa %d: Estação %d\n", t + 1, solution->machine[t] + 1);
    }
    printf("Makespan: %d\n", solution->makespan);
    printf("\n");

    printf("Solução Final:\n");
    for (int t = 0; t < instance->num_tasks; t++) {
        printf("Tarefa %d: Estação %d\n", t + 1, solution->machine[t] + 1);
    }
    printf("Makespan Final: %d\n", solution->makespan);
}

void calculate_makespan(const Instance *instance, const Solution *solution, int *makespan) {
    int machine_times[MAX_MACHINES] = {0};

    for (int t = 0; t < instance->num_tasks; t++) {
        int task_machine = solution->machine[t];
        int task_cost = instance->task_cost[t];
        int machine_time = machine_times[task_machine];

        if (machine_time > *makespan) {
            *makespan = machine_time;
        }

        machine_times[task_machine] += task_cost;
    }
}

void solve_instance(const Instance *instance, Solution *solution) {
    solution->makespan = 0;

    // Lógica para atribuir as tarefas às máquinas
    int num_machines = instance->num_machines;
    int tasks_per_machine = instance->num_tasks / num_machines;
    int remaining_tasks = instance->num_tasks % num_machines;
    int task_counter = 0;

    for (int m = 0; m < num_machines; m++) {
        for (int i = 0; i < tasks_per_machine; i++) {
            solution->machine[task_counter] = m;
            task_counter++;
        }

        if (remaining_tasks > 0) {
            solution->machine[task_counter] = m;
            task_counter++;
            remaining_tasks--;
        }
    }

    calculate_makespan(instance, solution, &solution->makespan);
}

void write_solution(const char *filename, const Instance *instance, const Solution *solution) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Erro ao criar o arquivo de saída.\n");
        return;
    }

    fprintf(file, "Solução Inicial:\n");
    for (int t = 0; t < instance->num_tasks; t++) {
        fprintf(file, "Tarefa %d: Estação %d\n", t + 1, solution->machine[t] + 1);
    }
    fprintf(file, "Makespan: %d\n", solution->makespan);
    fprintf(file, "\n");

    fprintf(file, "Solução Final:\n");
    for (int t = 0; t < instance->num_tasks; t++) {
        fprintf(file, "Tarefa %d: Estação %d\n", t + 1, solution->machine[t] + 1);
    }
    fprintf(file, "Makespan Final: %d\n", solution->makespan);

    fclose(file);
}

int main() {
    Instance instance;
    Solution solution;

    const char *filename = "KILBRID.IN2";
    read_instance(filename, &instance);

    printf("Informe o número de máquinas (entre 3 e 11): ");
    scanf("%d", &instance.num_machines);

    if (instance.num_machines < 3 || instance.num_machines > 11) {
        printf("Número de máquinas inválido. O valor deve estar entre 3 e 11.\n");
        return 1;
    }

    solve_instance(&instance, &solution);

    print_solution(&instance, &solution);
    write_solution("solucao.txt", &instance, &solution);

    return 0;
}
