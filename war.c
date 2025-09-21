#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <time.h>

// ============================================================================
//         PROJETO WAR ESTRUTURADO - NÍVEL MESTRE
// ============================================================================

// --- Constantes Globais ---
#define MAX_TERRITORIES 5        // Número fixo de territórios
#define MAX_NAME_LENGTH 50       // Tamanho máximo para nomes
#define MAX_COLOR_LENGTH 20      // Tamanho máximo para cor do exército
#define MIN_TROOPS_TO_ATTACK 2   // Mínimo de tropas para atacar
#define DICE_SIDES 6             // Lados do dado (1-6)
#define TERRITORIES_TO_WIN 3     // Territórios necessários para vitória
#define TARGET_ARMY_COLOR "Verde" // Cor do exército alvo para destruição

// --- Tipos de Missões ---
typedef enum {
    MISSION_DESTROY_GREEN = 1,   // Destruir todos os territórios verdes
    MISSION_CONQUER_THREE = 2    // Conquistar 3 territórios
} MissionType;

// --- Estrutura de Dados ---
typedef struct {
    char name[MAX_NAME_LENGTH];           // Nome do território
    char army_color[MAX_COLOR_LENGTH];    // Cor do exército dominante
    int troops;                           // Número de tropas no território
} Territory;

// --- Protótipos das Funções ---
// Funções de setup e gerenciamento de memória
Territory* allocate_map(void);
void initialize_territories(Territory* territories);
void free_memory(Territory* territories);

// Funções de interface com o usuário
void display_map(const Territory* territories);
void display_main_menu(void);
void display_mission(MissionType current_mission);

// Funções de lógica principal do jogo
void attack_phase(Territory* territories);
int simulate_attack(Territory* territories, int attacker_index, int defender_index);
MissionType draw_mission(void);
int check_victory(const Territory* territories, MissionType mission, const char* player_color);

// Funções auxiliares
int choose_territory(const char* action, const Territory* territories);
void clear_input_buffer(void);
int validate_territory_index(int index);
int count_player_territories(const Territory* territories, const char* player_color);
int count_green_territories(const Territory* territories);

// --- Função Principal ---
int main() {
    // Configura o locale para português e inicializa números aleatórios
    setlocale(LC_ALL, "Portuguese");
    srand((unsigned int)time(NULL));
    
    // Aloca memória dinamicamente para o mapa
    Territory* territories = allocate_map();
    if (territories == NULL) {
        printf("Erro: Falha na alocação de memória!\n");
        return 1;
    }
    
    printf("=== JOGO WAR - MISSÕES ESTRATÉGICAS ===\n\n");
    
    // Inicializa territórios automaticamente
    initialize_territories(territories);
    
    // Sorteia uma missão para o jogador
    const char* player_color = "Azul"; // Cor fixa do jogador
    MissionType current_mission = draw_mission();
    
    printf("Sua cor de exército: %s\n", player_color);
    display_mission(current_mission);
    
    // Loop principal do jogo
    int choice;
    int game_won = 0;
    
    do {
        printf("\n=== MAPA ATUAL DOS TERRITÓRIOS ===\n");
        display_map(territories);
        
        display_main_menu();
        
        // Lê a escolha do jogador com validação
        printf("Digite sua escolha: ");
        if (scanf("%d", &choice) != 1) {
            printf("Erro: Digite apenas números!\n");
            clear_input_buffer();
            choice = -1;
            continue;
        }
        clear_input_buffer();
        
        // Executa a ação baseada na escolha
        switch (choice) {
            case 1:
                attack_phase(territories);
                break;
                
            case 2:
                // Verifica se a missão foi cumprida
                game_won = check_victory(territories, current_mission, player_color);
                if (game_won) {
                    printf("\n🎉 PARABÉNS! VOCÊ VENCEU O JOGO! 🎉\n");
                    printf("Missão cumprida com sucesso!\n");
                } else {
                    printf("\n📋 Status da Missão: AINDA NÃO CUMPRIDA\n");
                    display_mission(current_mission);
                }
                break;
                
            case 0:
                printf("Encerrando o jogo...\n");
                break;
                
            default:
                printf("Opção inválida! Tente novamente.\n");
                break;
        }
        
        // Pausa para o jogador ler os resultados
        if (choice != 0 && !game_won) {
            printf("\nPressione Enter para continuar...");
            getchar();
        }
        
    } while (choice != 0 && !game_won);
    
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
    Territory* territories = (Territory*)calloc(MAX_TERRITORIES, sizeof(Territory));
    
    if (territories == NULL) {
        printf("Erro: Não foi possível alocar memória para o mapa!\n");
        return NULL;
    }
    
    return territories;
}

