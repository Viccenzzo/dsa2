#include "joias.h"

// funcoes utilitarias
void preencher_string(char* str, int tamanho) {
    int len = strlen(str);
    for (int i = len; i < tamanho - 1; i++) {
        str[i] = ' ';
    }
    str[tamanho - 1] = '\0';
}

void limpar_string(char* str, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        if (str[i] == '\n' || str[i] == '\r') {
            str[i] = '\0';
            break;
        }
    }
}

int contar_registros(const char* arquivo_dados, size_t tamanho_registro) {
    FILE* arquivo = fopen(arquivo_dados, "rb");
    if (!arquivo) return 0;
    
    fseek(arquivo, 0, SEEK_END);
    long tamanho = ftell(arquivo);
    fclose(arquivo);
    
    return (int)(tamanho / tamanho_registro);
}

// funcoes de ordenacao
int comparar_joias(const void* a, const void* b) {
    const Joia* joia_a = (const Joia*)a;
    const Joia* joia_b = (const Joia*)b;
    return (joia_a->id_produto > joia_b->id_produto) - (joia_a->id_produto < joia_b->id_produto);
}

int comparar_compras(const void* a, const void* b) {
    const Compra* compra_a = (const Compra*)a;
    const Compra* compra_b = (const Compra*)b;
    return (compra_a->id_pedido > compra_b->id_pedido) - (compra_a->id_pedido < compra_b->id_pedido);
}

void ordenar_joias_por_id_produto(Joia* joias, int quantidade) {
    qsort(joias, quantidade, sizeof(Joia), comparar_joias);
}

void ordenar_compras_por_id_pedido(Compra* compras, int quantidade) {
    qsort(compras, quantidade, sizeof(Compra), comparar_compras);
}

int comparar_indices(const void* a, const void* b) {
    const IndiceParcial* indice_a = (const IndiceParcial*)a;
    const IndiceParcial* indice_b = (const IndiceParcial*)b;
    return (indice_a->chave > indice_b->chave) - (indice_a->chave < indice_b->chave);
}

// funcoes para joias
int inserir_joias_do_csv(const char* arquivo_csv, const char* arquivo_dados) {
    FILE* csv = fopen(arquivo_csv, "r");
    if (!csv) {
        printf("Erro ao abrir arquivo CSV: %s\n", arquivo_csv);
        return -1;
    }
    
    FILE* dados = fopen(arquivo_dados, "wb");
    if (!dados) {
        printf("Erro ao criar arquivo de dados: %s\n", arquivo_dados);
        fclose(csv);
        return -1;
    }
    
    char linha[1000];
    Joia* joias = NULL;
    int capacidade = 1000;
    int quantidade = 0;
    
    joias = (Joia*)malloc(capacidade * sizeof(Joia));
    if (!joias) {
        printf("Erro de memoria\n");
        fclose(csv);
        fclose(dados);
        return -1;
    }
    
    // O CSV nao tem cabecalho, comeca diretamente com os dados
    // Voltar para o inicio do arquivo
    fseek(csv, 0, SEEK_SET);
    
    while (fgets(linha, sizeof(linha), csv)) {
        if (quantidade >= capacidade) {
            capacidade *= 2;
            joias = (Joia*)realloc(joias, capacidade * sizeof(Joia));
            if (!joias) {
                printf("Erro de memoria\n");
                fclose(csv);
                fclose(dados);
                return -1;
            }
        }
        
        // Parse da linha CSV - ordem correta:
        // Order datetime, Order ID, Product ID, Quantity, Category ID, Category alias, Brand ID, Price, User ID, Gender, Color, Material, Gemstone
        char* token = strtok(linha, ",");
        if (!token) continue;
        
        // Pular Order datetime (já lido acima)
        token = strtok(NULL, ","); // Order ID
        if (!token) continue;
        
        token = strtok(NULL, ","); // Product ID
        if (!token) continue;
        joias[quantidade].id_produto = atoll(token);
        
        token = strtok(NULL, ","); // Quantity
        if (!token) continue;
        
        token = strtok(NULL, ","); // Category ID
        if (!token) continue;
        joias[quantidade].id_categoria = atoll(token);
        
        token = strtok(NULL, ","); // category_alias
        if (!token) continue;
        strncpy(joias[quantidade].alias_categoria, token, TAM_CATEGORIA - 1);
        joias[quantidade].alias_categoria[TAM_CATEGORIA - 1] = '\0';
        preencher_string(joias[quantidade].alias_categoria, TAM_CATEGORIA);
        
        token = strtok(NULL, ","); // brand_id
        if (!token) continue;
        joias[quantidade].id_marca = atoll(token);
        
        token = strtok(NULL, ","); // price
        if (!token) continue;
        joias[quantidade].preco = atof(token);
        
        token = strtok(NULL, ","); // user_id
        token = strtok(NULL, ","); // gender
        if (!token) continue;
        strncpy(joias[quantidade].genero, token, TAM_GENERO - 1);
        joias[quantidade].genero[TAM_GENERO - 1] = '\0';
        preencher_string(joias[quantidade].genero, TAM_GENERO);
        
        token = strtok(NULL, ","); // color
        if (!token) continue;
        strncpy(joias[quantidade].cor, token, TAM_COR - 1);
        joias[quantidade].cor[TAM_COR - 1] = '\0';
        preencher_string(joias[quantidade].cor, TAM_COR);
        
        token = strtok(NULL, ","); // material
        if (!token) continue;
        strncpy(joias[quantidade].material, token, TAM_MATERIAL - 1);
        joias[quantidade].material[TAM_MATERIAL - 1] = '\0';
        preencher_string(joias[quantidade].material, TAM_MATERIAL);
        
        token = strtok(NULL, ","); // gemstone
        if (token) {
            // Remover quebra de linha se existir
            char* newline = strchr(token, '\n');
            if (newline) *newline = '\0';
            strncpy(joias[quantidade].pedra_preciosa, token, TAM_GEMSTONE - 1);
            joias[quantidade].pedra_preciosa[TAM_GEMSTONE - 1] = '\0';
        } else {
            limpar_string(joias[quantidade].pedra_preciosa, TAM_GEMSTONE);
        }
        preencher_string(joias[quantidade].pedra_preciosa, TAM_GEMSTONE);
        
        joias[quantidade].quebra_linha = '\n';
        quantidade++;
    }
    
    // Ordenar por id_produto
    ordenar_joias_por_id_produto(joias, quantidade);
    
    // Escrever no arquivo binario
    for (int i = 0; i < quantidade; i++) {
        fwrite(&joias[i], sizeof(Joia), 1, dados);
    }
    
    printf("Inseridas %d joias no arquivo %s\n", quantidade, arquivo_dados);
    
    free(joias);
    fclose(csv);
    fclose(dados);
    return quantidade;
}

int mostrar_joias(const char* arquivo_dados) {
    FILE* arquivo = fopen(arquivo_dados, "rb");
    if (!arquivo) {
        printf("Erro ao abrir arquivo: %s\n", arquivo_dados);
        return -1;
    }
    
    Joia joia;
    int contador = 0;
    
    printf("\n=== JOIAS ===\n");
    printf("%-15s %-15s %-20s %-10s %-10s %-10s %-15s %-15s %-15s\n",
           "ID Produto", "ID Categoria", "Categoria", "ID Marca", "Preco", "Genero", 
           "Cor", "Material", "Pedra Preciosa");
    printf("--------------------------------------------------------------------------------------------------------\n");
    
    while (fread(&joia, sizeof(Joia), 1, arquivo) == 1) {
        printf("%-15lld %-15lld %-20s %-10lld %-10.2f %-10s %-15s %-15s %-15s\n",
                joia.id_produto, joia.id_categoria, joia.alias_categoria, joia.id_marca,
                joia.preco, joia.genero, joia.cor, joia.material, joia.pedra_preciosa);
        contador++;
        if (contador >= 20) { // Limitar exibicao
            printf("... (mostrando apenas os primeiros 20 registros)\n");
            break;
        }
    }
    
    fclose(arquivo);
    return contador;
}

