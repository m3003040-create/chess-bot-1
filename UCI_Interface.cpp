#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iterator>

class UCI {
public:
    Position pos;
    Searcher searcher;

    UCI() : searcher(pos) {}

    void loop() {
        std::string line, token;
        while (std::getline(std::cin, line)) {
            std::istringstream is(line);
            token.clear();
            is >> token;

            if (token == "uci") {
                std::cout << "id name ApexChess_v1" << std::endl;
                std::cout << "id author Gemini_User" << std::endl;
                std::cout << "uciok" << std::endl;
            } else if (token == "isready") {
                std::cout << "readyok" << std::endl;
            } else if (token == "ucinewgame") {
                reset_board(pos.board);
                init_all_tables();
            } else if (token == "position") {
                parse_position(is);
            } else if (token == "go") {
                parse_go(is);
            } else if (token == "quit") {
                break;
            } else if (token == "stop") {
                // Логика прерывания поиска
            }
        }
    }

private:
    void parse_position(std::istringstream& is) {
        std::string token, fen;
        is >> token;
        if (token == "startpos") {
            reset_board(pos.board);
            // Здесь должна быть инициализация начальной расстановки
            is >> token; 
        } else if (token == "fen") {
            while (is >> token && token != "moves") {
                fen += token + " ";
            }
            // Функция загрузки FEN (будет в следующем файле)
        }

        if (token == "moves") {
            std::string move_str;
            while (is >> move_str) {
                Move m = parse_move(move_str);
                if (m.data != 0) pos.make_move(m);
            }
        }
    }

    void parse_go(std::istringstream& is) {
        int depth = 6; // Значение по умолчанию
        std::string token;
        while (is >> token) {
            if (token == "depth") is >> depth;
        }
        searcher.iterative_deepening(depth);
    }

    Move parse_move(std::string str) {
        MoveList ml;
        generate_all_moves(pos.board, ml);
        int from = (str[0] - 'a') + (8 - (str[1] - '0')) * 8;
        int to = (str[2] - 'a') + (8 - (str[3] - '0')) * 8;
        
        for (int i = 0; i < ml.count; i++) {
            if (ml.moves[i].from() == from && ml.moves[i].to() == to) {
                return ml.moves[i];
            }
        }
        return Move();
    }
};

int main() {
    init_all_tables();
    UCI interface;
    interface.loop();
    return 0;
}
