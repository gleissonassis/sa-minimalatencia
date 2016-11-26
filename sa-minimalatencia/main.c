#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define TRUE 1
#define FALSE 0

struct matriz {
    int numero_elementos;
    int** elementos;
};

struct nodo {
    int indice;
    int valor;
};

void ler_arquivo(struct matriz*, char[20]);
void imprimir_caminho(int, int*);
int calcular_custo(struct matriz, int*);
void copiar_caminho(int, int*, int*);
int* construir_caminho_guloso(struct matriz);

void selection_sort(struct nodo *, int);
void linha();

void gerar_vizinho_aleatorio(struct matriz, int*, int*);
void sa(struct matriz, double, double, int*, int, int*);

int rnd(int, int);


//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    int *caminho, *caminho_atual;
    int custo, custo_atual;
    int debug;
    time_t inicio;
    struct matriz m;
    
    //inicializando o algoritimo de geração de numeros aleatorios
    //com uma semente estatica
    srand(10);
    
    /* -- leitura de parâmetros
    //lendo o arquivo da instância
    ler_arquivo(&m, argv[1]);
    
    //lendo os parametros referentes a quantidade de execucoes
    n = atoi(argv[2]);
    
    percentual_inicial = atof(argv[3]);
    percentual_final = atof(argv[4]);
    
    debug = atoi(argv[5]);
    */
    
    ///* -- configurações de teste
     ler_arquivo(&m, "/Users/gleissonassis/Dropbox/Mestrado/Implementações/minima-latencia/grasp-minimalatencia/instancias/60_1_100_1000.txt");
     
    
     debug = 1;
     //*/
    
    if(debug) {
        printf("Instancia : %s\n\n", argv[1]);
    }
    
    
    //alocando memória para o caminho que será criado
    caminho = malloc((m.numero_elementos + 1) * sizeof(int));
    caminho_atual = malloc((m.numero_elementos + 1) * sizeof(int));
    
    //inicializando a contagem de tempo
    inicio = time(NULL);
    
    caminho = construir_caminho_guloso(m);
    custo = calcular_custo(m, caminho);
    custo_atual = custo;
    
    if(debug) {
        printf("Caminho (construção gulosa) = ");
        imprimir_caminho(m.numero_elementos + 1, caminho);
        printf("Custo = %d\n", calcular_custo(m, caminho));
        printf("Tempo gasto : %d\n\n",  (int)(time(NULL) - inicio));
    }
    
    sa(m, 0.001, 100000, caminho, 100, caminho);
    
    if(debug) {
        printf("Caminho = ");
        imprimir_caminho(m.numero_elementos + 1, caminho);
        printf("Custo = %d\n", calcular_custo(m, caminho));
        printf("Tempo gasto : %d",  (int)(time(NULL) - inicio));
        
        linha();
    } else {
        imprimir_caminho(m.numero_elementos + 1, caminho);
        printf("%d\n%d\n", custo, (int)(time(NULL) - inicio));
    }
    
    //liberando memoria para a matriz de adjacencia e os caminhos
    free(caminho);
    free(caminho_atual);
    free(m.elementos);
    
    return 0;
}

int rnd(int min, int max) {
    min = ceil(min);
    max = floor(max);
    
    return floor(rand() % (max + 1 - min)) + min;
}

/*
 * Function: gerar_vizinho_aleatorio
 * -----------------------------------------------------------------------------
 *   Função que perturba uma solução com o objetivo de exporar de forma melhor
 *   o espaço de soluções.
 *
 *   s: estrutura que representa uma construção de solução para o problema.
 */
void gerar_vizinho_aleatorio(struct matriz m, int* caminho, int* caminho_destino) {
    int tmp, i, j;
    
    copiar_caminho(m.numero_elementos + 1, caminho, caminho_destino);
    
    if(rnd(0, 50) > 25) {
        i = rnd(1, m.numero_elementos - 1);
        j = rnd(1, m.numero_elementos - 1);
    
        tmp = caminho_destino[i];
        caminho_destino[i] = caminho_destino[j];
        caminho_destino[j] = tmp;
    } else {
        i = rnd(1, m.numero_elementos - 2);
        
        tmp = caminho_destino[i];
        caminho_destino[i] = caminho_destino[i + 1];
        caminho_destino[i + 1] = tmp;
    }
}

/*
 * Function: sa
 * -----------------------------------------------------------------------------
 *   Implementação da metaheurística Simulated Annealing.
 *
 *   a: fator de redução de temperatura.
 *   T0: temperatura inicial.
 *   s: estrutura que representa uma construção de solução para o problema.
 *   SAmax: quantidade de perturbações em uma iteração do Simulated Annealing
 */