int buscar_joia_binaria(const char* arquivo_dados, long long id_produto, Joia* joia) {
    FILE* arquivo = fopen(arquivo_dados, "rb");
    if (!arquivo) {
        printf("Erro ao abrir arquivo: %s\n", arquivo_dados);
        return -1;
    }
    
    fseek(arquivo, 0, SEEK_END);
    long tamanho = ftell(arquivo);
    int total_registros = (int)(tamanho / sizeof(Joia));
    
    int esquerda = 0;
    int direita = total_registros - 1;
    
    while (esquerda <= direita) {
        int meio = (esquerda + direita) / 2;
        fseek(arquivo, meio * sizeof(Joia), SEEK_SET);
        
        if (fread(joia, sizeof(Joia), 1, arquivo) != 1) {
            fclose(arquivo);
            return -1;
        }
        
        if (joia->id_produto == id_produto) {
            fclose(arquivo);
            return meio;
        } else if (joia->id_produto < id_produto) {
            esquerda = meio + 1;
        } else {
            direita = meio - 1;
        }
    }
    
    fclose(arquivo);
    return -1; // Nao encontrado
}

int consultar_joia(const char* arquivo_dados, long long id_produto) {
    Joia joia;
    int posicao = buscar_joia_binaria(arquivo_dados, id_produto, &joia);
    
    if (posicao >= 0) {
        printf("\n=== JOIA ENCONTRADA ===\n");
        printf("ID Produto: %lld\n", joia.id_produto);
        printf("ID Categoria: %lld\n", joia.id_categoria);
        printf("Categoria: %s\n", joia.alias_categoria);
        printf("ID Marca: %lld\n", joia.id_marca);
        printf("Preco: $%.2f\n", joia.preco);
        printf("Genero: %s\n", joia.genero);
        printf("Cor: %s\n", joia.cor);
        printf("Material: %s\n", joia.material);
        printf("Pedra Preciosa: %s\n", joia.pedra_preciosa);
        printf("Posicao no arquivo: %d\n", posicao);
        return 1;
    } else {
        printf("Joia com ID Produto %lld nao encontrada.\n", id_produto);
        return 0;
    }
}

int criar_indice_joias(const char* arquivo_dados, const char* arquivo_indice) {
    FILE* dados = fopen(arquivo_dados, "rb");
    if (!dados) {
        printf("Erro ao abrir arquivo de dados: %s\n", arquivo_dados);
        return -1;
    }
    
    FILE* indice = fopen(arquivo_indice, "wb");
    if (!indice) {
        printf("Erro ao criar arquivo de indice: %s\n", arquivo_indice);
        fclose(dados);
        return -1;
    }
    
    Joia joia;
    IndiceParcial entrada_indice;
    int posicao = 0;
    
    while (fread(&joia, sizeof(Joia), 1, dados) == 1) {
        entrada_indice.chave = joia.id_produto;
        entrada_indice.posicao = posicao;
        fwrite(&entrada_indice, sizeof(IndiceParcial), 1, indice);
        posicao++;
    }
    
    fclose(dados);
    fclose(indice);
    
    // Ordenar o indice por chave para busca binaria
    FILE* indice_read = fopen(arquivo_indice, "rb");
    if (!indice_read) {
        printf("Erro ao abrir arquivo de indice para ordenacao: %s\n", arquivo_indice);
        return -1;
    }
    
    IndiceParcial* indices = (IndiceParcial*)malloc(posicao * sizeof(IndiceParcial));
    if (!indices) {
        printf("Erro de memoria\n");
        fclose(indice_read);
        return -1;
    }
    
    fread(indices, sizeof(IndiceParcial), posicao, indice_read);
    fclose(indice_read);
    
    // Ordenar por chave
    qsort(indices, posicao, sizeof(IndiceParcial), comparar_indices);
    
    // Reescrever o indice ordenado
    FILE* indice_write = fopen(arquivo_indice, "wb");
    if (!indice_write) {
        printf("Erro ao reescrever arquivo de indice ordenado: %s\n", arquivo_indice);
        free(indices);
        return -1;
    }
    
    fwrite(indices, sizeof(IndiceParcial), posicao, indice_write);
    fclose(indice_write);
    free(indices);
    
    printf("Indice criado e ordenado com %d entradas para %s\n", posicao, arquivo_dados);
    return posicao;
}

// funcoes para compras
int inserir_compras_do_csv(const char* arquivo_csv, const char* arquivo_dados) {
    FILE* csv = fopen(arquivo_csv, "r");
    if (!csv) {
        printf("Erro ao abrir arquivo CSV: %s\n", arquivo_csv);
        return -1;
    }
    
    FILE* dados = fopen(arquivo_dados, "wb");
    if (!dados) {
        printf("Erro ao criar arquivo de dados: %s\n", arquivo_dados);
        fclose(csv);
        return -1;
    }
    
    char linha[1000];
    Compra* compras = NULL;
    int capacidade = 1000;
    int quantidade = 0;
    
    compras = (Compra*)malloc(capacidade * sizeof(Compra));
    if (!compras) {
        printf("Erro de memoria\n");
        fclose(csv);
        fclose(dados);
        return -1;
    }
    
    // Voltar para o inicio do arquivo
    fseek(csv, 0, SEEK_SET);
    
    while (fgets(linha, sizeof(linha), csv)) {
        if (quantidade >= capacidade) {
            capacidade *= 2;
            compras = (Compra*)realloc(compras, capacidade * sizeof(Compra));
            if (!compras) {
                printf("Erro de memoria\n");
                fclose(csv);
                fclose(dados);
                return -1;
            }
        }
        
        // Parse da linha CSV - ordem correta:
        // Order datetime, Order ID, Product ID, Quantity, Category ID, Category alias, Brand ID, Price, User ID, Gender, Color, Material, Gemstone
        char* token = strtok(linha, ",");
        if (!token) continue;
        
        // Order datetime (já lido acima)
        strncpy(compras[quantidade].data_hora, token, TAM_TIMESTAMP - 1);
        compras[quantidade].data_hora[TAM_TIMESTAMP - 1] = '\0';
        preencher_string(compras[quantidade].data_hora, TAM_TIMESTAMP);
        
        token = strtok(NULL, ","); // Order ID
        if (!token) continue;
        compras[quantidade].id_pedido = atoll(token);
        
        token = strtok(NULL, ","); // Product ID
        if (!token) continue;
        compras[quantidade].id_produto = atoll(token);
        
        token = strtok(NULL, ","); // Quantity
        if (!token) continue;
        compras[quantidade].quantidade = atoi(token);
        
        // Pular category_id, category_alias, brand_id, price
        for (int i = 0; i < 4; i++) {
            token = strtok(NULL, ",");
            if (!token) break;
        }
        
        token = strtok(NULL, ","); // User ID
        if (!token) continue;
        compras[quantidade].id_usuario = atoll(token);
        
        compras[quantidade].quebra_linha = '\n';
        quantidade++;
    }
    
    // Ordenar por id_pedido
    ordenar_compras_por_id_pedido(compras, quantidade);
    
    // Escrever no arquivo binario
    for (int i = 0; i < quantidade; i++) {
        fwrite(&compras[i], sizeof(Compra), 1, dados);
    }
    
    printf("Inseridas %d compras no arquivo %s\n", quantidade, arquivo_dados);
    
    free(compras);
    fclose(csv);
    fclose(dados);
    return quantidade;
}

