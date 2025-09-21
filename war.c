#include <stdio.h>
#include <string.h>
#include <locale.h>

// ============================================================================
//         PROJETO WAR ESTRUTURADO - NÍVEL NOVATO
// ============================================================================

// --- Constantes Globais ---
#define MAX_TERRITORIES 5        // Número fixo de territórios
#define MAX_NAME_LENGTH 50       // Tamanho máximo para nomes
#define MAX_COLOR_LENGTH 20      // Tamanho máximo para cor do exército

// --- Estrutura de Dados ---
typedef struct {
    char name[MAX_NAME_LENGTH];           // Nome do território
    char army_color[MAX_COLOR_LENGTH];    // Cor do exército dominante
    int troops;                           // Número de tropas no território
} Territory;

// --- Protótipos das Funções ---
void initialize_territories(Territory territories[]);
void display_map(const Territory territories[]);
void clear_input_buffer(void);
int validate_troops(int troops);

// --- Função Principal ---
int main() {
    // Configura o locale para português (acentos e caracteres especiais)
    setlocale(LC_ALL, "Portuguese");
    
    // Declara o vetor estático de territórios
    Territory territories[MAX_TERRITORIES];
    
    printf("=== JOGO WAR - CADASTRO DE TERRITÓRIOS ===\n\n");
    
    // Chama a função para cadastrar os territórios
    initialize_territories(territories);
    
    printf("\n=== MAPA ATUAL DOS TERRITÓRIOS ===\n");
    
    // Exibe o estado atual do mapa
    display_map(territories);
    
    printf("\nCadastro concluído com sucesso!\n");
    
    return 0;
}

// --- Implementação das Funções ---

/**
 * Função para cadastrar os dados de todos os territórios
 * Solicita ao usuário nome, cor do exército e número de tropas para cada território
 */
void initialize_territories(Territory territories[]) {
    // Loop para cadastrar todos os 5 territórios
    for (int i = 0; i < MAX_TERRITORIES; i++) {
        printf("--- Cadastro do Território %d ---\n", i + 1);
        
        // Solicita e lê o nome do território
        printf("Digite o nome do território: ");
        clear_input_buffer(); // Limpa o buffer antes de usar fgets
        
        // Usa fgets para ler string com espaços de forma segura
        if (fgets(territories[i].name, MAX_NAME_LENGTH, stdin) != NULL) {
            // Remove a quebra de linha que o fgets pode capturar
            size_t len = strlen(territories[i].name);
            if (len > 0 && territories[i].name[len - 1] == '\n') {
                territories[i].name[len - 1] = '\0';
            }
        }
        
        // Solicita e lê a cor do exército
        printf("Digite a cor do exército dominante: ");
        
        // Usa fgets para ler a cor de forma segura
        if (fgets(territories[i].army_color, MAX_COLOR_LENGTH, stdin) != NULL) {
            // Remove a quebra de linha que o fgets pode capturar
            size_t len = strlen(territories[i].army_color);
            if (len > 0 && territories[i].army_color[len - 1] == '\n') {
                territories[i].army_color[len - 1] = '\0';
            }
        }
        
        // Solicita e lê o número de tropas com validação
        do {
            printf("Digite o número de tropas (mínimo 1): ");
            
            // Verifica se a leitura do scanf foi bem-sucedida
            if (scanf("%d", &territories[i].troops) != 1) {
                printf("Erro: Digite apenas números!\n");
                clear_input_buffer(); // Limpa input inválido
                territories[i].troops = 0; // Força repetição do loop
            } else if (!validate_troops(territories[i].troops)) {
                printf("Erro: Número de tropas deve ser maior que 0!\n");
            }
            
        } while (!validate_troops(territories[i].troops));
        
        printf("Território cadastrado com sucesso!\n\n");
    }
}

/**
 * Função para exibir o mapa atual de todos os territórios
 * Mostra os dados em formato de tabela organizada
 */
void display_map(const Territory territories[]) {
    // Cabeçalho da tabela
    printf("\n+------+---------------------------+------------------+----------+\n");
    printf("| Nº   | Nome do Território        | Cor do Exército  | Tropas   |\n");
    printf("+------+---------------------------+------------------+----------+\n");
    
    // Loop para exibir cada território
    for (int i = 0; i < MAX_TERRITORIES; i++) {
        printf("| %-4d | %-25s | %-16s | %-8d |\n", 
               i + 1,                           // Número do território (1-5)
               territories[i].name,             // Nome do território
               territories[i].army_color,       // Cor do exército
               territories[i].troops);          // Número de tropas
    }
    
    // Rodapé da tabela
    printf("+------+---------------------------+------------------+----------+\n");
}

/**
 * Função utilitária para limpar o buffer de entrada
 * Evita problemas com leituras consecutivas de scanf e fgets
 */
void clear_input_buffer(void) {
    int c;
    // Lê e descarta caracteres até encontrar quebra de linha ou EOF
    while ((c = getchar()) != '\n' && c != EOF) {
        // Loop vazio - apenas limpa o buffer
    }
}

/**
 * Função para validar se o número de tropas é válido
 * Retorna 1 se válido (maior que 0), 0 se inválido
 */
int validate_troops(int troops) {
    // Valida se o número de tropas é positivo
    return troops > 0;
}
