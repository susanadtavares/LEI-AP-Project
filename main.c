#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define NUM_ANDARES 3
#define LUGARES_POR_ANDAR 25

struct Cliente {
    int id; // id = 0 -> cliente não existe
    char nome[10];
    char matricula[9]; // XX-XX-XX
    int ativo; // 1 - ativo ; 0 - removido
};

struct Veiculo {
    int id;
    char matricula[9];
    int andar; // 3 andares
    int lugar; // 25 lugares
    int diaEntrada; // Dia do mês de entrada
    int mesEntrada; // Mês de entrada (1 a 12)
    int anoEntrada; // Ano de entrada (formato completo)
    int horaEntrada; // Hora de entrada
    int minutoEntrada; // Minuto de entrada
    struct Cliente cliente;
    int sub; // 1 - subscritor ; 0 - cliente normal
    int ativo; // 1 - ativo ; 0 - removido
};



struct DadosArquivo {
    int ultimoId;
};

static int index = 0;

static int esindex = 0;

void Logo();
void menu();

// DADOS
void salvarDados(struct Cliente *clientes, int numClientes, struct Veiculo *veiculo, int numVeiculos, int ultimoIdCliente, int ultimoIdVeiculo);
void carregarDados(struct Cliente *clientes, int *numClientes, struct Veiculo *veiculo, int *numVeiculos, int *ultimoIdCliente, int *ultimoIdVeiculo);
void limparDadosClientes();
void limparDadosVeiculos();

// FUNCOES CLIENTE
void subCliente(struct Cliente *clientes, int index, int *nextId, struct Cliente *cliente, struct Veiculo *veiculo);
void lerCliente(struct Cliente *clientes, int numClientes);
void encontrarCliente(struct Cliente *clientes, int numClientes);
void removerCliente(struct Cliente *clientes, int *numClientes, struct Veiculo *veiculo);
void extrairDados(struct Cliente *clientes, int *numClientes);

// FUNCOES VEICULO
void estacionar(struct Veiculo *veiculo, int *numVeiculos, struct Cliente *clientes, int numClientes, int *nextId);
void listarVeiculos(struct Veiculo *veiculo, int numVeiculos);
void retirarVeiculo(struct Veiculo *veiculo, int *numVeiculos);
void pagamento(struct Veiculo *veiculo);

int main() {
    struct Cliente clientes[100];
    struct Veiculo veiculo[100];
    int lugaresEstacionamento[NUM_ANDARES][LUGARES_POR_ANDAR] = {0};

    int ultimoIdCliente = 1; // Inicializado com o valor padrão
    int ultimoIdVeiculo = 1; // Inicializado com o valor padrão

    carregarDados(clientes, &index, veiculo, &esindex, &ultimoIdCliente, &ultimoIdVeiculo);

    system("cls");
    while (1) {
        Logo();
        menu();
        int escolha;
        printf("\n> ");
        scanf("%d", &escolha);

        // Limpar o buffer de entrada após a leitura de um inteiro
        while (getchar() != '\n');

        switch (escolha) {
            case 1:
                subCliente(clientes, index, &ultimoIdCliente, &clientes[index], veiculo);
                index++;
                break;
            case 2:
                lerCliente(clientes, index);
                break;
            case 3:
                encontrarCliente(clientes, index);
                break;
            case 4:
                removerCliente(clientes, &index, veiculo);
                break;
            case 5:
                extrairDados(clientes, &index);
                break;
            case 6:
                limparDadosClientes();
                return 0;
                break;
            case 7:
                estacionar(veiculo, &esindex, clientes, index, &ultimoIdVeiculo);
                esindex++;
                break;
            case 8:
                listarVeiculos(veiculo, esindex);
                break;
            case 9:
                retirarVeiculo(veiculo, &esindex);
                break;
            case 10:
                limparDadosVeiculos();
                return 0;
                break;
            case 0:
                salvarDados(clientes, index, veiculo, esindex, ultimoIdCliente, ultimoIdVeiculo);
                printf("Programa terminado.\n");
                return 0;
            default:
                printf("\033[0;31mOpcao invalida. Tente novamente.\033[0;37m\n");
        }
    }

    return 0;
}

void Logo() {
    char *filename = "Logo.txt";
    FILE *fp = fopen(filename, "r");

    if (fp == NULL)
    {
        printf("Erro: Nao e possivel abrir o ficheiro! %s", filename);
    }

    // reading line by line, max 256 bytes
    const unsigned MAX_LENGTH = 256;
    char buffer[MAX_LENGTH];

    while (fgets(buffer, MAX_LENGTH, fp))
        printf("%s", buffer);

    // close the file
    fclose(fp);
}