int mostrar_compras(const char* arquivo_dados) {
    FILE* arquivo = fopen(arquivo_dados, "rb");
    if (!arquivo) {
        printf("Erro ao abrir arquivo: %s\n", arquivo_dados);
        return -1;
    }
    
    Compra compra;
    int contador = 0;
    
    printf("\n=== COMPRAS ===\n");
        printf("%-15s %-15s %-8s %-15s %-30s\n",
           "ID Pedido", "ID Produto", "Qtd", "ID Usuario", "Data/Hora");
    printf("--------------------------------------------------------------------\n");
    
    while (fread(&compra, sizeof(Compra), 1, arquivo) == 1) {
        printf("%-15lld %-15lld %-8d %-15lld %-30s\n",
                compra.id_pedido, compra.id_produto, compra.quantidade,
                compra.id_usuario, compra.data_hora);
        contador++;
        if (contador >= 20) {
            printf("... (mostrando apenas os primeiros 20 registros)\n");
            break;
        }
    }
    
    fclose(arquivo);
    return contador;
}

int buscar_compra_binaria(const char* arquivo_dados, long long id_pedido, Compra* compra) {
    FILE* arquivo = fopen(arquivo_dados, "rb");
    if (!arquivo) {
        printf("Erro ao abrir arquivo: %s\n", arquivo_dados);
        return -1;
    }
    
    fseek(arquivo, 0, SEEK_END);
    long tamanho = ftell(arquivo);
    int total_registros = (int)(tamanho / sizeof(Compra));
    
    int esquerda = 0;
    int direita = total_registros - 1;
    
    while (esquerda <= direita) {
        int meio = (esquerda + direita) / 2;
        fseek(arquivo, meio * sizeof(Compra), SEEK_SET);
        
        if (fread(compra, sizeof(Compra), 1, arquivo) != 1) {
            fclose(arquivo);
            return -1;
        }
        
        if (compra->id_pedido == id_pedido) {
            fclose(arquivo);
            return meio;
        } else if (compra->id_pedido < id_pedido) {
            esquerda = meio + 1;
        } else {
            direita = meio - 1;
        }
    }
    
    fclose(arquivo);
    return -1; // Nao encontrado
}

int consultar_compra(const char* arquivo_dados, long long id_pedido) {
    Compra compra;
    int posicao = buscar_compra_binaria(arquivo_dados, id_pedido, &compra);
    
    if (posicao >= 0) {
        printf("\n=== COMPRA ENCONTRADA ===\n");
        printf("ID Pedido: %lld\n", compra.id_pedido);
        printf("ID Produto: %lld\n", compra.id_produto);
        printf("Quantidade: %d\n", compra.quantidade);
        printf("ID Usuario: %lld\n", compra.id_usuario);
        printf("Data/Hora: %s\n", compra.data_hora);
        printf("Posicao no arquivo: %d\n", posicao);
        return 1;
    } else {
        printf("Compra com ID Pedido %lld nao encontrada.\n", id_pedido);
        return 0;
    }
}

int criar_indice_compras(const char* arquivo_dados, const char* arquivo_indice) {
    FILE* dados = fopen(arquivo_dados, "rb");
    if (!dados) {
        printf("Erro ao abrir arquivo de dados: %s\n", arquivo_dados);
        return -1;
    }
    
    FILE* indice = fopen(arquivo_indice, "wb");
    if (!indice) {
        printf("Erro ao criar arquivo de indice: %s\n", arquivo_indice);
        fclose(dados);
        return -1;
    }
    
    Compra compra;
    IndiceParcial entrada_indice;
    int posicao = 0;
    
    while (fread(&compra, sizeof(Compra), 1, dados) == 1) {
        entrada_indice.chave = compra.id_pedido;
        entrada_indice.posicao = posicao;
        fwrite(&entrada_indice, sizeof(IndiceParcial), 1, indice);
        posicao++;
    }
    
    fclose(dados);
    fclose(indice);
    
    // Ordenar o indice por chave para busca binaria
    FILE* indice_read = fopen(arquivo_indice, "rb");
    if (!indice_read) {
        printf("Erro ao abrir arquivo de indice para ordenacao: %s\n", arquivo_indice);
        return -1;
    }
    
    IndiceParcial* indices = (IndiceParcial*)malloc(posicao * sizeof(IndiceParcial));
    if (!indices) {
        printf("Erro de memoria\n");
        fclose(indice_read);
        return -1;
    }
    
    fread(indices, sizeof(IndiceParcial), posicao, indice_read);
    fclose(indice_read);
    
    // Ordenar por chave
    qsort(indices, posicao, sizeof(IndiceParcial), comparar_indices);
    
    // Reescrever o indice ordenado
    FILE* indice_write = fopen(arquivo_indice, "wb");
    if (!indice_write) {
        printf("Erro ao reescrever arquivo de indice ordenado: %s\n", arquivo_indice);
        free(indices);
        return -1;
    }
    
    fwrite(indices, sizeof(IndiceParcial), posicao, indice_write);
    fclose(indice_write);
    free(indices);
    
    printf("Indice criado e ordenado com %d entradas para %s\n", posicao, arquivo_dados);
    return posicao;
}

// funcoes para visualizar indices
void mostrar_indice_joias(const char* arquivo_indice) {
    FILE* indice = fopen(arquivo_indice, "rb");
    if (!indice) {
        printf("Erro ao abrir arquivo de indice: %s\n", arquivo_indice);
        return;
    }

    IndiceParcial entrada;
    int contador = 0;
    
    printf("\n=== INDICE DE JOIAS ===\n");
    printf("%-15s %-15s\n", "ID Produto", "Posicao");
    printf("------------------------------\n");
    
    while (fread(&entrada, sizeof(IndiceParcial), 1, indice) == 1) {
        printf("%-15lld %-15ld\n", entrada.chave, entrada.posicao);
        contador++;
        if (contador >= 50) { // Limitar exibicao
            printf("... (mostrando apenas os primeiros 50 registros)\n");
            break;
        }
    }
    
    fclose(indice);
    printf("\nTotal de entradas no indice: %d\n", contador);
}

void mostrar_indice_compras(const char* arquivo_indice) {
    FILE* indice = fopen(arquivo_indice, "rb");
    if (!indice) {
        printf("Erro ao abrir arquivo de indice: %s\n", arquivo_indice);
        return;
    }

    IndiceParcial entrada;
    int contador = 0;
    
    printf("\n=== INDICE DE COMPRAS ===\n");
    printf("%-15s %-15s\n", "ID Pedido", "Posicao");
    printf("------------------------------\n");
    
    while (fread(&entrada, sizeof(IndiceParcial), 1, indice) == 1) {
        printf("%-15lld %-15ld\n", entrada.chave, entrada.posicao);
        contador++;
        if (contador >= 50) { // Limitar exibicao
            printf("... (mostrando apenas os primeiros 50 registros)\n");
            break;
        }
    }
    
    fclose(indice);
    printf("\nTotal de entradas no indice: %d\n", contador);
}

long long pesquisa_por_idx_arqInd(const char* arquivo_indice, long idx) {
    FILE* fp = fopen(arquivo_indice, "rb");
    if (!fp) {
        printf("Erro ao abrir arquivo de índice: %s\n", arquivo_indice);
        return 0;
    }

    fseek(fp, 0, SEEK_END);
    long nRegistros = ftell(fp) / sizeof(IndiceParcial);
    rewind(fp);

    long left = 0, right = nRegistros - 1;
    IndiceParcial entrada;
    long long melhor_chave = 0;
    long melhor_pos = -1;

    while (left <= right) {
        long mid = (left + right) / 2;

        fseek(fp, mid * sizeof(IndiceParcial), SEEK_SET);
        fread(&entrada, sizeof(IndiceParcial), 1, fp);

        if (entrada.posicao == idx) {
            fclose(fp);
            return entrada.chave; // Encontrou posição EXATA (raro em índice parcial)
        }
        else if (entrada.posicao < idx) {
            // possível candidato
            melhor_chave = entrada.chave;
            melhor_pos = entrada.posicao;
            left = mid + 1;
        }
        else {
            right = mid - 1;
        }
    }

    fclose(fp);

    // Se nenhum índice é <= idx, não há como buscar
    if (melhor_pos == -1)
        return 0;

    return melhor_chave;
}

