#include <iostream>
#include <cstdint>
#include <algorithm>

const int PawnValue = 100;
const int KnightValue = 320;
const int BishopValue = 330;
const int RookValue = 500;
const int QueenValue = 900;

const int PawnTable[64] = {
      0,  0,  0,  0,  0,  0,  0,  0,
     50, 50, 50, 50, 50, 50, 50, 50,
     10, 10, 20, 30, 30, 20, 10, 10,
      5,  5, 10, 25, 25, 10,  5,  5,
      0,  0,  0, 20, 20,  0,  0,  0,
      5, -5,-10,  0,  0,-10, -5,  5,
      5, 10, 10,-20,-20, 10, 10,  5,
      0,  0,  0,  0,  0,  0,  0,  0
};

const int KnightTable[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50
};

const int BishopTable[64] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20
};

const int RookTable[64] = {
      0,  0,  0,  0,  0,  0,  0,  0,
      5, 10, 10, 10, 10, 10, 10,  5,
     -5,  0,  0,  0,  0,  0,  0, -5,
     -5,  0,  0,  0,  0,  0,  0, -5,
     -5,  0,  0,  0,  0,  0,  0, -5,
     -5,  0,  0,  0,  0,  0,  0, -5,
     -5,  0,  0,  0,  0,  0,  0, -5,
      0,  0,  0,  5,  5,  0,  0,  0
};

const int QueenTable[64] = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
     -5,  0,  5,  5,  5,  5,  0, -5,
      0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};

const int KingMiddleGameTable[64] = {
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
     20, 20,  0,  0,  0,  0, 20, 20,
     20, 30, 10,  0,  0, 10, 30, 20
};

const int Mirror64[64] = {
    56, 57, 58, 59, 60, 61, 62, 63,
    48, 49, 50, 51, 52, 53, 54, 55,
    40, 41, 42, 43, 44, 45, 46, 47,
    32, 33, 34, 35, 36, 37, 38, 39,
    24, 25, 26, 27, 28, 29, 30, 31,
    16, 17, 18, 19, 20, 21, 22, 23,
     8,  9, 10, 11, 12, 13, 14, 15,
     0,  1,  2,  3,  4,  5,  6,  7
};

class Evaluator {
public:
    static int evaluate_position(const BoardState& board) {
        int score = 0;
        Bitboard white_pawns = board.pieces[WHITE][PAWN];
        Bitboard black_pawns = board.pieces[BLACK][PAWN];
        
        while (white_pawns) {
            int sq = get_lsb(white_pawns);
            score += PawnValue + PawnTable[Mirror64[sq]];
            clear_bit(white_pawns, sq);
        }
        while (black_pawns) {
            int sq = get_lsb(black_pawns);
            score -= (PawnValue + PawnTable[sq]);
            clear_bit(black_pawns, sq);
        }

        Bitboard white_knights = board.pieces[WHITE][KNIGHT];
        Bitboard black_knights = board.pieces[BLACK][KNIGHT];
        while (white_knights) {
            int sq = get_lsb(white_knights);
            score += KnightValue + KnightTable[Mirror64[sq]];
            clear_bit(white_knights, sq);
        }
        while (black_knights) {
            int sq = get_lsb(black_knights);
            score -= (KnightValue + KnightTable[sq]);
            clear_bit(black_knights, sq);
        }

        Bitboard white_bishops = board.pieces[WHITE][BISHOP];
        Bitboard black_bishops = board.pieces[BLACK][BISHOP];
        while (white_bishops) {
            int sq = get_lsb(white_bishops);
            score += BishopValue + BishopTable[Mirror64[sq]];
            clear_bit(white_bishops, sq);
        }
        while (black_bishops) {
            int sq = get_lsb(black_bishops);
            score -= (BishopValue + BishopTable[sq]);
            clear_bit(black_bishops, sq);
        }

        Bitboard white_rooks = board.pieces[WHITE][ROOK];
        Bitboard black_rooks = board.pieces[BLACK][ROOK];
        while (white_rooks) {
            int sq = get_lsb(white_rooks);
            score += RookValue + RookTable[Mirror64[sq]];
            clear_bit(white_rooks, sq);
        }
        while (black_rooks) {
            int sq = get_lsb(black_rooks);
            score -= (RookValue + RookTable[sq]);
            clear_bit(black_rooks, sq);
        }

        Bitboard white_queens = board.pieces[WHITE][QUEEN];
        Bitboard black_queens = board.pieces[BLACK][QUEEN];
        while (white_queens) {
            int sq = get_lsb(white_queens);
            score += QueenValue + QueenTable[Mirror64[sq]];
            clear_bit(white_queens, sq);
        }
        while (black_queens) {
            int sq = get_lsb(black_queens);
            score -= (QueenValue + QueenTable[sq]);
            clear_bit(black_queens, sq);
        }

        int white_king_sq = get_lsb(board.pieces[WHITE][KING]);
        int black_king_sq = get_lsb(board.pieces[BLACK][KING]);
        score += KingMiddleGameTable[Mirror64[white_king_sq]];
        score -= KingMiddleGameTable[black_king_sq];

        return (board.sideToMove == WHITE) ? score : -score;
    }
};