void sa(struct matriz m, double a, double T0, int* solucao_inicial, int perturbarcoes, int* s_estrela) {
    int* s;
    int* sl;
    
    int IterT = 0;
    double T = T0;
    double delta = 0;
    
    //alocando memória para o caminho que será criado
    s = malloc((m.numero_elementos + 1) * sizeof(int));
    sl = malloc((m.numero_elementos + 1) * sizeof(int));
    
    //copiando para as soluções temporárias a solução inicial passada como
    //parâmetro
    copiar_caminho(m.numero_elementos + 1, solucao_inicial, s_estrela);
    copiar_caminho(m.numero_elementos + 1, solucao_inicial, s);
    copiar_caminho(m.numero_elementos + 1, solucao_inicial, sl);
    
    while(T > 0.0001) {
        while(IterT < perturbarcoes) {
            IterT++;
            
            //A geração de um vizinho aleatório simula a perturbação de uma
            //solução
            gerar_vizinho_aleatorio(m, s, sl);
            
            int fsl = calcular_custo(m, sl);
            int fs = calcular_custo(m, s);
            delta = fsl - fs;
            
            //se a solução tem um custo melhor que a solução corrente ela é
            //automaticamente aceita. Observe que a solução corrente não é
            //necessáriamente a melhor solução (s_estrela)
            if(delta < 0) {
                copiar_caminho(m.numero_elementos + 1, sl, s);
                
                int fs_estrela = calcular_custo(m, s_estrela);
                
                //Se a solução corrente é melhor de todos até o momento ela
                //passa ser s_estrela
                if(fsl < fs_estrela) {
                    copiar_caminho(m.numero_elementos + 1, sl, s_estrela);
                }
            } else {
                double x = rand() / RAND_MAX;
                
                //aceitando uma solução de piora dada a função de probabilidade
                if(x < pow(M_E, -delta / T)) {
                    copiar_caminho(m.numero_elementos + 1, sl, s);
                }
            }
        }
        
        //Reduzindo a temperatura através do fator de redução e zerando a contagem
        //de perturbações
        IterT = 0;
        T = T - (a * T);
    }
    
    copiar_caminho(m.numero_elementos + 1, s_estrela, s);
    
    free(s);
    free(sl);
}


//-----------------------------------------------------------------------------

int* construir_caminho_guloso(struct matriz m) {
    int iv;
    int *inserido;
    struct nodo *vizinhos;
    int* caminho;
    
    //alocando memória para o caminho que será retornado
    caminho = malloc((m.numero_elementos + 1) * sizeof(int));
    
    //alocando memoria para o array que ira informar se um elemento
    //ja foi inserido no caminho ou nao e inicializando os valores
    inserido = malloc(m.numero_elementos * sizeof(int));
    for(int i = 0; i < m.numero_elementos; i++) {
        inserido[i] = FALSE;
    }
    
    //alocando memoria para o array que irá armazenar informações sobre a vizinhança
    //do elemento atual
    vizinhos = (struct nodo*) malloc((m.numero_elementos) * sizeof(struct nodo));
    
    caminho[0] = 0;
    inserido[0] = TRUE;
    
    for(int i = 0; i < m.numero_elementos; i++) {
        //indice do vizinho atual;
        iv = 0;
        
        //construindo a lista de vizinhos e seus respectivos valores
        for(int j = 0; j < m.numero_elementos; j++) {
            //nao pode ser selecionado um elemento que ja esteja no caminho
            if(!inserido[j]) {
                vizinhos[iv].indice = j;
                vizinhos[iv].valor = m.elementos[i][j];
                
                iv++;
            }
        }
        
        if(iv == 0) {
            caminho[i + 1] = 0;
        } else {
            //ordenando a lista de vizinhos por custo da aresta e selecionando
            //o vizinho com menor custo (construção gulosa)
            selection_sort(vizinhos, iv);
            
            caminho[i + 1] = vizinhos[0].indice;
            inserido[vizinhos[0].indice] = TRUE;
            
        }
    }
    
    //liberando memoria alocada para os arrays
    free(inserido);
    free(vizinhos);
    
    return caminho;
}

//-----------------------------------------------------------------------------

void ler_arquivo(struct matriz* m, char arquivo[2000]) {
    FILE* fp;
    int t;
    
    fp = fopen(arquivo, "r");
    fscanf(fp, "%d %d\n\n", &m->numero_elementos, &t);
    
    //alocando espaço para a matriz de adjacencia
    m->elementos = malloc(m->numero_elementos * m->numero_elementos * sizeof(int));
    
    
    //pulando as linhas de 1s
    for(int i = 0; i < m->numero_elementos; i++) {
        fscanf(fp, "%d ", &t);
    }
    
    //percorrendo os elementos da matriz de ajdacencia que estao no arquivo
    for(int i = 0; i < m->numero_elementos; i++) {
        m->elementos[i] = malloc(m->numero_elementos * sizeof(int));
        
        for(int j = 0; j < m->numero_elementos; j++) {
            m->elementos[i][j] = 0;
            fscanf(fp, "%d ", &m->elementos[i][j]);
        }
    }
}

//-----------------------------------------------------------------------------

void selection_sort(struct nodo *array, int n) {
    int i, j;
    int min;
    struct nodo temp;
    
    for(i = 0; i < n - 1; i++) {
        min=i;
        for(j = i + 1; j < n; j++) {
            if(array[j].valor < array[min].valor) {
                min = j;
            }
        }
        
        temp = array[i];
        array[i] = array[min];
        array[min] = temp;
    }
}

//-----------------------------------------------------------------------------

void copiar_caminho(int n, int* origem, int* destino) {
    int i;
    
    for(i = 0; i < n; i++) {
        destino[i] = origem[i];
    }
}

//-----------------------------------------------------------------------------

int calcular_custo(struct matriz m, int* caminho) {
    int custo = 0;
    int i;
    
    for(i = 0; i < m.numero_elementos; i++) {
        custo += m.elementos[caminho[i]][caminho[i + 1]] * (m.numero_elementos - i);
    }
    
    return custo;
}

//-----------------------------------------------------------------------------

void imprimir_caminho(int n, int* caminho) {
    int i;
    
    for(i = 0; i < n; i++) {
        printf("%d ", caminho[i]);
    }
    printf("\n");
}

//-----------------------------------------------------------------------------

void linha() {
    int i;
    printf("\n");
    for(i = 0; i < 80; i++) printf("_");
    printf("\n");
}
