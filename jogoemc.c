#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define LINHAS_POR_PERGUNTA 8 // Pergunta + 4x Alternativas + Resposta + Linha em branco
#define MAX_LINHA 256         // Tamanho máximo de uma linha da pergunta

/*
 ====================================================================
   SUA FUNÇÃO 'lerArquivo' (DO SEGUNDO CÓDIGO)
   - Agora lendo "perguntas.txt"
 ====================================================================
*/
void lerArquivo(const char *nomeArquivo) {
    FILE *arquivo;
    char caractere;

    arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        printf("Erro: Não foi possível abrir o arquivo %s\n", nomeArquivo);
        return;
    }

    printf("\n--- Imprimindo lentamente '%s' ---\n", nomeArquivo);
    while ((caractere = fgetc(arquivo)) != EOF) {
        putchar(caractere);
        fflush(stdout);     // Garante impressão imediata
    }
    printf("\n--- Fim da impressao ---\n");
    fclose(arquivo);
}

/*
 ====================================================================
   NOVO: Estrutura para guardar a pergunta
   Isso é muito mais fácil que lidar com JSON.
 ====================================================================
*/
typedef struct {
    char texto[MAX_LINHA];
    char alternativas[4][MAX_LINHA];
    int resposta_correta; // Guardamos como número
} Pergunta;


// Função auxiliar para remover o '\n' que o fgets() deixa no final
void trim_newline(char *str) {
    int len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

/*
 ====================================================================
   NOVO: A função principal de leitura (Substitui o cJSON)
   - Ela busca a pergunta 'numero_pergunta' no TXT
   - E preenche a struct 'p' com os dados
 ====================================================================
*/
int buscarPerguntaNoTxt(int numero_pergunta, Pergunta *p) {
    FILE *fp = fopen("perguntas.txt", "r");
    if (fp == NULL) {
        printf("ERRO: Nao foi possivel abrir o 'perguntas.txt'\n");
        return 1;
    }

    // 1. Pular para a pergunta correta
    // Ex: para pegar a pergunta 5 (índice 4), pulamos 4 * 7 = 28 linhas
    int linhas_para_pular = numero_pergunta * LINHAS_POR_PERGUNTA;
    char buffer_lixo[MAX_LINHA];
    
    for (int i = 0; i < linhas_para_pular; i++) {
        if (fgets(buffer_lixo, MAX_LINHA, fp) == NULL) {
            printf("ERRO: Arquivo terminou antes de achar a pergunta %d\n", numero_pergunta);
            fclose(fp);
            return 1; // Erro, arquivo inesperado
        }
    }

    // 2. Ler os dados da pergunta
    if (fgets(p->texto, MAX_LINHA, fp) == NULL) return 1;
    if (fgets(p->alternativas[0], MAX_LINHA, fp) == NULL) return 1;
    if (fgets(p->alternativas[1], MAX_LINHA, fp) == NULL) return 1;
    if (fgets(p->alternativas[2], MAX_LINHA, fp) == NULL) return 1;
    if (fgets(p->alternativas[3], MAX_LINHA, fp) == NULL) return 1;
    
    // 3. Ler a resposta (como texto) e converter para número
    char buffer_resposta[MAX_LINHA];
    if (fgets(buffer_resposta, MAX_LINHA, fp) == NULL) return 1;

    // 4. Limpar o '\n' do final de cada linha lida
    trim_newline(p->texto);
    trim_newline(p->alternativas[0]);
    trim_newline(p->alternativas[1]);
    trim_newline(p->alternativas[2]);
    trim_newline(p->alternativas[3]);
    
    p->resposta_correta = atoi(buffer_resposta); // atoi = "ascii to integer"

    fclose(fp);
    return 0; // Sucesso
}


/*
 ====================================================================
   SUA FUNÇÃO 'main' (DO PRIMEIRO CÓDIGO)
   - Agora usando a lógica do TXT
 ====================================================================
*/
int main() {
    srand(time(NULL));
    int errou = 0;
    int vitoria = 0;
    int n = 0; // Nível atual (0 a 4)

    while (errou == 0 && vitoria == 0) {
        char menu_choice;
        int numero_pergunta = 0;
        switch (n) {
            case 0: numero_pergunta = (rand() % 5); break;
            case 1: numero_pergunta = (rand() % 5) + 5; break;
            case 2: numero_pergunta = (rand() % 5) + 10; break;
            case 3: numero_pergunta = (rand() % 5) + 15; break;
            case 4: numero_pergunta = (rand() % 5) + 20; break;
        }

        /*
         -----------------------------------------------------------------
         ALTERAÇÃO: Aqui chamamos nossa nova função
         Em vez de cJSON, usamos buscarPerguntaNoTxt
         -----------------------------------------------------------------
        */
        Pergunta p; // Cria uma struct para guardar os dados
        if (buscarPerguntaNoTxt(numero_pergunta, &p) != 0) {
            printf("Erro ao ler a pergunta %d. Saindo.\n", numero_pergunta);
            return 1; // Sai do jogo se deu erro
        }
        
        // Imprime a pergunta lida da struct
        printf("\nNivel %d - Pergunta %d:\n", n + 1, numero_pergunta + 1);
        printf("%s\n", p.texto); 
        printf("-----------------------------------\n");
        printf("1) %s\n", p.alternativas[0]);
        printf("2) %s\n", p.alternativas[1]);
        printf("3) %s\n", p.alternativas[2]);
        printf("4) %s\n", p.alternativas[3]);
        
        
        // Imprime o menu de AÇÃO
        printf("\n1) Responder pergunta\n");
        printf("2) Sair do jogo\n");
        printf("3) Utilizar acao especial (Nao implementado)\n");
        printf("Escolha uma opcao: ");
        
        scanf(" %c", &menu_choice);
        
        switch (menu_choice) {
            case '1': { 
                char resposta_usuario_char;
                printf("Digite sua resposta (1-4): ");
                scanf(" %c", &resposta_usuario_char);

                // Converte o char '1' para o int 1
                int resposta_usuario_int = resposta_usuario_char - '0';

                // Compara a resposta com a struct
                if (p.resposta_correta == resposta_usuario_int) {
                    printf("\nCERTA RESPOSTA!\n");
                    n++;
                    if (n == 5) {
                        printf("\nParabens, VC ganhou!!!!\n");
                        vitoria = 1;
                    }
                } else {
                    printf("\nResposta errada! A resposta correta era %d. Fim de jogo.\n", p.resposta_correta);
                    errou = 1;
                }
                break;
            }
            case '2':
                printf("Saindo do jogo...\n");
                errou = 1;
                break;
            case '3':
                printf("Acao especial ainda nao implementada.\n");
                break;
            default:
                printf("Opcao invalida, tente novamente.\n");
                break;
        }
    }
    
    return 0;
}