/**
 * Inicializa automaticamente todos os territórios com dados pré-definidos
 * Cria um cenário balanceado para o jogo
 */
void initialize_territories(Territory* territories) {
    // Verifica se o ponteiro é válido
    if (territories == NULL) {
        printf("Erro: Ponteiro de territórios é inválido!\n");
        return;
    }
    
    // Dados pré-definidos dos territórios para inicialização automática
    const char* territory_names[MAX_TERRITORIES] = {
        "Brasil", "Argentina", "Peru", "Colombia", "Venezuela"
    };
    
    const char* army_colors[MAX_TERRITORIES] = {
        "Azul", "Verde", "Vermelho", "Verde", "Amarelo"
    };
    
    const int initial_troops[MAX_TERRITORIES] = {
        3, 4, 2, 3, 5
    };
    
    // Inicializa cada território com os dados pré-definidos
    for (int i = 0; i < MAX_TERRITORIES; i++) {
        // Copia o nome do território de forma segura
        strncpy(territories[i].name, territory_names[i], MAX_NAME_LENGTH - 1);
        territories[i].name[MAX_NAME_LENGTH - 1] = '\0'; // Garante terminação nula
        
        // Copia a cor do exército de forma segura
        strncpy(territories[i].army_color, army_colors[i], MAX_COLOR_LENGTH - 1);
        territories[i].army_color[MAX_COLOR_LENGTH - 1] = '\0'; // Garante terminação nula
        
        // Define o número inicial de tropas
        territories[i].troops = initial_troops[i];
    }
    
    printf("Territórios inicializados automaticamente!\n");
}

/**
 * Libera a memória previamente alocada para o mapa
 */
void free_memory(Territory* territories) {
    if (territories != NULL) {
        free(territories);
        territories = NULL;
    }
}

/**
 * Exibe o menu principal com todas as opções disponíveis
 */
void display_main_menu(void) {
    printf("\n=== MENU PRINCIPAL ===\n");
    printf("1 - Iniciar Fase de Ataque\n");
    printf("2 - Verificar Status da Missão\n");
    printf("0 - Sair do Jogo\n");
    printf("=======================\n");
}

/**
 * Exibe o mapa atual de todos os territórios em formato de tabela
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
 * Exibe a descrição da missão atual do jogador
 */
void display_mission(MissionType current_mission) {
    printf("\n=== SUA MISSÃO SECRETA ===\n");
    
    switch (current_mission) {
        case MISSION_DESTROY_GREEN:
            printf("🎯 Destruir todos os exércitos VERDES\n");
            printf("   Conquiste todos os territórios verdes para vencer!\n");
            break;
            
        case MISSION_CONQUER_THREE:
            printf("🎯 Conquistar %d territórios\n", TERRITORIES_TO_WIN);
            printf("   Domine pelo menos %d territórios com seu exército para vencer!\n", 
                   TERRITORIES_TO_WIN);
            break;
            
        default:
            printf("❌ Missão desconhecida!\n");
            break;
    }
    
    printf("==========================\n");
}

/**
 * Gerencia a fase de ataque com validações completas
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
 * Implementa todas as regras de combate do War
 * Retorna 1 se houve conquista, 0 caso contrário
 */