int inserir_joia_ordenada(const char* arquivo_dados, const char* arquivo_indice) {
    Joia nova;
    printf("=== Inserir nova joia ===\n");

    printf("ID do produto (chave única): ");
    scanf("%lld", &nova.id_produto);

    printf("ID da categoria: ");
    scanf("%lld", &nova.id_categoria);

    printf("Alias da categoria: ");
    getchar(); // consumir \n
    fgets(nova.alias_categoria, TAM_CATEGORIA, stdin);
    nova.alias_categoria[strcspn(nova.alias_categoria, "\n")] = '\0';
    preencher_string(nova.alias_categoria, TAM_CATEGORIA);

    printf("ID da marca: ");
    scanf("%lld", &nova.id_marca);

    printf("Preço: ");
    scanf("%lf", &nova.preco);

    printf("Gênero (ex: f/m/u): ");
    getchar();
    fgets(nova.genero, TAM_GENERO, stdin);
    nova.genero[strcspn(nova.genero, "\n")] = '\0';
    preencher_string(nova.genero, TAM_GENERO);

    printf("Cor: ");
    fgets(nova.cor, TAM_COR, stdin);
    nova.cor[strcspn(nova.cor, "\n")] = '\0';
    preencher_string(nova.cor, TAM_COR);

    printf("Material: ");
    fgets(nova.material, TAM_MATERIAL, stdin);
    nova.material[strcspn(nova.material, "\n")] = '\0';
    preencher_string(nova.material, TAM_MATERIAL);

    printf("Pedra preciosa: ");
    fgets(nova.pedra_preciosa, TAM_GEMSTONE, stdin);
    nova.pedra_preciosa[strcspn(nova.pedra_preciosa, "\n")] = '\0';
    preencher_string(nova.pedra_preciosa, TAM_GEMSTONE);

    nova.quebra_linha = '\n';

    // ---------- Ler todas as joias existentes ----------
    FILE* dados = fopen(arquivo_dados, "rb");
    if (!dados) {
        printf("Erro ao abrir arquivo de dados para leitura: %s\n", arquivo_dados);
        return 0;
    }

    Joia* joias = NULL;
    int qtd = 0;
    int capacidade = 1000;
    joias = (Joia*)malloc(capacidade * sizeof(Joia));
    if (!joias) {
        printf("Erro de memória\n");
        fclose(dados);
        return -1;
    }

    while (fread(&joias[qtd], sizeof(Joia), 1, dados) == 1) {
        qtd++;
        if (qtd >= capacidade) {
            capacidade *= 2;
            joias = (Joia*)realloc(joias, capacidade * sizeof(Joia));
            if (!joias) {
                printf("Erro de memória\n");
                fclose(dados);
                return -1;
            }
        }
    }
    fclose(dados);

    // ---------- Inserir a nova joia no vetor ----------
    joias = (Joia*)realloc(joias, (qtd + 1) * sizeof(Joia));
    joias[qtd] = nova;
    qtd++;

    // Ordenar todas por id_produto
    ordenar_joias_por_id_produto(joias, qtd);

    // ---------- Regravar o arquivo de dados ordenado ----------
    FILE* dados_out = fopen(arquivo_dados, "wb");
    if (!dados_out) {
        printf("Erro ao regravar arquivo de dados: %s\n", arquivo_dados);
        free(joias);
        return -1;
    }

    for (int i = 0; i < qtd; i++) {
        fwrite(&joias[i], sizeof(Joia), 1, dados_out);
    }
    fclose(dados_out);

    printf("Nova joia inserida e arquivo regravado com %d registros.\n", qtd);

    // ---------- Recriar o índice ----------
    criar_indice_joias(arquivo_dados, arquivo_indice);

    free(joias);
    return 0;
}
int inserir_compra_ordenada(const char* arquivo_dados, const char* arquivo_indice) {
    FILE* fp = fopen(arquivo_dados, "rb");
    if (!fp) {
        printf("Arquivo %s não encontrado. Criando novo...\n", arquivo_dados);
        fp = fopen(arquivo_dados, "wb");
        if (!fp) {
            printf("Erro ao criar arquivo de dados.\n");
            return -1;
        }
        fclose(fp);
        fp = fopen(arquivo_dados, "rb");
    }

    // Carregar todas as compras existentes
    Compra* compras = NULL;
    int qtd = 0;
    Compra temp;

    while (fread(&temp, sizeof(Compra), 1, fp) == 1) {
        compras = realloc(compras, (qtd + 1) * sizeof(Compra));
        if (!compras) {
            printf("Erro de memória.\n");
            fclose(fp);
            return -1;
        }
        compras[qtd++] = temp;
    }
    fclose(fp);

    // Ler nova compra do usuário
    Compra nova;
    printf("Digite o ID do pedido: ");
    scanf("%lld", &nova.id_pedido);
    printf("Digite o ID do produto: ");
    scanf("%lld", &nova.id_produto);
    printf("Digite a quantidade: ");
    scanf("%d", &nova.quantidade);
    printf("Digite o ID do usuário: ");
    scanf("%lld", &nova.id_usuario);
    printf("Digite a data e hora (ex: 2025-10-21 14:35:00): ");
    scanf(" %[^\n]", nova.data_hora);
    nova.quebra_linha = '\n';

    // Inserir no vetor
    compras = realloc(compras, (qtd + 1) * sizeof(Compra));
    if (!compras) {
        printf("Erro de memória.\n");
        return -1;
    }
    compras[qtd++] = nova;

    // Ordenar por id_pedido
    qsort(compras, qtd, sizeof(Compra), comparar_compras);

    // Regravar o arquivo ordenado
    fp = fopen(arquivo_dados, "wb");
    if (!fp) {
        printf("Erro ao abrir arquivo de dados para escrita.\n");
        free(compras);
        return -1;
    }
    fwrite(compras, sizeof(Compra), qtd, fp);
    fclose(fp);

    // Recriar o índice
    criar_indice_compras(arquivo_dados, arquivo_indice); // usa a mesma lógica do joias

    free(compras);
    printf("Compra inserida com sucesso e arquivo %s reordenado.\n", arquivo_dados);
    return 0;
}
int remove_compra(const char* arquivo_dados, const char* arquivo_indice) {
    long long id_busca;
    printf("Digite o ID do pedido que deseja remover: ");
    scanf("%lld", &id_busca);

    // 1️⃣ Abrir o arquivo de índice
    FILE* idx = fopen(arquivo_indice, "rb");
    if (!idx) {
        printf("Erro ao abrir o arquivo de índice.\n");
        return -1;
    }

    // 2️⃣ Procurar a chave no índice
    IndiceParcial entrada;
    long posicao_dado = -1;
    long qtd_indices = 0;
    while (fread(&entrada, sizeof(IndiceParcial), 1, idx) == 1) {
        if (entrada.chave == id_busca) {
            posicao_dado = entrada.posicao;
        }
        qtd_indices++;
    }
    fclose(idx);

    if (posicao_dado == -1) {
        printf("ID %lld não encontrado no índice.\n", id_busca);
        return 0;
    }

    // 3️⃣ Abrir o arquivo de dados e marcar como "removido"
    FILE* dados = fopen(arquivo_dados, "r+b");
    if (!dados) {
        printf("Erro ao abrir o arquivo de dados.\n");
        return -1;
    }

    fseek(dados, posicao_dado * sizeof(Compra), SEEK_SET);
    Compra removida;

    // Ler o registro atual
    if (fread(&removida, sizeof(Compra), 1, dados) != 1) {
        printf("Erro ao ler registro para remoção.\n");
        fclose(dados);
        return -1;
    }

    // Marcar como "nulo"
    removida.id_pedido = -1;
    removida.id_produto = -1;
    removida.id_usuario = -1;
    removida.quantidade = 0;
    strcpy(removida.data_hora, "REMOVIDO");
    removida.quebra_linha = '\n';

    // Voltar o ponteiro e regravar
    fseek(dados, posicao_dado * sizeof(Compra), SEEK_SET);
    fwrite(&removida, sizeof(Compra), 1, dados);
    fclose(dados);

    printf("Registro ID %lld marcado como removido.\n", id_busca);

    // 4️⃣ Remover a entrada do índice
    idx = fopen(arquivo_indice, "rb");
    if (!idx) {
        printf("Erro ao reabrir o arquivo de índice.\n");
        return -1;
    }

    IndiceParcial* indices = malloc(qtd_indices * sizeof(IndiceParcial));
    if (!indices) {
        printf("Erro de memória.\n");
        fclose(idx);
        return -1;
    }

    fread(indices, sizeof(IndiceParcial), qtd_indices, idx);
    fclose(idx);

    FILE* idx_novo = fopen(arquivo_indice, "wb");
    if (!idx_novo) {
        printf("Erro ao reabrir arquivo de índice para escrita.\n");
        free(indices);
        return -1;
    }

    for (long i = 0; i < qtd_indices; i++) {
        if (indices[i].chave != id_busca) {
            fwrite(&indices[i], sizeof(IndiceParcial), 1, idx_novo);
        }
    }

    fclose(idx_novo);
    free(indices);

    printf("Entrada removida do índice com sucesso.\n");
    return 1;
}
int remove_joia(const char* arquivo_dados, const char* arquivo_indice) {
    long long id_busca;
    printf("Digite o ID do produto que deseja remover: ");
    scanf("%lld", &id_busca);

    FILE* idx = fopen(arquivo_indice, "rb");
    if (!idx) {
        printf("Erro ao abrir o arquivo de índice.\n");
        return -1;
    }

    IndiceParcial entrada;
    long posicao_dado = -1;
    long qtd_indices = 0;

    while (fread(&entrada, sizeof(IndiceParcial), 1, idx) == 1) {
        if (entrada.chave == id_busca) {
            posicao_dado = entrada.posicao;
        }
        qtd_indices++;
    }
    fclose(idx);

    if (posicao_dado == -1) {
        printf("ID %lld não encontrado no índice.\n", id_busca);
        return 0;
    }

    FILE* dados = fopen(arquivo_dados, "r+b");
    if (!dados) {
        printf("Erro ao abrir o arquivo de dados.\n");
        return -1;
    }

    fseek(dados, posicao_dado * sizeof(Joia), SEEK_SET);
    Joia removida;

    if (fread(&removida, sizeof(Joia), 1, dados) != 1) {
        printf("Erro ao ler registro para remoção.\n");
        fclose(dados);
        return -1;
    }

    removida.id_produto = -1;
    removida.id_categoria = -1;
    removida.id_marca = -1;
    removida.preco = 0.0;
    strcpy(removida.alias_categoria, "REMOVIDO");
    strcpy(removida.genero, "");
    strcpy(removida.cor, "");
    strcpy(removida.material, "");
    strcpy(removida.pedra_preciosa, "");
    removida.quebra_linha = '\n';

    fseek(dados, posicao_dado * sizeof(Joia), SEEK_SET);
    fwrite(&removida, sizeof(Joia), 1, dados);
    fclose(dados);

    printf("Registro ID %lld marcado como removido no arquivo de dados.\n", id_busca);

    idx = fopen(arquivo_indice, "rb");
    if (!idx) {
        printf("Erro ao reabrir o arquivo de índice.\n");
        return -1;
    }

    IndiceParcial* indices = malloc(qtd_indices * sizeof(IndiceParcial));
    if (!indices) {
        printf("Erro de memória.\n");
        fclose(idx);
        return -1;
    }

    fread(indices, sizeof(IndiceParcial), qtd_indices, idx);
    fclose(idx);

    FILE* idx_novo = fopen(arquivo_indice, "wb");
    if (!idx_novo) {
        printf("Erro ao reabrir o arquivo de índice para escrita.\n");
        free(indices);
        return -1;
    }

    for (long i = 0; i < qtd_indices; i++) {
        if (indices[i].chave != id_busca) {
            fwrite(&indices[i], sizeof(IndiceParcial), 1, idx_novo);
        }
    }

    fclose(idx_novo);
    free(indices);

    printf("Entrada removida do índice com sucesso.\n");
    return 1;
}

