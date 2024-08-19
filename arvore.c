#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SOBRENOMES 10
#define MAX_NOME 50
#define MAX_PESSOAS 100
#define MAX_CPF 12
#define HASH_SIZE 101  // Tamanho da tabela hash

typedef struct Pessoa {
    char nome[MAX_NOME];
    char sobrenomes[MAX_SOBRENOMES][MAX_NOME];
    int numSobrenomes;
    struct Pessoa *pai;
    struct Pessoa *mae;
    struct Pessoa *proximos;
    int generationIndex; // Índice da geração
    char cpf[MAX_CPF]; // CPF único para cada pessoa
    struct Pessoa* prox; // Ponteiro para resolver colisões na tabela hash
} Pessoa;

Pessoa* hashTable[HASH_SIZE];

// Função hash para calcular o índice da tabela hash
unsigned int hash(const char* cpf) {
    unsigned int hashValue = 0;
    while (*cpf) {
        hashValue = (hashValue << 5) + *cpf++;
    }
    return hashValue % HASH_SIZE;
}

// Função para buscar uma pessoa por CPF na tabela hash
Pessoa* buscarPessoaPorCPF(const char* cpf) {
    unsigned int index = hash(cpf);
    Pessoa* pessoa = hashTable[index];
    while (pessoa != NULL) {
        if (strcmp(pessoa->cpf, cpf) == 0) {
            return pessoa;
        }
        pessoa = pessoa->prox;
    }
    return NULL;
}

// Função para inserir uma pessoa na tabela hash
void inserirPessoaNaHash(Pessoa* novaPessoa) {
    unsigned int index = hash(novaPessoa->cpf);
    novaPessoa->prox = hashTable[index];
    hashTable[index] = novaPessoa;
}

// Função para criar uma nova pessoa
void criarPessoa(const char* nome, const char* cpf, const char* cpfPai, const char* cpfMae) {
    Pessoa* novaPessoa = (Pessoa*)malloc(sizeof(Pessoa));
    strcpy(novaPessoa->nome, nome);
    strcpy(novaPessoa->cpf, cpf);
    novaPessoa->numSobrenomes = 0;
    novaPessoa->pai = buscarPessoaPorCPF(cpfPai);
    novaPessoa->mae = buscarPessoaPorCPF(cpfMae);
    novaPessoa->prox = NULL;

    // Atualizar generationIndex
    int paiGen = (novaPessoa->pai != NULL) ? novaPessoa->pai->generationIndex : -1;
    int maeGen = (novaPessoa->mae != NULL) ? novaPessoa->mae->generationIndex : -1;
    novaPessoa->generationIndex = (paiGen > maeGen ? paiGen : maeGen) + 1;

    inserirPessoaNaHash(novaPessoa); // Inserir a nova pessoa na tabela hash
    printf("Pessoa %s com CPF %s criada com sucesso!\n", nome, cpf);
}

// Função para adicionar um sobrenome a uma pessoa
void adicionarSobrenome(Pessoa *pessoa, const char *sobrenome) {
    if (pessoa->numSobrenomes < MAX_SOBRENOMES) {
        strcpy(pessoa->sobrenomes[pessoa->numSobrenomes], sobrenome);
        pessoa->numSobrenomes++;
        printf("Sobrenome '%s' adicionado à pessoa %s com sucesso!\n", sobrenome, pessoa->nome);
    } else {
        printf("Limite de sobrenomes atingido!\n");
    }
}

// Função para exibir uma pessoa e seus detalhes
void exibirPessoa(Pessoa *pessoa) {
    printf("Nome: %s\n", pessoa->nome);
    printf("CPF: %s\n", pessoa->cpf);
    printf("Sobrenomes: ");
    for (int i = 0; i < pessoa->numSobrenomes; i++) {
        printf("%s ", pessoa->sobrenomes[i]);
    }
    printf("\n");

    printf("Geração: %d\n", pessoa->generationIndex);

    if (pessoa->pai != NULL) {
        printf("Pai: %s\n", pessoa->pai->nome);
    }

    if (pessoa->mae != NULL) {
        printf("Mãe: %s\n", pessoa->mae->nome);
    }

    printf("------------\n");
}

// Função para adicionar ou atualizar o pai de uma pessoa
void adicionarOuAtualizarPai(Pessoa* pessoa, const char* cpfPai) {
    Pessoa* pai = buscarPessoaPorCPF(cpfPai);
    if (pai != NULL) {
        pessoa->pai = pai;
        // Atualizar generationIndex
        int paiGen = (pai != NULL) ? pai->generationIndex : -1;
        int maeGen = (pessoa->mae != NULL) ? pessoa->mae->generationIndex : -1;
        pessoa->generationIndex = (paiGen > maeGen ? paiGen : maeGen) + 1;
        printf("Pai %s adicionado/atualizado com sucesso!\n", pai->nome);
    } else {
        printf("Pessoa com CPF %s não encontrada.\n", cpfPai);
    }
}

