#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <time.h>

// ============================================================================
//         PROJETO WAR ESTRUTURADO - NÍVEL AVENTUREIRO
// ============================================================================

// --- Constantes Globais ---
#define MAX_TERRITORIES 5        // Número fixo de territórios
#define MAX_NAME_LENGTH 50       // Tamanho máximo para nomes
#define MAX_COLOR_LENGTH 20      // Tamanho máximo para cor do exército
#define MIN_TROOPS_TO_ATTACK 2   // Mínimo de tropas para atacar
#define DICE_SIDES 6             // Lados do dado (1-6)

// --- Estrutura de Dados ---
typedef struct {
    char name[MAX_NAME_LENGTH];           // Nome do território
    char army_color[MAX_COLOR_LENGTH];    // Cor do exército dominante
    int troops;                           // Número de tropas no território
} Territory;

// --- Protótipos das Funções ---
// Funções de setup e gerenciamento de memória
Territory* allocate_map(void);
void free_memory(Territory* territories);

// Funções de interface com o usuário
void initialize_territories(Territory* territories);
void display_map(const Territory* territories);
void display_main_menu(void);

// Funções de lógica principal do jogo
void attack_phase(Territory* territories);
int simulate_attack(Territory* territories, int attacker_index, int defender_index);
int choose_territory(const char* action, const Territory* territories);

// Função utilitária
void clear_input_buffer(void);
int validate_troops(int troops);
int validate_territory_index(int index);

// --- Função Principal ---
int main() {
    // Configura o locale para português (acentos e caracteres especiais)
    setlocale(LC_ALL, "Portuguese");
    
    // Inicializa a semente para números aleatórios baseada no tempo atual
    srand((unsigned int)time(NULL));
    
    // Aloca memória dinamicamente para o mapa
    Territory* territories = allocate_map();
    if (territories == NULL) {
        printf("Erro: Falha na alocação de memória!\n");
        return 1;
    }
    
    printf("=== JOGO WAR - BATALHAS ESTRATÉGICAS ===\n\n");
    
    // Cadastra os territórios iniciais
    initialize_territories(territories);
    
    // Loop principal do jogo
    int choice;
    do {
        printf("\n=== MAPA ATUAL DOS TERRITÓRIOS ===\n");
        display_map(territories);
        
        display_main_menu();
        
        // Lê a escolha do jogador com validação
        printf("Digite sua escolha: ");
        if (scanf("%d", &choice) != 1) {
            printf("Erro: Digite apenas números!\n");
            clear_input_buffer();
            choice = -1; // Força repetição do loop
            continue;
        }
        clear_input_buffer();
        
        // Executa a ação baseada na escolha
        switch (choice) {
            case 1:
                attack_phase(territories);
                break;
            case 0:
                printf("Encerrando o jogo...\n");
                break;
            default:
                printf("Opção inválida! Tente novamente.\n");
                break;
        }
        
        // Pausa para o jogador ler os resultados
        if (choice != 0) {
            printf("\nPressione Enter para continuar...");
            getchar();
        }
        
    } while (choice != 0);
    
    // Libera a memória alocada
    free_memory(territories);
    
    printf("Obrigado por jogar!\n");
    return 0;
}

// --- Implementação das Funções ---

/**
 * Aloca dinamicamente a memória para o vetor de territórios
 * Retorna ponteiro para a memória alocada ou NULL em caso de falha
 */
Territory* allocate_map(void) {
    // Usa calloc para alocar e zerar a memória automaticamente
    Territory* territories = (Territory*)calloc(MAX_TERRITORIES, sizeof(Territory));
    
    if (territories == NULL) {
        printf("Erro: Não foi possível alocar memória para o mapa!\n");
        return NULL;
    }
    
    return territories;
}

/**
 * Libera a memória previamente alocada para o mapa
 */
void free_memory(Territory* territories) {
    if (territories != NULL) {
        free(territories);
        territories = NULL; // Boa prática para evitar uso após liberação
    }
}

