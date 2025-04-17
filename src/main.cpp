#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <array>

// Classe TicTacToe
class TicTacToe {
private:
    std::array<std::array<char, 3>, 3> board; // Tabuleiro do jogo
    std::mutex board_mutex; // Mutex para controle de acesso ao tabuleiro
    std::condition_variable turn_cv; // Variável de condição para alternância de turnos
    char current_player; // Jogador atual ('X' ou 'O')
    bool game_over; // Estado do jogo
    char winner; // Vencedor do jogo

public:
    TicTacToe() {
        // Inicializar o tabuleiro e as variáveis do jogo
        for(int i = 0; i < board.size(); i++){
            for(int j = 0; j < board[i].size(); j++){
                board[i][j] = ' ';
            }
        }

        current_player = 'X';
        game_over = false;
        winner = ' ';
    }

    void display_board() {
        // Exibir o tabuleiro no console
        for(int i = 0; i < board.size(); i++){
            for(int j = 0; j < board[i].size(); j++){
                std::cout << board[i][j];
                if(j<2) std::cout << " | ";
            }
            if(i<2) std::cout << std::endl << "---------" << std::endl;
        }
        std::cout << std::endl;
    }

    bool make_move(char player, int row, int col) {
        // Implementar a lógica para realizar uma jogada no tabuleiro
        // Utilizar mutex para controle de acesso
        // Utilizar variável de condição para alternância de turnos
        std::unique_lock<std::mutex> lock(board_mutex);

        while(player != current_player && not(game_over)){
            turn_cv.wait(lock);
        }


        if(game_over) return false;
        if(board[row][col] != ' ') return false;


        board[row][col] = player;
        display_board();
        std::this_thread::sleep_for(std::chrono::milliseconds(300)); //Para facilitar ver as jogadas

        if(check_win(player)){
            game_over = true;
            winner = player;
        }
        else if(check_draw()){
            game_over = true;
            winner = 'D';
        }
        else{
            if(player == 'X'){
                current_player = 'O';
            }
            else{
                current_player = 'X';
            }
        } 
        turn_cv.notify_all();
        return true;
    }

    bool check_win(char player) {
        // Verificar se o jogador atual venceu o jogo
        for(int i=0; i<board.size(); i++){ //Linhas e colunas
            if((board[i][0] == player && board[i][1] == player && board[i][2] == player) ||
               (board[0][i] == player && board[1][i] == player && board[2][i] == player)){
                return true;
            }
        }

        if ((board[0][0] == player && board[1][1] == player && board[2][2] == player) ||
            (board[0][2] == player && board[1][1] == player && board[2][0] == player)){
                return true;
            }
        
        return false;
    }

    bool check_draw() {
        // Verificar se houve um empate
        for(int i = 0; i < board.size(); i++){
            for(int j = 0; j < board[i].size(); j++){
                if(board[i][j] == ' ') return false;
            }
        }
        return true; //Só checar draw depois de checar win, para economizar processamento
    }

    bool is_game_over() {
        // Retornar se o jogo terminou
        return game_over;
    }

    char get_winner() {
        // Retornar o vencedor do jogo ('X', 'O', ou 'D' para empate)
        return winner;
    }
};

// Classe Player
class Player {
private:
    TicTacToe& game; // Referência para o jogo
    char symbol; // Símbolo do jogador ('X' ou 'O')
    std::string strategy; // Estratégia do jogador

public:
    Player(TicTacToe& g, char s, std::string strat) 
        : game(g), symbol(s), strategy(strat) {}

    void play() {
        // Executar jogadas de acordo com a estratégia escolhida
        if(strategy == "sequential"){
            play_sequential();
        }
        else if (strategy == "random"){
            play_random();
        }   
        else{
            std::cout << "Strategy chosen is not a valid option." << std::endl;
        }
    }


private:
    void play_sequential() {
        // Implementar a estratégia sequencial de jogadas
        for(int i=0; i<3; i++){
            for(int j=0; j<3; j++){
                game.make_move(symbol, i, j);
            }
        }
    }

    void play_random() {
        // Implementar a estratégia aleatória de jogadas
        while(not(game.is_game_over())){
            int rand_row = rand() % 3;
            int rand_col = rand() % 3;
            game.make_move(symbol, rand_row, rand_col);
        }
    }
};

// Função principal
int main() {
    // Inicializar o jogo e os jogadores
    TicTacToe game;

    // Sequential or Random
    Player player1(game, 'X', "random");
    Player player2(game, 'O', "random");

    // Criar as threads para os jogadores
    std::thread thread1(&Player::play, &player1);
    std::thread thread2(&Player::play, &player2);

    // Aguardar o término das threads
    thread1.join();
    thread2.join();

    // Exibir o resultado final do jogo
    if(game.get_winner() == 'D'){
        std::cout << "Jogo terminou empatado." << std::endl;
    }
    else{
        std::cout << "Winner: " << game.get_winner() << std::endl;
    }

    return 0;
}