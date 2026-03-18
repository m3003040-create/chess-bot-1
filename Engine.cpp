#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <cstdint>

using namespace std;

// --- ТИПЫ ДАННЫХ И КОНСТАНТЫ ---
typedef uint64_t Bitboard;
enum Piece { EMPTY, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };
enum Color { WHITE, BLACK };

struct Move {
    int from;
    int to;
    int score;
};

// --- ГЛОБАЛЬНЫЕ ТАБЛИЦЫ ---
Bitboard knight_moves[64];
Bitboard king_moves[64];

void init_all_tables() {
    for (int i = 0; i < 64; i++) {
        Bitboard b = 1ULL << i;
        // Упрощенная генерация для примера (конь)
        knight_moves[i] = ((b << 17) & 0xFEFEFEFEFEFEFEFEULL) | ((b << 10) & 0xFCFCFCFCFCFCFCFCULL);
        king_moves[i] = (b << 1) | (b >> 1) | (b << 8) | (b >> 8);
    }
}

// --- КЛАСС ПОЗИЦИИ ---
class Position {
public:
    Bitboard pieces[2][7]; 
    Color side_to_move;

    Position() {
        for(int c=0; c<2; c++) for(int p=0; p<7; p++) pieces[c][p] = 0;
        side_to_move = WHITE;
        // Начальная расстановка (упрощенно)
        pieces[WHITE][PAWN] = 0x000000000000FF00ULL;
        pieces[BLACK][PAWN] = 0x00FF000000000000ULL;
    }

    int evaluate() {
        // Базовая оценка: считаем материал
        int score = 0;
        score += __builtin_popcountll(pieces[WHITE][PAWN]) * 100;
        score += __builtin_popcountll(pieces[WHITE][KNIGHT]) * 300;
        score -= __builtin_popcountll(pieces[BLACK][PAWN]) * 100;
        score -= __builtin_popcountll(pieces[BLACK][KNIGHT]) * 300;
        return (side_to_move == WHITE) ? score : -score;
    }
};

// --- КЛАСС ПОИСКА ---
class Searcher {
public:
    Position& pos;
    Searcher(Position& p) : pos(p) {}

    int alpha_beta(int alpha, int beta, int depth) {
        if (depth == 0) return pos.evaluate();
        
        // Здесь должен быть генератор ходов. Для теста вернем оценку.
        return pos.evaluate();
    }

    void iterative_deepening(int max_depth) {
        for (int d = 1; d <= max_depth; d++) {
            int score = alpha_beta(-10000, 10000, d);
            cout << "info depth " << d << " score cp " << score << endl;
        }
        cout << "bestmove e2e4" << endl; // Заглушка хода
    }
};

// --- ИНТЕРФЕЙС UCI ---
void uci_loop() {
    Position pos;
    Searcher searcher(pos);
    string line, cmd;

    while (getline(cin, line)) {
        istringstream is(line);
        is >> cmd;
        if (cmd == "uci") {
            cout << "id name ApexChess_v1" << endl;
            cout << "id author User" << endl;
            cout << "uciok" << endl;
        } else if (cmd == "isready") {
            cout << "readyok" << endl;
        } else if (cmd == "position") {
            // Логика обновления позиции
        } else if (cmd == "go") {
            searcher.iterative_deepening(6);
        } else if (cmd == "quit") {
            break;
        }
    }
}

int main() {
    init_all_tables();
    uci_loop();
    return 0;
}