/**
 * Função para cadastrar os dados de todos os territórios
 * Usa ponteiro para modificar o mapa original
 */
void initialize_territories(Territory* territories) {
    // Verifica se o ponteiro é válido
    if (territories == NULL) {
        printf("Erro: Ponteiro de territórios é inválido!\n");
        return;
    }
    
    // Loop para cadastrar todos os territórios
    for (int i = 0; i < MAX_TERRITORIES; i++) {
        printf("--- Cadastro do Território %d ---\n", i + 1);
        
        // Solicita e lê o nome do território
        printf("Digite o nome do território: ");
        clear_input_buffer();
        
        if (fgets(territories[i].name, MAX_NAME_LENGTH, stdin) != NULL) {
            // Remove a quebra de linha que o fgets pode capturar
            size_t len = strlen(territories[i].name);
            if (len > 0 && territories[i].name[len - 1] == '\n') {
                territories[i].name[len - 1] = '\0';
            }
        }
        
        // Solicita e lê a cor do exército
        printf("Digite a cor do exército dominante: ");
        
        if (fgets(territories[i].army_color, MAX_COLOR_LENGTH, stdin) != NULL) {
            // Remove a quebra de linha que o fgets pode capturar
            size_t len = strlen(territories[i].army_color);
            if (len > 0 && territories[i].army_color[len - 1] == '\n') {
                territories[i].army_color[len - 1] = '\0';
            }
        }
        
        // Solicita e lê o número de tropas com validação
        do {
            printf("Digite o número de tropas (mínimo 2 para poder atacar): ");
            
            if (scanf("%d", &territories[i].troops) != 1) {
                printf("Erro: Digite apenas números!\n");
                clear_input_buffer();
                territories[i].troops = 0;
            } else if (!validate_troops(territories[i].troops)) {
                printf("Erro: Número de tropas deve ser maior que 1!\n");
            }
            
        } while (!validate_troops(territories[i].troops));
        
        printf("Território cadastrado com sucesso!\n\n");
    }
}

/**
 * Exibe o menu principal com as opções disponíveis
 */
void display_main_menu(void) {
    printf("\n=== MENU PRINCIPAL ===\n");
    printf("1 - Iniciar Fase de Ataque\n");
    printf("0 - Sair do Jogo\n");
    printf("=======================\n");
}

/**
 * Função para exibir o mapa atual de todos os territórios
 * Usa const para garantir que apenas leia os dados
 */