void menu() {
    printf("\n\t\t\t+---------------------\033[0;33m[ MENU ]\033[0;37m---------------------+\n");
    printf("\t\t\t|                                                  |\n");
    printf("\t\t\t|                   \033[0;33m[ CLIENTES ]\033[0;37m                   |\n");
    printf("\t\t\t| [1] Subscrever Cliente    [2] Listar Clientes    |\n");
    printf("\t\t\t| [3] Encontrar Cliente     [4] Remover Cliente    |\n");
    printf("\t\t\t| [5] Extrair Dados         [6] Limpar Dados       |\n");
    printf("\t\t\t|                                                  |\n");
    printf("\t\t\t|               \033[0;33m[ ESTACIONAMENTOS ]\033[0;37m                |\n");
    printf("\t\t\t| [7] Estacionar Carro    [8] Listar Veiculos      |\n");
    printf("\t\t\t| [9] Retirar Veiculo     [10] Limpar Dados        |\n");
    printf("\t\t\t| \033[0;31m[0] Sair\033[0;37m                                         |\n");
    printf("\t\t\t+--------------------------------------------------+\n");
}

void subCliente(struct Cliente *clientes, int index, int *nextId, struct Cliente *cliente, struct Veiculo *veiculo) {
    system("cls");
    fflush(stdin);

    (*nextId)++; // Incrementar o próximo ID

    printf("Matricula (XX-XX-XX): ");
    scanf(" %s", cliente->matricula);

    //Verificar se matricula já existe
    for (int i = 0; i < index; i++) {
        if (clientes[i].id != 0 && strcmp(clientes[i].matricula, cliente->matricula) == 0) {
            printf("\033[0;31mMatricula já existe!\033[0;37m\n");
            cliente->id = 0;
            return;
        }
    }

    //Verificar se matricula está atribuida a um cliente
    for (int i = 0; i < esindex; i++) {
        if (strcmp(veiculo[i].matricula, cliente->matricula) == 0) {
            veiculo[i].sub = 1; // É subscritor
        } else {
            veiculo[i].sub = 0; // Não é subscritor
        }
    }

    printf("Nome: ");
    scanf(" %s", cliente->nome);

    cliente->id = *nextId;
    printf("\033[0;32mCliente adicionado com sucesso!\033[0;37m\n"); 
}

void lerCliente(struct Cliente *clientes, int numClientes) {
    system("cls");
    if (numClientes == 0) {
        printf("\033[0;31mNenhum cliente cadastrado ainda!\033[0;37m\n");
        return;
    }

    for (int i = 0; i < numClientes; i++) {
        if (clientes[i].id != 0) {
            printf("\033[0;33m== ID: %d ==\033[0;37m\n", clientes[i].id);
            printf("Nome: %s\nMatricula: %s\n\n", clientes[i].nome, clientes[i].matricula);
        }
    }

    printf("Pressiona ENTER para voltar...\n");  
    getchar();   
}

void encontrarCliente(struct Cliente *clientes, int numClientes) {
    system("cls");
    char busca[10];
    printf("Insira o nome, matricula ou ID do cliente: ");
    scanf(" %9[^\n]", busca);

    for (int i = 0; i < numClientes; i++) {
        if(clientes[i].id != 0) {
            if (strcmp(clientes[i].nome, busca) == 0 || strcmp(clientes[i].matricula, busca) == 0 || clientes[i].id == atoi(busca) && numClientes != 0) {
                printf("\n\033[0;33m== ID: %d ==\033[0;37m\n", clientes[i].id);
                printf("Nome: %s\nMatricula: %s\n\n", clientes[i].nome, clientes[i].matricula);
                return; // Encerrar busca após encontrar o cliente
            }
        }
    }

    printf("\033[0;31mCliente nao encontrado!\033[0;37m\n");
}