void gerar_indice_parcial10(const char* arquivo_dados, const char* arquivo_indice) {
    FILE* dados = fopen(arquivo_dados, "rb");
    if (!dados) {
        printf("Erro ao abrir arquivo de dados: %s\n", arquivo_dados);
        return;
    }
    
    FILE* indice = fopen(arquivo_indice, "wb");
    if (!indice) {
        printf("Erro ao criar arquivo de indice: %s\n", arquivo_indice);
        fclose(dados);
        return;
    }

    Compra compra;
    IndiceParcial entrada_indice;
    int posicao = 0;
    int qtd_indices = 0;

    while (fread(&compra, sizeof(Compra), 1, dados) == 1) {

        if (posicao % 10 == 0) {
            entrada_indice.chave = compra.id_pedido;
            entrada_indice.posicao = posicao;

            fwrite(&entrada_indice, sizeof(IndiceParcial), 1, indice);
            qtd_indices++;
        }

        posicao++;
    }

    fclose(dados);
    fclose(indice);

    // Reabrir para ordenar
    FILE* indice_read = fopen(arquivo_indice, "rb");
    if (!indice_read) {
        printf("Erro ao abrir arquivo de indice para ordenacao\n");
        return;
    }

    IndiceParcial* indices = malloc(qtd_indices * sizeof(IndiceParcial));
    if (!indices) {
        printf("Erro de memoria\n");
        fclose(indice_read);
        return;
    }

    fread(indices, sizeof(IndiceParcial), qtd_indices, indice_read);
    fclose(indice_read);

    // Ordenar por chave
    qsort(indices, qtd_indices, sizeof(IndiceParcial), comparar_indices);

    // Gravar índice ordenado
    FILE* indice_write = fopen(arquivo_indice, "wb");
    if (!indice_write) {
        printf("Erro ao reescrever indice ordenado\n");
        free(indices);
        return;
    }

    fwrite(indices, sizeof(IndiceParcial), qtd_indices, indice_write);

    fclose(indice_write);
    free(indices);

    printf("Indice parcial criado: %d entradas (1 a cada 10 registros)\n", qtd_indices);
    
}

unsigned int hash_ll(long long x) {
    x = ((x >> 33) ^ x) * 0xff51afd7ed558ccdULL;
    x = ((x >> 33) ^ x) * 0xc4ceb9fe1a85ec53ULL;
    x = (x >> 33) ^ x;
    return (unsigned int)(x % TAM_HASH);
}

void inicializar_hash(HashTable* ht) {
    for (int i = 0; i < TAM_HASH; i++)
        ht->tabela[i] = NULL;
}

void hash_inserir(HashTable* ht, long long chaveBusca, long posicao) {
    unsigned int h = hash_ll(chaveBusca);

    HashNode* novo = (HashNode*)malloc(sizeof(HashNode));
    novo->chaveBusca = chaveBusca;
    novo->posicao = posicao;
    novo->next = ht->tabela[h];

    ht->tabela[h] = novo;
}
void construir_hash_por_coluna(const char* arquivo_dados, HashTable* ht) {

    FILE* fp = fopen(arquivo_dados, "rb");
    if (!fp) {
        printf("Erro ao abrir %s\n", arquivo_dados);
        return;
    }

    inicializar_hash(ht);

    Compra compra;
    long pos = 0;

    while (fread(&compra, sizeof(Compra), 1, fp) == 1) {

        long long chave = compra.id_usuario;

        hash_inserir(ht, chave, pos);

        pos++;
    }

    fclose(fp);

    printf("Índice hash em memória criado com %ld entradas\n", pos);
}

