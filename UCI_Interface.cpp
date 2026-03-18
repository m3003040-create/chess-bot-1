#include <iostream>
#include <string>
#include <sstream>
#include <vector>

// Подключаем связи с другими частями движка
#include "Bitboard.cpp"
#include "Position.cpp"
#include "Search.cpp"
#include "MoveGen.cpp"

void uci_loop() {
    Position pos;
    Searcher searcher(pos); // Теперь он будет знать, что такое searcher
    reset_board(pos.board);
    
    std::string line, command;
    while (std::getline(std::cin, line)) {
        std::istringstream is(line);
        is >> command;
        if (command == "uci") {
            std::cout << "id name ApexChess" << std::endl;
            std::cout << "id author User" << std::endl;
            std::cout << "uciok" << std::endl;
        } else if (command == "isready") {
            std::cout << "readyok" << std::endl;
        } else if (command == "go") {
            searcher.iterative_deepening(6); // Запуск поиска
        } else if (command == "quit") {
            break;
        }
    }
}

// Если в этом файле есть main, убедись, что таблицы инициализируются
int main() {
    init_all_tables(); // Теперь эта функция будет видна из Bitboard.cpp
    uci_loop();
    return 0;
}