void removerCliente(struct Cliente *clientes, int *numClientes, struct Veiculo *veiculo) {
    int encontrado = 0, i;
    system("cls");
    if (*numClientes == 0) {
        printf("\033[0;31mNenhum cliente cadastrado para remover.\033[0;37m\n");
        return;
    }

    char busca[10];
    printf("Insira a matricula ou ID do cliente a ser removido: ");
    scanf(" %9[^\n]", busca);


    for (i = 0; i < *numClientes; i++) {
        if (strcmp(clientes[i].matricula, busca) == 0 || clientes[i].id == atoi(busca) && &numClientes != 0) {

            //Remover carro subscricao
            for (int i = 0; i < esindex; i++) {
                if (strcmp(veiculo[i].matricula, clientes->matricula) == 0) {
                    veiculo[i].sub = 0;
                }
            }

            // Marcar cliente como removido, mantendo ID 0
            clientes[i].id = 0;
            encontrado = 1;
            printf("\033[0;32mCliente removido com sucesso.\033[0;37m\n");
            *numClientes--;
            return;
        }
    }

    if (!encontrado) {
        printf("\033[0;31mCliente nao encontrado.\033[0;37m\n");
    }
}

void extrairDados(struct Cliente *clientes, int *numClientes) {
    system("cls");
    FILE *arquivo;
    arquivo = fopen("clientes.txt", "w");

    if (arquivo == NULL) {
        printf("\033[0;31mErro ao abrir o arquivo.\033[0;37m\n");
        return;
    }

    for (int i = 0; i < *numClientes; i++) {
        if (clientes[i].id != 0) { // Ignorar clientes marcados como removidos
            fprintf(arquivo, "== ID: %d ==\n", clientes[i].id);
            fprintf(arquivo, "Nome: %s\nMatricula: %s\n\n", clientes[i].nome, clientes[i].matricula);
        }
    }

    fclose(arquivo);

    printf("\033[0;33mDados extraidos para o arquivo clientes.txt.\033[0;37m\n");
}


void estacionar(struct Veiculo *veiculo, int *numVeiculos, struct Cliente *clientes, int numClientes, int *nextId) {
    system("cls");
    fflush(stdin);

    (*nextId)++; // Incrementar o próximo ID

    struct Veiculo novoVeiculo;
    printf("Matricula (XX-XX-XX): ");
    scanf(" %s", novoVeiculo.matricula);

    // Verificar se matricula já existe em veiculos
    for (int i = 0; i < *numVeiculos; i++) {
        if (veiculo[i].id != 0 && strcmp(veiculo[i].matricula, novoVeiculo.matricula) == 0) {
            printf("\033[0;31mVeiculo já estacionado!\033[0;37m\n");
            return;
        }
    }

    // Verificar se matricula já existe em clientes
    for (int i = 0; i < numClientes; i++) {
        if (clientes[i].id != 0 && strcmp(clientes[i].matricula, novoVeiculo.matricula) == 0) {
            novoVeiculo.sub = 1; // É subscritor
            break;
        } else {
            novoVeiculo.sub = 0; // Não é subscritor
        }
    }

    // Seleção do andar
    printf("Andar (1 a %d): ", NUM_ANDARES);
    scanf("%d", &novoVeiculo.andar);

    if (novoVeiculo.andar < 1 || novoVeiculo.andar > NUM_ANDARES) {
        printf("\033[0;31mAndar introduzido invalido!\033[0;37m\n");
        return;
    }

    // Seleção do lugar
    printf("Lugar (1 a %d): ", LUGARES_POR_ANDAR);
    scanf("%d", &novoVeiculo.lugar);

    if (novoVeiculo.lugar < 1 || novoVeiculo.lugar > LUGARES_POR_ANDAR) {
        printf("\033[0;31mLugar introduzido invalido!\033[0;37m\n");
        return;
    }

    // Verificar se o local já está ocupado
    for (int i = 0; i < *numVeiculos; i++) {
        if (veiculo[i].id != 0 && veiculo[i].andar == novoVeiculo.andar && veiculo[i].lugar == novoVeiculo.lugar) {
            printf("\033[0;31mEste lugar já está ocupado!\033[0;37m\n");
            return;
        }
    }

    time_t now_time = time(NULL);
    struct tm *localtime_info = localtime(&now_time);

    // Atribuir informações de data e hora de entrada ao novo veículo
    novoVeiculo.diaEntrada = localtime_info->tm_mday;
    novoVeiculo.mesEntrada = localtime_info->tm_mon + 1; // tm_mon é base 0, então adicionamos 1
    novoVeiculo.anoEntrada = localtime_info->tm_year + 1900; // tm_year é o número de anos desde 1900
    novoVeiculo.horaEntrada = localtime_info->tm_hour;
    novoVeiculo.minutoEntrada = localtime_info->tm_min;

    // Atribuir ID ao veículo
    veiculo[*numVeiculos].id = *nextId;
    strcpy(veiculo[*numVeiculos].matricula, novoVeiculo.matricula);
    veiculo[*numVeiculos].andar = novoVeiculo.andar;
    veiculo[*numVeiculos].lugar = novoVeiculo.lugar;
    veiculo[*numVeiculos].diaEntrada = novoVeiculo.diaEntrada;
    veiculo[*numVeiculos].mesEntrada = novoVeiculo.mesEntrada;
    veiculo[*numVeiculos].anoEntrada = novoVeiculo.anoEntrada;
    veiculo[*numVeiculos].horaEntrada = novoVeiculo.horaEntrada;
    veiculo[*numVeiculos].minutoEntrada = novoVeiculo.minutoEntrada;
    veiculo[*numVeiculos].sub = novoVeiculo.sub;

    printf("\033[0;32mVeiculo estacionado com sucesso!\033[0;37m\n");
}