HashNode* hash_buscar(HashTable* ht, long long chaveBusca) {
    unsigned int h = hash_ll(chaveBusca);
    return ht->tabela[h];
}
void imprimir_resultados(HashNode* lista) {
    while (lista) {
        printf("Posição no arquivo: %ld\n", lista->posicao);
        lista = lista->next;
    }
}

// cria um novo no da arvore b (pode ser folha ou no interno)
NoArvoreB* criar_no_arvore_b(bool eh_folha){
    int i;
    NoArvoreB* novo_no =(NoArvoreB*)malloc(sizeof(NoArvoreB));
    if(!novo_no){
        printf("Erro de memoria ao criar no da arvore B\n");
        return NULL;
    }
    
    // inicializa os campos basicos do no
    novo_no->eh_folha = eh_folha;  // indica se e um no folha (sem filhos) ou interno
    novo_no->quantidade_chaves = 0;  // contador de chaves atualmente no no
    
    // inicializa o array de chaves e posicoes no arquivo
    for(i = 0; i < ORDEM_ARVORE_B - 1; i++){
        novo_no->chaves[i] = 0;
        novo_no->posicoes_arquivo[i] = -1;
    }

    // inicializa o array de ponteiros para filhos
    for(i = 0; i < ORDEM_ARVORE_B; i++){
        novo_no->filhos[i] = NULL;
    }
    
    return novo_no;
}

// destroi recursivamente a arvore b liberando toda a memoria alocada
void destruir_arvore_b(NoArvoreB* no){
    int i;
    if(!no){
        return;
    }
    // se nao for folha, precisa destruir os filhos primeiro
    if(!no->eh_folha){
        for(i = 0; i <= no->quantidade_chaves; i++){
            if(no->filhos[i]){
                destruir_arvore_b(no->filhos[i]);  // chamada recursiva para cada filho
            }
        }
    }
    
    // apos destruir todos libera o proprio no
    free(no);
}

// inicializa uma arvore b vazia
void inicializar_arvore_b(ArvoreB* arvore){
    arvore->raiz = NULL;
    arvore->total_registros = 0;
}

// busca uma chave na arvore b e retorna a posicao do registro no arquivo
int buscar_posicao_na_arvore_b(ArvoreB* arvore, long long chave_busca){
    if(!arvore || !arvore->raiz){
        return -1;  // arvore vazia, chave nao encontrada
    }
    
    NoArvoreB* no_atual = arvore->raiz;

    // desce da raiz ate uma folha em nos internos, comparamos a chave buscada com as chaves do no
    while(!no_atual->eh_folha){
        int indice = 0;
        int i;

        // busca linear nas chaves do no interno para encontrar o filho correto
        // filhos[0] contem chaves < chaves[0] (se chaves[0] existir)
        // filhos[1] contem chaves >= chaves[0] e < chaves[1] (se chaves[1] existir)
        // filhos[2] contem chaves >= chaves[1] e < chaves[2] (se chaves[2] existir)
        // ...
        // filhos[quantidade_chaves] contem chaves >= chaves[quantidade_chaves-1]
        
        // percorre todas as chaves separadoras
        for(i = 0; i < no_atual->quantidade_chaves; i++){
            if(chave_busca < no_atual->chaves[i]){
                // chave esta na subarvore do filho[i]
                indice = i;
                break;
            }
            // se chave_busca >= chaves[i], continua procurando
            indice = i + 1;  // atualiza para o proximo filho
        }
        // se nao encontrou nenhuma chave maior, indice sera quantidade_chaves (ultimo filho)

        // validar indice
        if(indice < 0 || indice > no_atual->quantidade_chaves){
            return -1;  // indice invalido
        }

        // verificar se o filho existe
        if(!no_atual->filhos[indice]){
            return -1;  // filho nao existe
        }

        no_atual = no_atual->filhos[indice];
    }
    
    // busca binaria na folha
    int esquerda = 0;
    int direita = no_atual->quantidade_chaves - 1;
    
    while(esquerda <= direita){
        int meio = (esquerda + direita) / 2;
        
        if(no_atual->chaves[meio] == chave_busca){
            // retorna a posicao no arquivo associada
            return no_atual->posicoes_arquivo[meio];
        }else if(no_atual->chaves[meio] < chave_busca){
            // chave buscada eh maior, buscar na metade direita
            esquerda = meio + 1;
        }else{
            // chave buscada eh menor, buscar na metade esquerda
            direita = meio - 1;
        }
    }
    
    return -1;  // chave nao encontrada na folha
}

// insere uma chave e sua posicao no arquivo em um no folhaa
int inserir_chave_no_folha(NoArvoreB* no_folha, long long chave, long posicao){

    int i;

    if(no_folha->quantidade_chaves >= ORDEM_ARVORE_B - 1){
        return -1;  // no cheio
    }

    // busca para encontrar onde a nova chave sera inserida
    int indice_insercao = 0;

    while(indice_insercao < no_folha->quantidade_chaves && no_folha->chaves[indice_insercao] < chave){
        indice_insercao++;
    }
    
    // desloca as chaves existentes para a direita
    for(i = no_folha->quantidade_chaves; i > indice_insercao; i--){
        no_folha->chaves[i] = no_folha->chaves[i - 1];
        no_folha->posicoes_arquivo[i] = no_folha->posicoes_arquivo[i - 1];
    }

    // insere a nova chave
    no_folha->chaves[indice_insercao] = chave;
    no_folha->posicoes_arquivo[indice_insercao] = posicao;
    no_folha->quantidade_chaves++;  // incrementa o contador de chaves
    
    return indice_insercao;  // retorna o indice onde foi inserido
}