// Função para adicionar ou atualizar a mãe de uma pessoa
void adicionarOuAtualizarMae(Pessoa* pessoa, const char* cpfMae) {
    Pessoa* mae = buscarPessoaPorCPF(cpfMae);
    if (mae != NULL) {
        pessoa->mae = mae;
        // Atualizar generationIndex
        int paiGen = (pessoa->pai != NULL) ? pessoa->pai->generationIndex : -1;
        int maeGen = (mae != NULL) ? mae->generationIndex : -1;
        pessoa->generationIndex = (paiGen > maeGen ? paiGen : maeGen) + 1;
        printf("Mãe %s adicionado/atualizado com sucesso!\n", mae->nome);
    } else {
        printf("Pessoa com CPF %s não encontrada.\n", cpfMae);
    }
}

// Função para listar pessoas por sobrenome e geração específica
void listarPorSobrenomeEGeracao(Pessoa *pessoas[], int numPessoas, const char *sobrenome, int geracao) {
    printf("Pessoas com o sobrenome '%s' na %dª geração:\n", sobrenome, geracao);
    int encontrou = 0;

    for (int i = 0; i < numPessoas; i++) {
        Pessoa *pessoa = pessoas[i];
        for (int j = 0; j < pessoa->numSobrenomes; j++) {
            if (strcmp(pessoa->sobrenomes[j], sobrenome) == 0 &&
                pessoa->generationIndex == geracao) {
                printf("Nome: %s\n", pessoa->nome);
                encontrou = 1;
                break;
            }
        }
    }

    if (!encontrou) {
        printf("Nenhuma pessoa encontrada com o sobrenome '%s' na %dª geração.\n",
               sobrenome, geracao);
    }
    printf("------------\n");
}

// Função para listar pessoas por sobrenome
void listarPorSobrenome(Pessoa *pessoas[], int numPessoas,
                        const char *sobrenome) {
    printf("Pessoas com o sobrenome '%s':\n", sobrenome);
    int encontrou = 0;

    for (int i = 0; i < numPessoas; i++) {
        Pessoa *pessoa = pessoas[i];
        for (int j = 0; j < pessoa->numSobrenomes; j++) {
            if (strcmp(pessoa->sobrenomes[j], sobrenome) == 0) {
                printf("Nome: %s\n", pessoa->nome);
                encontrou = 1;
                break;
            }
        }
    }

    if (!encontrou) {
        printf("Nenhuma pessoa encontrada com o sobrenome '%s'.\n", sobrenome);
    }
    printf("------------\n");
}

// Função para listar pessoas por sobrenome e geração específica usando sobrenome
void listarPorSobrenomeEGeracaoUsandoSobrenome(const char* sobrenome) {
    for (int geracao = 0; geracao <= 10; geracao++) { // Arbitrário, ajustável
        listarPorSobrenomeEGeracao(NULL, 0, sobrenome, geracao); // NULL porque não precisamos do array de pessoas
    }
}

// Função para listar filhos de uma pessoa (por CPF)
void listarFilhos(const char* cpf) {
    Pessoa* pessoa = buscarPessoaPorCPF(cpf);
    if (pessoa == NULL) {
        printf("Pessoa com CPF %s não encontrada.\n", cpf);
        return;
    }

    printf("Filhos de %s (%s):\n", pessoa->nome, pessoa->cpf);
    int encontrou = 0;
    for (int i = 0; i < HASH_SIZE; i++) {
        Pessoa* p = hashTable[i];
        while (p != NULL) {
            if ((p->pai != NULL && strcmp(p->pai->cpf, cpf) == 0) ||
                (p->mae != NULL && strcmp(p->mae->cpf, cpf) == 0)) {
                printf("Nome: %s, CPF: %s\n", p->nome, p->cpf);
                encontrou = 1;
            }
            p = p->prox;
        }
    }

    if (!encontrou) {
        printf("Nenhum filho encontrado para %s.\n", pessoa->nome);
    }
    printf("------------\n");
}

// Função para listar todas as pessoas na tabela hash
void listarTodasAsPessoas() {
    printf("Listagem completa de todas as pessoas:\n");
    for (int i = 0; i < HASH_SIZE; i++) {
        Pessoa* pessoa = hashTable[i];
        while (pessoa != NULL) {
            exibirPessoa(pessoa);
            pessoa = pessoa->prox;
        }
    }
}