void listarVeiculos(struct Veiculo *veiculo, int numVeiculos) {
    system("cls");
    if (numVeiculos == 0) {
        printf("\033[0;31mNenhum Veiculo estacionado ainda!\033[0;37m\n");
        return;
    }

    for (int i = 0; i < numVeiculos; i++) {
        if (veiculo[i].id != 0) {
            printf("\033[0;33m== ID: %d ==\033[0;37m\n", veiculo[i].id);
            printf("Matricula: %s\nAndar: %d\nLugar: %d\nDia de entrada: %d/%d/%d\nHora de entrada: %d:%d\n", veiculo[i].matricula, veiculo[i].andar, veiculo[i].lugar, veiculo[i].diaEntrada, veiculo[i].mesEntrada, veiculo[i].anoEntrada, veiculo[i].horaEntrada, veiculo[i].minutoEntrada);
        }
    }

    printf("Pressiona ENTER para voltar...\n");  
    getchar();   
}

void retirarVeiculo(struct Veiculo *veiculo, int *numVeiculos) {
    int encontrado = 0, i;
    system("cls");
    if (*numVeiculos == 0) {
        printf("\033[0;31mNenhum veiculo estacionado para retirar.\033[0;37m\n");
        return;
    }

    char busca[10];
    int diaSaida, horaSaida;
    printf("Insira a matricula ou ID do veiculo a ser retirado: ");
    scanf(" %9[^\n]", busca);


    for (i = 0; i < *numVeiculos; i++) {
        if (strcmp(veiculo[i].matricula, busca) == 0 || ( veiculo[i].id == atoi(busca) && &numVeiculos != 0)) {
            pagamento(veiculo);
            // Marcar cliente como removido, mantendo ID 0
            veiculo[i].id = 0;
            encontrado = 1;
            printf("\033[0;32mCliente removido com sucesso.\033[0;37m\n");
            *numVeiculos--;
            return;
        }
    }

    if (!encontrado) {
        printf("\033[0;31mVeiculo nao estacionado.\033[0;37m\n");
    }
}

void pagamento(struct Veiculo *veiculo) {
    float valor = 0;

    if (veiculo->sub == 1) {
        printf("\033[0;33mParque gratuito para subscritor.\033[0;37m\n");  // Estacionamento gratuito para subscritores
        veiculo->sub = 0;
        return;
    }

    // Obter a data e hora atuais do sistema
    time_t now = time(NULL);
    struct tm *localtime_info = localtime(&now);

    // Calcular o tempo decorrido em horas
    int horasDecorridas = localtime_info->tm_hour - veiculo->horaEntrada;
    int diasDecorridos = localtime_info->tm_mday - veiculo->diaEntrada;

    // Se o tempo decorrido for menor que uma hora, não cobrar nada
    if (horasDecorridas == 0 && diasDecorridos == 0) {
        printf("\033[0;33mTOTAL: 0.00eur\n\033[0;37m");
        return;
    }


    // Se o tempo decorrido for mais de uma hora, cobrar mais 0.50 por hora adicional
    if (horasDecorridas >= 1 || diasDecorridos > 0) {
        if ((veiculo->horaEntrada >= 8 && veiculo->horaEntrada < 20)) {
            valor += 0.5 * horasDecorridas;
        } else if((veiculo->horaEntrada >= 21 || veiculo->horaEntrada < 7)){
            valor += 0.9 * horasDecorridas;
        }
    }

    // Se o tempo decorrido for mais de 24 horas, cobrar 5 por dia
    if (diasDecorridos >= 1) {
        valor += 5 * diasDecorridos;
    }

    printf("\033[0;33mTOTAL: %.2feur\n\033[0;37m", valor);
}