int simulate_attack(Territory* territories, int attacker_index, int defender_index) {
    // Verifica se os índices são válidos
    if (!validate_territory_index(attacker_index) || !validate_territory_index(defender_index)) {
        printf("Erro: Índices de território inválidos!\n");
        return 0;
    }
    
    printf("\n--- SIMULAÇÃO DE BATALHA ---\n");
    printf("Atacante: %s (%s - %d tropas)\n", 
           territories[attacker_index].name, 
           territories[attacker_index].army_color,
           territories[attacker_index].troops);
    printf("Defensor: %s (%s - %d tropas)\n", 
           territories[defender_index].name,
           territories[defender_index].army_color, 
           territories[defender_index].troops);
    
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
            strncpy(territories[defender_index].army_color, 
                   territories[attacker_index].army_color, 
                   MAX_COLOR_LENGTH - 1);
            territories[defender_index].army_color[MAX_COLOR_LENGTH - 1] = '\0';
            
            territories[defender_index].troops = 1; // Coloca 1 tropa do conquistador
            territories[attacker_index].troops--;   // Atacante perde 1 tropa
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
 * Sorteia uma missão aleatória para o jogador
 * Retorna o tipo da missão sorteada
 */
MissionType draw_mission(void) {
    // Gera número aleatório entre 1 e 2 para escolher a missão
    int mission_roll = (rand() % 2) + 1;
    
    printf("🎲 Sorteando sua missão secreta...\n");
    
    return (MissionType)mission_roll;
}

/**
 * Verifica se o jogador cumpriu os requisitos de sua missão atual
 * Implementa a lógica para cada tipo de missão
 * Retorna 1 se a missão foi cumprida, 0 caso contrário
 */
int check_victory(const Territory* territories, MissionType mission, const char* player_color) {
    // Verifica se os parâmetros são válidos
    if (territories == NULL || player_color == NULL) {
        printf("Erro: Parâmetros inválidos para verificação de vitória!\n");
        return 0;
    }
    
    switch (mission) {
        case MISSION_DESTROY_GREEN:
            // Missão: Destruir todos os exércitos verdes
            {
                int green_territories = count_green_territories(territories);
                printf("\nTerritórios verdes restantes: %d\n", green_territories);
                
                if (green_territories == 0) {
                    return 1; // Vitória! Não há mais territórios verdes
                }
                return 0; // Ainda há territórios verdes
            }
            
        case MISSION_CONQUER_THREE:
            // Missão: Conquistar 3 territórios
            {
                int player_territories = count_player_territories(territories, player_color);
                printf("\nTerritórios conquistados: %d/%d\n", player_territories, TERRITORIES_TO_WIN);
                
                if (player_territories >= TERRITORIES_TO_WIN) {
                    return 1; // Vitória! Conquistou 3 ou mais territórios
                }
                return 0; // Ainda não conquistou territórios suficientes
            }
            
        default:
            printf("Erro: Tipo de missão desconhecido!\n");
            return 0;
    }
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
 * Valida se o índice do território está dentro dos limites válidos
 */
int validate_territory_index(int index) {
    return index >= 0 && index < MAX_TERRITORIES;
}

/**
 * Conta quantos territórios pertencem ao jogador especificado
 * Retorna o número de territórios da cor especificada
 */
int count_player_territories(const Territory* territories, const char* player_color) {
    if (territories == NULL || player_color == NULL) {
        return 0;
    }
    
    int count = 0;
    
    // Percorre todos os territórios contando os do jogador
    for (int i = 0; i < MAX_TERRITORIES; i++) {
        if (strcmp(territories[i].army_color, player_color) == 0) {
            count++;
        }
    }
    
    return count;
}

/**
 * Conta quantos territórios verdes ainda existem no mapa
 * Retorna o número de territórios verdes restantes
 */
int count_green_territories(const Territory* territories) {
    if (territories == NULL) {
        return 0;
    }
    
    int count = 0;
    
    // Percorre todos os territórios contando os verdes
    for (int i = 0; i < MAX_TERRITORIES; i++) {
        if (strcmp(territories[i].army_color, TARGET_ARMY_COLOR) == 0) {
            count++;
        }
    }
    
    return count;
}