// constroi uma arvore b em memoria a partir do arquivo de dados ordenado
void construir_arvore_b_do_arquivo(const char* arquivo_dados, ArvoreB* arvore){
    int i, j, k;
    FILE* arquivo = fopen(arquivo_dados, "rb");
    if(!arquivo){
        printf("Erro ao abrir arquivo de dados: %s\n", arquivo_dados);
        return;
    }

    inicializar_arvore_b(arvore);
    
    Joia joia;
    long posicao_arquivo = 0;  // posicao atual no arquivo
    int total_inserido = 0;  // contador de registros inseridos na arvore
    
    // estrutura para armazenar todas as folhas criadas
    NoArvoreB** folhas = NULL;
    int capacidade_folhas = 1000;  // capacidade inicial
    int total_folhas = 0;  // numero atual de folhas criadas
    NoArvoreB* folha_atual = NULL;  // folha atual
    
    folhas =(NoArvoreB**)malloc(capacidade_folhas * sizeof(NoArvoreB*));
    if(!folhas){
        printf("ERRO: Falha ao alocar memoria para lista de folhas\n");
        fclose(arquivo);
        return;
    }
    
    printf("Lendo arquivo e criando folhas...\n");
    
    int registros_lidos = 0;
    int registros_ignorados = 0;
    
    // ler e criar todas as folhas
    while(fread(&joia, sizeof(Joia), 1, arquivo) == 1){
        registros_lidos++;
        
        if(joia.id_produto < 0){
            registros_ignorados++;
            posicao_arquivo++;
            continue;
        }
        
        // criar uma nova folha caso nao tenha folha atual ou esteja cheia
        if(!folha_atual || folha_atual->quantidade_chaves >= ORDEM_ARVORE_B - 1){
            folha_atual = criar_no_arvore_b(true);
            if(!folha_atual){
                printf("ERRO: Falha ao criar no folha\n");
                fclose(arquivo);

                // limpar memoria em caso de erro
                for(i = 0; i < total_folhas; i++){
                    destruir_arvore_b(folhas[i]);
                }
                free(folhas);
                return;
            }

            // se o array de folhas esta cheio, dobrar sua capacidade
            if(total_folhas >= capacidade_folhas){
                capacidade_folhas *= 2;
                NoArvoreB** novo_array =(NoArvoreB**)realloc(folhas, capacidade_folhas * sizeof(NoArvoreB*));
                if(!novo_array){
                    printf("ERRO: Falha ao realocar memoria\n");
                    fclose(arquivo);
                    for(i = 0; i < total_folhas; i++){
                        destruir_arvore_b(folhas[i]);
                    }
                    free(folhas);
                    return;
                }
                folhas = novo_array;
            }
            
            // adiciona a nova folha ao array
            folhas[total_folhas] = folha_atual;
            total_folhas++;
        }

        // insere a chave na folha atual mantendo a ordenacao
        int resultado = inserir_chave_no_folha(folha_atual, joia.id_produto, posicao_arquivo);
        if(resultado == -1){
            printf("ERRO: Falha ao inserir chave %lld na posicao %ld(folha cheia?)\n", joia.id_produto, posicao_arquivo);
        }else{
            total_inserido++;
        }
        posicao_arquivo++;

        // mostrar progresso periodicamente
        if(registros_lidos % 10000 == 0){
            printf("Progresso: %d registros lidos, %d inseridos, %d folhas criadas\n", registros_lidos, total_inserido, total_folhas);
        }
    }
    
    printf("Total de registros lidos: %d\n", registros_lidos);
    printf("Total de registros ignorados: %d\n", registros_ignorados);
    printf("Total de registros inseridos: %d\n", total_inserido);
    printf("Total de folhas criadas: %d\n", total_folhas);
    
    fclose(arquivo);
    arvore->total_registros = total_inserido;

    // se temos apenas uma folha, ela e a raiz
    if(total_folhas == 1){
        arvore->raiz = folhas[0];
        printf("Arvore B criada com sucesso! Total de registros: %d\n", total_inserido);
        return;
    }
    
    // constroi os niveis internos de baixo para cima
    printf("Criando estrutura de nos internos...\n");
    printf("Total de folhas: %d, cada no interno pode ter %d filhos\n", total_folhas, ORDEM_ARVORE_B);

    NoArvoreB** nivel_atual = folhas;  // comeca com as folhas atuais
    int nos_no_nivel = total_folhas;  // numero de nos atuais
    int nivel = 0;  // nivel atual
    
    NoArvoreB** nivel_anterior_array = NULL;  // array do nivel anterior
    
    // criar um nivel acima enquanto temos mais de um no atual
    while(nos_no_nivel > 1){
        nivel++;
        int nos_proximo_nivel =(nos_no_nivel + ORDEM_ARVORE_B - 1) / ORDEM_ARVORE_B;

        NoArvoreB** proximo_nivel =(NoArvoreB**)malloc(nos_proximo_nivel * sizeof(NoArvoreB*));
        
        if(!proximo_nivel){
            printf("ERRO: Falha ao alocar memoria para nivel %d(%d nos)\n", nivel, nos_proximo_nivel);

            // limpar memoria em caso de erro
            if(nivel_anterior_array){
                for(i = 0; i < nos_no_nivel; i++){
                    destruir_arvore_b(nivel_atual[i]);
                }
                free(nivel_anterior_array);
            }
            for(i = 0; i < total_folhas; i++){
                destruir_arvore_b(folhas[i]);
            }
            free(folhas);
            return;
        }
    
        // criar os nos internos do proximo nivel
        for(i = 0; i < nos_proximo_nivel; i++){
            NoArvoreB* no_interno = criar_no_arvore_b(false);
            if(!no_interno){
                printf("ERRO: Falha ao criar no interno\n");

                // limpar memoria em caso de erro
                for(j = 0; j < i; j++){
                    destruir_arvore_b(proximo_nivel[j]);
                }
                free(proximo_nivel);
                if(nivel > 1 && nivel_atual != folhas){
                    free(nivel_atual);
                }
                for(j = 0; j < total_folhas; j++){
                    destruir_arvore_b(folhas[j]);
                }
                free(folhas);
                return;
            }

            // calcula quais filhos do nivel anterior este no interno vai agrupar
            int inicio = i * ORDEM_ARVORE_B;
            int fim = inicio + ORDEM_ARVORE_B;
            if(fim > nos_no_nivel){
                fim = nos_no_nivel;  // ajustar para nao ultrapassar o limite
            }

            if(inicio >= nos_no_nivel){
                printf("ERRO: Indice inicio %d >= nos_no_nivel %d\n", inicio, nos_no_nivel);
                destruir_arvore_b(no_interno);
                free(proximo_nivel);
                if(nivel_anterior_array){
                    free(nivel_anterior_array);
                }
                for(j = 0; j < total_folhas; j++){
                    destruir_arvore_b(folhas[j]);
                }
                free(folhas);
                return;
            }
            
            // primeiro filho sempre aponta para o primeiro no do grupo
            no_interno->filhos[0] = nivel_atual[inicio];
            int indice_chave = 0;  // indice para inserir no interno
            int indice_filho = 1;  // indice para inserir no interno
            
            // inserir chave separadora a cada filho subsequente
            for(j = inicio + 1; j < fim && indice_filho < ORDEM_ARVORE_B; j++){
                if(j >= nos_no_nivel){
                    break;
                }

                // obter a primeira chave do filho para usar como chave separadora
                long long primeira_chave = 0;
                if(nivel == 1){
                    // se estamos criando o primeiro nivel interno, nivel_atual sao folhas
                    if(nivel_atual[j] && nivel_atual[j]->eh_folha){
                        for(k = 0; k < nivel_atual[j]->quantidade_chaves; k++){
                            if(nivel_atual[j]->chaves[k] > 0){
                                primeira_chave = nivel_atual[j]->chaves[k];
                                break;
                            }
                        }
                    }
                }else{
                    // se estamos criando niveis superiores, nivel_atual sao nos internos
                    if(nivel_atual[j] && !nivel_atual[j]->eh_folha && nivel_atual[j]->quantidade_chaves > 0){
                        primeira_chave = nivel_atual[j]->chaves[0];
                    }
                }
                
                // inserir a chave separadora e o filho correspondente
                if(primeira_chave > 0 && indice_chave < ORDEM_ARVORE_B - 1){
                    no_interno->chaves[indice_chave] = primeira_chave;
                    no_interno->filhos[indice_filho] = nivel_atual[j];
                    no_interno->quantidade_chaves++;
                    indice_chave++;
                    indice_filho++;
                }else{
                    // se nao conseguimos obter uma chave valida, apenas adiciona o filho
                    no_interno->filhos[indice_filho] = nivel_atual[j];
                    indice_filho++;
                }
            }
            
            proximo_nivel[i] = no_interno;
        }
    
        // liberar o array do nivel anterior
        if(nivel_anterior_array && nivel_anterior_array != folhas){
            free(nivel_anterior_array);
            nivel_anterior_array = NULL;
        }
        // atualizar para a proxima iteracao
        nivel_anterior_array = nivel_atual;  // guardar referencia para liberar depois
        nivel_atual = proximo_nivel;  // avancar para o proximo nivel
        nos_no_nivel = nos_proximo_nivel;
        
        printf("Nivel %d criado com %d nos\n", nivel, nos_no_nivel);

        if(nos_no_nivel <= 0){
            printf("ERRO: nos_no_nivel <= 0!\n");
            break;
        }

        // se chegamos a 1 no, ele eh a raiz
        if(nos_no_nivel == 1){
            break;
        }
    }
    
    // deve ter 1 no
    if(nos_no_nivel != 1){
        printf("ERRO: Esperado 1 no restante, mas temos %d\n", nos_no_nivel);
        // limpar memoria em caso de erro
        if(nivel_anterior_array && nivel_anterior_array != folhas){
            free(nivel_anterior_array);
        }
        if(nivel_atual && nivel_atual != folhas){
            for(i = 0; i < nos_no_nivel; i++){
                if(nivel_atual[i]) destruir_arvore_b(nivel_atual[i]);
            }
            free(nivel_atual);
        }
        for(i = 0; i < total_folhas; i++){
            destruir_arvore_b(folhas[i]);
        }
        free(folhas);
        return;
    }

    // liberar o array do nivel anterior se ainda existir
    if(nivel_anterior_array && nivel_anterior_array != folhas){
        free(nivel_anterior_array);
    }
    
    // o unico no restante eh a raiz
    arvore->raiz = nivel_atual[0];

    // liberar o array do ultimo nivel
    if(nivel_atual != folhas){
        free(nivel_atual);
    }

    // liberar o array de folhas
    free(folhas);

    printf("Arvore B criada com sucesso! Total de registros: %d, Total de folhas: %d, Niveis: %d\n", 
           total_inserido, total_folhas, nivel + 1);
}