void display_map(const Territory* territories) {
    // Verifica se o ponteiro é válido
    if (territories == NULL) {
        printf("Erro: Mapa não disponível!\n");
        return;
    }
    
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
 * Gerencia a fase de ataque do jogo
 * Permite ao jogador escolher territórios para atacar e defender
 */
void attack_phase(Territory* territories) {
    printf("\n=== FASE DE ATAQUE ===\n");
    
    // Escolhe o território atacante
    int attacker = choose_territory("ATACANTE", territories);
    if (attacker == -1) return; // Operação cancelada
    
    // Verifica se o atacante tem tropas suficientes
    if (territories[attacker].troops < MIN_TROOPS_TO_ATTACK) {
        printf("Erro: O território %s precisa de pelo menos %d tropas para atacar!\n", 
               territories[attacker].name, MIN_TROOPS_TO_ATTACK);
        return;
    }
    
    // Escolhe o território defensor
    int defender = choose_territory("DEFENSOR", territories);
    if (defender == -1) return; // Operação cancelada
    
    // Verifica se não está atacando a si mesmo
    if (attacker == defender) {
        printf("Erro: Um território não pode atacar a si mesmo!\n");
        return;
    }
    
    // Executa a simulação do ataque
    int conquest = simulate_attack(territories, attacker, defender);
    
    if (conquest) {
        printf("\n🎉 TERRITÓRIO CONQUISTADO! 🎉\n");
        printf("O território %s foi conquistado por %s!\n", 
               territories[defender].name, territories[attacker].army_color);
    }
}

/**
 * Simula uma batalha entre dois territórios usando dados aleatórios
 * Retorna 1 se houve conquista, 0 caso contrário
 */
int simulate_attack(Territory* territories, int attacker_index, int defender_index) {
    // Verifica se os índices são válidos
    if (!validate_territory_index(attacker_index) || !validate_territory_index(defender_index)) {
        printf("Erro: Índices de território inválidos!\n");
        return 0;
    }
    
    printf("\n--- SIMULAÇÃO DE BATALHA ---\n");
    printf("Atacante: %s (%d tropas)\n", 
           territories[attacker_index].name, territories[attacker_index].troops);
    printf("Defensor: %s (%d tropas)\n", 
           territories[defender_index].name, territories[defender_index].troops);
    
    // Gera dados aleatórios para atacante e defensor
    int attack_dice = (rand() % DICE_SIDES) + 1;  // Dado de 1 a 6
    int defense_dice = (rand() % DICE_SIDES) + 1; // Dado de 1 a 6
    
    printf("\nResultado dos dados:\n");
    printf("🎲 Atacante rolou: %d\n", attack_dice);
    printf("🎲 Defensor rolou: %d\n", defense_dice);
    
    // Determina o vencedor da batalha (empates favorecem o atacante)
    if (attack_dice >= defense_dice) {
        printf("✅ Atacante venceu a batalha!\n");
        territories[defender_index].troops--; // Defensor perde 1 tropa
        
        printf("O defensor perdeu 1 tropa!\n");
        
        // Verifica se o território foi conquistado
        if (territories[defender_index].troops <= 0) {
            // Transfere o território para o atacante
            strcpy(territories[defender_index].army_color, territories[attacker_index].army_color);
            territories[defender_index].troops = 1; // Coloca 1 tropa do conquistador
            territories[attacker_index].troops--;   // Atacante perde 1 tropa (que foi para o território conquistado)
            return 1; // Houve conquista
        }
    } else {
        printf("❌ Defensor resistiu ao ataque!\n");
        territories[attacker_index].troops--; // Atacante perde 1 tropa
        printf("O atacante perdeu 1 tropa!\n");
    }
    
    return 0; // Não houve conquista
}

/**
 * Permite ao jogador escolher um território para uma ação específica
 * Retorna o índice do território escolhido ou -1 se cancelado
 */
int choose_territory(const char* action, const Territory* territories) {
    int choice;
    
    printf("\nEscolha o território %s:\n", action);
    display_map(territories);
    
    do {
        printf("Digite o número do território (1-%d) ou 0 para cancelar: ", MAX_TERRITORIES);
        
        if (scanf("%d", &choice) != 1) {
            printf("Erro: Digite apenas números!\n");
            clear_input_buffer();
            choice = -1;
            continue;
        }
        clear_input_buffer();
        
        if (choice == 0) {
            printf("Operação cancelada.\n");
            return -1;
        }
        
        if (choice < 1 || choice > MAX_TERRITORIES) {
            printf("Erro: Escolha um número entre 1 e %d!\n", MAX_TERRITORIES);
        }
        
    } while (choice < 1 || choice > MAX_TERRITORIES);
    
    return choice - 1; // Converte para índice do array (0-4)
}

/**
 * Função utilitária para limpar o buffer de entrada
 */
void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        // Loop vazio - apenas limpa o buffer
    }
}

/**
 * Valida se o número de tropas é válido para o jogo
 * Retorna 1 se válido, 0 se inválido
 */
int validate_troops(int troops) {
    return troops >= MIN_TROOPS_TO_ATTACK; // Mínimo de 2 tropas
}

/**
 * Valida se o índice do território está dentro dos limites válidos
 * Retorna 1 se válido, 0 se inválido
 */
int validate_territory_index(int index) {
    return index >= 0 && index < MAX_TERRITORIES;
}