// Função para exibir o menu e receber escolhas do usuário
void menu() {
    int escolha;
    char nome[MAX_NOME];
    char cpf[MAX_CPF];
    char cpfPai[MAX_CPF];
    char cpfMae[MAX_CPF];
    char sobrenome[MAX_NOME];
    char cpfBusca[MAX_CPF];
    Pessoa *pessoa;
    int geracao;

    do {
        printf("\nMenu de Escolhas:\n");
        printf("1. Criar Pessoa\n");
        printf("2. Adicionar Sobrenome a Pessoa\n");
        printf("3. Adicionar/Atualizar Pai ou Mãe\n");
        printf("4. Listar Atributos por CPF\n");
        printf("5. Listar Pessoas por Sobrenome\n");
        printf("6. Listar Pessoas por Sobrenome e Geração\n");
        printf("7. Listar por Sobrenome e Geração (usando Sobrenome)\n");
        printf("8. Listar Filhos\n");
        printf("9. Listar Todas as Pessoas\n");
        printf("10. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &escolha);

        switch (escolha) {
            case 1:
                printf("Digite o nome da pessoa: ");
                scanf("%s", nome);
                printf("Digite o CPF (sem pontos ou traços): ");
                scanf("%s", cpf);
                printf("Digite o CPF do pai (ou '0' se não conhecido): ");
                scanf("%s", cpfPai);
                printf("Digite o CPF da mãe (ou '0' se não conhecido): ");
                scanf("%s", cpfMae);

                if (strcmp(cpfPai, "0") == 0) {
                    strcpy(cpfPai, "");
                }

                if (strcmp(cpfMae, "0") == 0) {
                    strcpy(cpfMae, "");
                }

                criarPessoa(nome, cpf, cpfPai, cpfMae);
                break;
            case 2:
                printf("Digite o CPF da pessoa para adicionar sobrenome: ");
                scanf("%s", cpfBusca);
                pessoa = buscarPessoaPorCPF(cpfBusca);
                if (pessoa != NULL) {
                    printf("Digite o sobrenome para adicionar: ");
                    scanf("%s", sobrenome);
                    adicionarSobrenome(pessoa, sobrenome);
                } else {
                    printf("Pessoa com CPF %s não encontrada.\n", cpfBusca);
                }
                break;
            case 3:
                printf("Digite o CPF da pessoa para atualizar pai/mãe: ");
                scanf("%s", cpfBusca);
                pessoa = buscarPessoaPorCPF(cpfBusca);
                if (pessoa != NULL) {
                    printf("1. Adicionar/Atualizar Pai\n");
                    printf("2. Adicionar/Atualizar Mãe\n");
                    int opcaoParente;
                    scanf("%d", &opcaoParente);
                    if (opcaoParente == 1) {
                        printf("Digite o CPF do pai: ");
                        scanf("%s", cpfPai);
                        adicionarOuAtualizarPai(pessoa, cpfPai);
                    } else if (opcaoParente == 2) {
                        printf("Digite o CPF da mãe: ");
                        scanf("%s", cpfMae);
                        adicionarOuAtualizarMae(pessoa, cpfMae);
                    } else {
                        printf("Opção inválida.\n");
                    }
                } else {
                    printf("Pessoa com CPF %s não encontrada.\n", cpfBusca);
                }
                break;
            case 4:
                printf("Digite o CPF da pessoa para busca: ");
                scanf("%s", cpfBusca);
                pessoa = buscarPessoaPorCPF(cpfBusca);
                if (pessoa != NULL) {
                    exibirPessoa(pessoa);
                } else {
                    printf("Pessoa com CPF %s não encontrada.\n", cpfBusca);
                }
                break;
            case 5:
                printf("Digite o sobrenome para listar: ");
                scanf("%s", sobrenome);
                listarPorSobrenome(NULL, 0, sobrenome);
                break;
            case 6:
                printf("Digite o sobrenome para listar: ");
                scanf("%s", sobrenome);
                printf("Digite a geração para listar: ");
                scanf("%d", &geracao);
                listarPorSobrenomeEGeracao(NULL, 0, sobrenome, geracao);
                break;
            case 7:
                printf("Digite o sobrenome para listar: ");
                scanf("%s", sobrenome);
                listarPorSobrenomeEGeracaoUsandoSobrenome(sobrenome);
                break;
            case 8:
                printf("Digite o CPF da pessoa para listar filhos: ");
                scanf("%s", cpfBusca);
                listarFilhos(cpfBusca);
                break;
            case 9:
                listarTodasAsPessoas();
                break;
            case 10:
                printf("Saindo...\n");
                break;
            default:
                printf("Opção inválida, tente novamente.\n");
        }
    } while (escolha != 10);
}

int main() {
    menu();
    return 0;
}