// mostra recursivamente a estrutura da arvore b usando percorrimento em profundidade
void mostrar_arvore_b_recursivo(NoArvoreB* no, int nivel){
    int i;
    if(!no) return;

    // indentacao baseada no nivel
    for(i = 0; i < nivel; i++){
        printf("  ");
    }
    
    if(no->eh_folha){
        // mostra as chaves e posicoes do arquivo em um no folha
        printf("[FOLHA: ");
        for(i = 0; i < no->quantidade_chaves; i++){
            printf("%lld(%ld) ", no->chaves[i], no->posicoes_arquivo[i]);
        }
        printf("]\n");
    } else{
        // mostra as chaves separadoras em um no interno
        printf("[INTERNO: ");
        for(i = 0; i < no->quantidade_chaves; i++){
            printf("%lld ", no->chaves[i]);
        }
        printf("]\n");
        // recursivamente mostra todos os filhos deste no interno
        for(i = 0; i <= no->quantidade_chaves; i++){
            if(no->filhos[i]){
                mostrar_arvore_b_recursivo(no->filhos[i], nivel + 1);
            }
        }
    }
}

// conta recursivamente o numero de folhas na arvore b
int contar_folhas_arvore_b(NoArvoreB* no){
    int i, total = 0;
    if(!no){
        return 0;
    }

    if(no->eh_folha){
        return 1;
    }

    // soma o numero de folhas em cada subarvore filha
    for(i = 0; i <= no->quantidade_chaves; i++){
        if(no->filhos[i]){
            total += contar_folhas_arvore_b(no->filhos[i]);
        }
    }
    return total;
}

// conta recursivamente o numero total de chaves armazenadas em todas as folhas
int contar_chaves_arvore_b(NoArvoreB* no){
    int i, total = 0;
    if(!no){
        return 0;
    }
    
    // se eh uma folha, retorna o numero de chaves nela
    if(no->eh_folha){
        return no->quantidade_chaves;
    }
    
    // soma o numero de chaves em todas as folhas das subarvores filhas
    for(i = 0; i <= no->quantidade_chaves; i++){
        if(no->filhos[i]){
            total += contar_chaves_arvore_b(no->filhos[i]);
        }
    }
    return total;
}

// mostra informacoes sobre a estrutura da arvore b e sua organizacao hierarquica
void mostrar_arvore_b(ArvoreB* arvore){
    if(!arvore || !arvore->raiz){
        printf("Arvore B vazia.\n");
        return;
    }
    
    printf("\n=== ESTRUTURA DA ARVORE B ===\n");
    printf("Total de registros indexados(contador): %d\n", arvore->total_registros);
    
    // calcula estatisticas da arvore usando funcoes recursivas
    int total_folhas = contar_folhas_arvore_b(arvore->raiz);
    int total_chaves = contar_chaves_arvore_b(arvore->raiz);
    printf("Total de folhas na arvore: %d\n", total_folhas);
    printf("Total de chaves nas folhas: %d\n", total_chaves);
    
    // o numero de chaves deve corresponder ao numero de registros indexados
    if(total_chaves != arvore->total_registros){
        printf("*** ATENCAO: Contador de registros(%d) diferente do total de chaves(%d)!\n", 
               arvore->total_registros, total_chaves);
    }
    
    // mostra a estrutura hierarquica completa da arvore
    printf("\nEstrutura da arvore:\n");
    mostrar_arvore_b_recursivo(arvore->raiz, 0);
    printf("\n");
}

// funcao auxiliar para buscar chave em todas as folhas (debug)
void buscar_chave_em_folhas_recursivo(NoArvoreB* no, long long chave_busca, int* encontrado, long* posicao_encontrada){
    if(!no) return;
    
    if(no->eh_folha){
        // busca linear na folha
        for(int i = 0; i < no->quantidade_chaves; i++){
            if(no->chaves[i] == chave_busca){
                *encontrado = 1;
                *posicao_encontrada = no->posicoes_arquivo[i];
                return;
            }
        }
    } else {
        // recursao nos filhos
        for(int i = 0; i <= no->quantidade_chaves; i++){
            if(no->filhos[i]){
                buscar_chave_em_folhas_recursivo(no->filhos[i], chave_busca, encontrado, posicao_encontrada);
                if(*encontrado) return;
            }
        }
    }
}

// consulta uma joia usando a arvore b como indice em memoria
void consultar_joia_por_arvore_b(ArvoreB* arvore, const char* arquivo_dados, long long id_produto){
    if(!arvore || !arvore->raiz){
        printf("Arvore B nao foi inicializada.\n");
        return;
    }

    // buscar a chave na arvore b para obter a posicao no arquivo
    long posicao = buscar_posicao_na_arvore_b(arvore, id_produto);
    
    if(posicao == -1){
        // se a busca normal falhou, tenta busca recursiva em todas as folhas (fallback)
        int encontrado = 0;
        long posicao_fallback = -1;
        buscar_chave_em_folhas_recursivo(arvore->raiz, id_produto, &encontrado, &posicao_fallback);
        
        if(encontrado){
            posicao = posicao_fallback;
        } else {
            printf("Joia com ID Produto %lld nao encontrada na arvore B.\n", id_produto);
            printf("Tente usar a opcao 4(Busca binaria direta) para verificar se o registro existe.\n");
            return;
        }
    }
    
    // abrir o arquivo de dados e posicionar no registro encontrado
    FILE* arquivo = fopen(arquivo_dados, "rb");
    if(!arquivo){
        printf("Erro ao abrir arquivo de dados: %s\n", arquivo_dados);
        return;
    }

    // acesso direto ao registro usando a posicao retornada pela busca na arvore
    fseek(arquivo, posicao * sizeof(Joia), SEEK_SET);
    
    // ler o registro do arquivo e exibir os dados
    Joia joia;
    if(fread(&joia, sizeof(Joia), 1, arquivo) == 1){
        printf("\n=== JOIA ENCONTRADA(via Arvore B) ===\n");
        printf("ID Produto: %lld\n", joia.id_produto);
        printf("ID Categoria: %lld\n", joia.id_categoria);
        printf("Categoria: %s\n", joia.alias_categoria);
        printf("ID Marca: %lld\n", joia.id_marca);
        printf("Preco: $%.2f\n", joia.preco);
        printf("Genero: %s\n", joia.genero);
        printf("Cor: %s\n", joia.cor);
        printf("Material: %s\n", joia.material);
        printf("Pedra Preciosa: %s\n", joia.pedra_preciosa);
        printf("Posicao no arquivo: %ld\n", posicao);
    } else{
        printf("Erro ao ler registro do arquivo.\n");
    }
    
    fclose(arquivo);
}
