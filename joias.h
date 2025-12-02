#ifndef JOIAS_H
#define JOIAS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TAM_CATEGORIA 50
#define TAM_GENERO 10
#define TAM_COR 20
#define TAM_MATERIAL 20
#define TAM_GEMSTONE 20
#define TAM_TIMESTAMP 30

// estrutura para joias
typedef struct {
    long long id_produto;
    long long id_categoria;
    char alias_categoria[TAM_CATEGORIA];
    long long id_marca;
    double preco;
    char genero[TAM_GENERO];
    char cor[TAM_COR];
    char material[TAM_MATERIAL];
    char pedra_preciosa[TAM_GEMSTONE];
    char quebra_linha;
} Joia;

// estrutura para compras
typedef struct {
    long long id_pedido;
    long long id_produto;
    int quantidade;
    long long id_usuario;
    char data_hora[TAM_TIMESTAMP];
    char quebra_linha;
} Compra;

// estrutura para indice parcial
typedef struct {
    long long chave;
    long posicao;
} IndiceParcial;

//estrutura para hash
typedef struct HashNode {
    long long chaveBusca;    // ex: id_cliente, quantidade, etc.
    long posicao;            // posição no arquivo de dados
    struct HashNode* next;
} HashNode;
#define TAM_HASH 100003  // grande e primo → menos colisões

typedef struct {
    HashNode* tabela[TAM_HASH];
} HashTable;

// estrutura para arvore B (simples)
#define ORDEM_ARVORE_B 5  // ordem da arvore (maximo de chaves por no = ORDEM-1)

// no da arvore B (todos os nos tem a mesma estrutura)
typedef struct NoArvoreB {
    bool eh_folha;                          // true se for no folha, false se for interno
    int quantidade_chaves;                  // numero de chaves atualmente no no
    long long chaves[ORDEM_ARVORE_B - 1];   // array de chaves (id_produto)
    
    // ponteiros para filhos (usado apenas em nos internos)
    struct NoArvoreB* filhos[ORDEM_ARVORE_B];
    
    // posicoes no arquivo de dados (usado apenas em nos folha)
    long posicoes_arquivo[ORDEM_ARVORE_B - 1];
} NoArvoreB;

// estrutura da arvore B completa
typedef struct {
    NoArvoreB* raiz;                        // ponteiro para a raiz da arvore
    int total_registros;                    // total de registros indexados
} ArvoreB;


// funcoes para joias
int inserir_joias_do_csv(const char* arquivo_csv, const char* arquivo_dados);
int mostrar_joias(const char* arquivo_dados);
int buscar_joia_binaria(const char* arquivo_dados, long long id_produto, Joia* joia);
int consultar_joia(const char* arquivo_dados, long long id_produto);
int criar_indice_joias(const char* arquivo_dados, const char* arquivo_indice);


// funcoes para compras
int inserir_compras_do_csv(const char* arquivo_csv, const char* arquivo_dados);
int mostrar_compras(const char* arquivo_dados);
int buscar_compra_binaria(const char* arquivo_dados, long long id_pedido, Compra* compra);
int consultar_compra(const char* arquivo_dados, long long id_pedido);
int criar_indice_compras(const char* arquivo_dados, const char* arquivo_indice);
void gerar_indice_parcial10(const char* arquivo_dados, const char* arquivo_indice);

// funcoes utilitarias
void preencher_string(char* str, int tamanho);
void limpar_string(char* str, int tamanho);
int contar_registros(const char* arquivo_dados, size_t tamanho_registro);
void ordenar_joias_por_id_produto(Joia* joias, int quantidade);
void ordenar_compras_por_id_pedido(Compra* compras, int quantidade);
int comparar_joias(const void* a, const void* b);
int comparar_compras(const void* a, const void* b);
int comparar_indices(const void* a, const void* b);

// funcoes para visualizar indices
void mostrar_indice_joias(const char* arquivo_indice);
void mostrar_indice_compras(const char* arquivo_indice);
void mostrar_indice_parcial(const char* arquivo_indice);
void inicializar_hash(HashTable* ht);
void hash_inserir(HashTable* ht, long long chaveBusca, long posicao);
void construir_hash_por_coluna(const char* arquivo_dados, HashTable* ht);
HashNode* hash_buscar(HashTable* ht, long long chaveBusca);
void imprimir_resultados(HashNode* lista);

// funcoes para pesquisar por indices
long long pesquisa_por_idx_arqInd(const char* arquivo_indice,long idx);
// insercoes 
int inserir_joia_ordenada(const char* arquivo_dados, const char* arquivo_indice);
int inserir_compra_ordenada(const char* arquivo_dados, const char* arquivo_indice);
//remocoes 
int remove_compra(const char* arquivo_dados, const char* arquivo_indice);
int remove_joia(const char* arquivo_dados, const char* arquivo_indice);

// funcoes para arvore B
NoArvoreB* criar_no_arvore_b(bool eh_folha);
void destruir_arvore_b(NoArvoreB* no);
void inicializar_arvore_b(ArvoreB* arvore);
int buscar_posicao_na_arvore_b(ArvoreB* arvore, long long chave_busca);
void construir_arvore_b_do_arquivo(const char* arquivo_dados, ArvoreB* arvore);
void mostrar_arvore_b(ArvoreB* arvore);
void consultar_joia_por_arvore_b(ArvoreB* arvore, const char* arquivo_dados, long long id_produto);
#endif
