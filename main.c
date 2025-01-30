#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_CIDADES 20
#define MAX_NOME 30

// Função para embaralhar uma rota aleatoriamente
void gerarRotaAleatoria(int rota[], int num_cidades) {
    for (int i = 0; i < num_cidades; i++) {
        rota[i] = i; // Inicializa a rota em ordem
    }

    // Embaralha a rota
    srand(time(NULL)); // Inicializa o gerador de números aleatórios
    for (int i = num_cidades - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = rota[i];
        rota[i] = rota[j];
        rota[j] = temp;
    }
}

// Função para ler as cidades de um arquivo
void leiaCidades(char cidades[MAX_CIDADES][MAX_NOME], FILE* arquivo, int* count) {
    *count = 0;
    while (fgets(cidades[*count], MAX_NOME, arquivo) != NULL && *count < MAX_CIDADES) {
        cidades[*count][strcspn(cidades[*count], "\n")] = '\0'; // Remove o '\n'
        (*count)++;
    }
}

// Função para obter a posição de uma cidade pelo nome
int posicaoCidade(char cidades[MAX_CIDADES][MAX_NOME], char* cidade) {
    for (int i = 0; i < MAX_CIDADES; i++) {
        if (strcmp(cidades[i], cidade) == 0) {
            return i;
        }
    }
    return -1; // Retorna -1 se não encontrar
}

// Função para ler as rotas e preencher a matriz de custos
void leiaRotas(int matriz_custo[MAX_CIDADES][MAX_CIDADES], char cidades[MAX_CIDADES][MAX_NOME], FILE* arquivo) {
    char linha[256];
    char origem[MAX_NOME];
    char destino[MAX_NOME];
    int custo;

    fgets(linha, sizeof(linha), arquivo); // Ignora o cabeçalho

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        char *token = strtok(linha, ",");
        strcpy(origem, token);

        token = strtok(NULL, ",");
        strcpy(destino, token);

        token = strtok(NULL, ",");
        custo = atoi(token);

        int posOrigem = posicaoCidade(cidades, origem);
        int posDestino = posicaoCidade(cidades, destino);

        if (posOrigem != -1 && posDestino != -1) {
            matriz_custo[posOrigem][posDestino] = custo;
            matriz_custo[posDestino][posOrigem] = custo; // Matriz simétrica
        }
        
    }
}

// Função para calcular o custo total de uma rota
int calcularCustoRota(int rota[], int matrizCusto[MAX_CIDADES][MAX_CIDADES], int num_cidades) {
    int custoTotal = 0;
    for (int i = 0; i < num_cidades - 1; i++) {
        int origem = rota[i];
        int destino = rota[i + 1];
        custoTotal += matrizCusto[origem][destino];
    } 
    printf("%d --> %d  = %d\n", origem, destino, matrizCusto[origem][destino]);
    // Retorna ao ponto inicial
    custoTotal += matrizCusto[rota[num_cidades - 1]][rota[0]];
    return custoTotal;
}

// Função para inverter um segmento da rota
void inverterSegmento(int rota[], int inicio, int fim) {
    while (inicio < fim) {
        int temp = rota[inicio];
        rota[inicio] = rota[fim];
        rota[fim] = temp;
        inicio++;
        fim--;
    }
}

// Função para otimizar a rota com o algoritmo 2-opt
void otimizar2opt(int rota[], int matrizCusto[MAX_CIDADES][MAX_CIDADES], int num_cidades) {
    int melhorou = 1;
    while (melhorou) {
        melhorou = 0;
        int custoAtual = calcularCustoRota(rota, matrizCusto, num_cidades);

        for (int i = 1; i < num_cidades - 2; i++) {
            for (int j = i + 1; j < num_cidades - 1; j++) {
                int rotaTemp[MAX_CIDADES];
                memcpy(rotaTemp, rota, num_cidades * sizeof(int));

                inverterSegmento(rotaTemp, i, j);
                int novoCusto = calcularCustoRota(rotaTemp, matrizCusto, num_cidades);

                if (novoCusto < custoAtual) {
                    memcpy(rota, rotaTemp, num_cidades * sizeof(int));
                    melhorou = 1;
                    break;
                }
            }
            if (melhorou) break;
        }
    }
}

int main() {
    FILE* arquivo;
    char *arquivoCidades = "cidades.txt";
    char *arquivoCustos = "custos.txt";

    char cidades[MAX_CIDADES][MAX_NOME];
    int matrizCusto[MAX_CIDADES][MAX_CIDADES] = {0};
    int countCidades = 0;

    // Ler as cidades
    arquivo = fopen(arquivoCidades, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s\n", arquivoCidades);
        return 1;
    }
    leiaCidades(cidades, arquivo, &countCidades);
    fclose(arquivo);

    // Ler as rotas
    arquivo = fopen(arquivoCustos, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s\n", arquivoCustos);
        return 1;
    }
    leiaRotas(matrizCusto, cidades, arquivo);
    fclose(arquivo);

    // Gerar uma rota aleatória
    int rota[MAX_CIDADES];
    gerarRotaAleatoria(rota, countCidades);

    // Calcular o custo da rota inicial
    printf("Rota inicial:\n");
    for (int i = 0; i < countCidades; i++) {
        printf("%s\n ", cidades[rota[i]]);
    }
    printf("\nCusto inicial: %d\n", calcularCustoRota(rota, matrizCusto, countCidades));

    // Otimizar a rota
    otimizar2opt(rota, matrizCusto, countCidades);

    // Calcular o custo da rota otimizada
    printf("\nRota otimizada:\n");
    for (int i = 0; i < countCidades; i++) {
        printf("%s\n ", cidades[rota[i]]);
    }
    printf("\nCusto otimizado: %d\n", calcularCustoRota(rota, matrizCusto, countCidades));

    

    return 0;
}