void salvarDados(struct Cliente *clientes, int numClientes, struct Veiculo *veiculo, int numVeiculos, int ultimoIdCliente, int ultimoIdVeiculo) {
    FILE *arquivoClientes = fopen("clientes.dat", "wb");
    FILE *arquivoVeiculos = fopen("veiculos.dat", "wb");

    if (arquivoClientes == NULL || arquivoVeiculos == NULL) {
        printf("\033[0;31mErro ao abrir o arquivo.\033[0;37m\n");
        return;
    }

    struct DadosArquivo dadosClientes = { ultimoIdCliente };
    fwrite(&dadosClientes, sizeof(struct DadosArquivo), 1, arquivoClientes);
    fwrite(&numClientes, sizeof(int), 1, arquivoClientes);
    for (int i = 0; i < numClientes; i++) {
        fwrite(&clientes[i], sizeof(struct Cliente), 1, arquivoClientes);
    }

    struct DadosArquivo dadosVeiculos = { ultimoIdVeiculo };
    fwrite(&dadosVeiculos, sizeof(struct DadosArquivo), 1, arquivoVeiculos);
    fwrite(&numVeiculos, sizeof(int), 1, arquivoVeiculos);
    for (int i = 0; i < numVeiculos; i++) {
        fwrite(&veiculo[i], sizeof(struct Veiculo), 1, arquivoVeiculos);
    }

    fclose(arquivoClientes);
    fclose(arquivoVeiculos);

    printf("\033[0;33mDados salvos com sucesso.\033[0;37m\n");
}

void carregarDados(struct Cliente *clientes, int *numClientes, struct Veiculo *veiculo, int *numVeiculos, int *ultimoIdCliente, int *ultimoIdVeiculo) {
    FILE *arquivoClientes = fopen("clientes.dat", "rb");
    FILE *arquivoVeiculos = fopen("veiculos.dat", "rb");

    if (arquivoClientes == NULL || arquivoVeiculos == NULL) {
        printf("\033[0;31mArquivo de dados não encontrado. Inicializando com dados vazios.\033[0;37m\n");
        return;
    }

    struct DadosArquivo dadosClientes;
    fread(&dadosClientes, sizeof(struct DadosArquivo), 1, arquivoClientes);
    *ultimoIdCliente = dadosClientes.ultimoId;

    fread(numClientes, sizeof(int), 1, arquivoClientes);
    for (int i = 0; i < *numClientes; i++) {
        fread(&clientes[i], sizeof(struct Cliente), 1, arquivoClientes);
    }

    struct DadosArquivo dadosVeiculos;
    fread(&dadosVeiculos, sizeof(struct DadosArquivo), 1, arquivoVeiculos);
    *ultimoIdVeiculo = dadosVeiculos.ultimoId;

    fread(numVeiculos, sizeof(int), 1, arquivoVeiculos);
    for (int i = 0; i < *numVeiculos; i++) {
        fread(&veiculo[i], sizeof(struct Veiculo), 1, arquivoVeiculos);
    }

    fclose(arquivoClientes);
    fclose(arquivoVeiculos);

    printf("\033[0;33mDados carregados com sucesso.\033[0;37m\n");
}

void limparDadosClientes() {
    FILE *arquivoClientes = fopen("clientes.dat", "wb");
    if (arquivoClientes == NULL) {
        printf("\033[0;31mErro ao abrir o arquivo de clientes.\033[0;37m\n");
        return;
    }

    int numClientes = 0;
    fwrite(&numClientes, sizeof(int), 1, arquivoClientes); // Sobrescreve o número de clientes como 0
    fclose(arquivoClientes);
    printf("\033[0;33mDados de clientes apagados com sucesso.\033[0;37m\n");
}

void limparDadosVeiculos() {
    FILE *arquivoVeiculos = fopen("veiculos.dat", "wb");
    if (arquivoVeiculos == NULL) {
        printf("\033[0;31mErro ao abrir o arquivo de veiculos.\033[0;37m\n");
        return;
    }
    int numVeiculos = 0;
    fwrite(&numVeiculos, sizeof(int), 1, arquivoVeiculos); // Sobrescreve o número de veículos como 0
    fclose(arquivoVeiculos);
    printf("\033[0;33mDados de veiculos apagados com sucesso.\033[0;37m\n");
}