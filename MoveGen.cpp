#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm>

struct Move {
    uint16_t data;
    Move() : data(0) {}
    Move(int from, int to, int flags) {
        data = (from & 0x3F) | ((to & 0x3F) << 6) | ((flags & 0xF) << 12);
    }
    int from() const { return data & 0x3F; }
    int to() const { return (data >> 6) & 0x3F; }
    int flags() const { return (data >> 12) & 0xF; }
};

enum MoveFlags {
    QUIET = 0,
    DOUBLE_PAWN_PUSH = 1,
    KING_CASTLE = 2,
    QUEEN_CASTLE = 3,
    CAPTURE = 4,
    EN_PASSANT = 5,
    PROMOTION_KNIGHT = 8,
    PROMOTION_BISHOP = 9,
    PROMOTION_ROOK = 10,
    PROMOTION_QUEEN = 11,
    PROMOTION_KNIGHT_CAPTURE = 12,
    PROMOTION_BISHOP_CAPTURE = 13,
    PROMOTION_ROOK_CAPTURE = 14,
    PROMOTION_QUEEN_CAPTURE = 15
};

class MoveList {
public:
    Move moves[256];
    int count;
    MoveList() : count(0) {}
    void add(Move m) {
        moves[count++] = m;
    }
};

Bitboard get_rook_attacks_on_the_fly(int square, Bitboard occupancy) {
    Bitboard attacks = 0ULL;
    int r, f;
    int tr = square / 8;
    int tf = square % 8;
    for (r = tr + 1; r <= 7; r++) {
        attacks |= (1ULL << (r * 8 + tf));
        if (occupancy & (1ULL << (r * 8 + tf))) break;
    }
    for (r = tr - 1; r >= 0; r--) {
        attacks |= (1ULL << (r * 8 + tf));
        if (occupancy & (1ULL << (r * 8 + tf))) break;
    }
    for (f = tf + 1; f <= 7; f++) {
        attacks |= (1ULL << (tr * 8 + f));
        if (occupancy & (1ULL << (tr * 8 + f))) break;
    }
    for (f = tf - 1; f >= 0; f--) {
        attacks |= (1ULL << (tr * 8 + f));
        if (occupancy & (1ULL << (tr * 8 + f))) break;
    }
    return attacks;
}

Bitboard get_bishop_attacks_on_the_fly(int square, Bitboard occupancy) {
    Bitboard attacks = 0ULL;
    int r, f;
    int tr = square / 8;
    int tf = square % 8;
    for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++) {
        attacks |= (1ULL << (r * 8 + f));
        if (occupancy & (1ULL << (r * 8 + f))) break;
    }
    for (r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--) {
        attacks |= (1ULL << (r * 8 + f));
        if (occupancy & (1ULL << (r * 8 + f))) break;
    }
    for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++) {
        attacks |= (1ULL << (r * 8 + f));
        if (occupancy & (1ULL << (r * 8 + f))) break;
    }
    for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--) {
        attacks |= (1ULL << (r * 8 + f));
        if (occupancy & (1ULL << (r * 8 + f))) break;
    }
    return attacks;
}

void generate_pawn_moves(const BoardState &board, Color side, MoveList &ml) {
    Bitboard pawns = board.pieces[side][PAWN];
    Bitboard enemies = board.combined[side ^ 1];
    Bitboard empty = ~board.all;
    while (pawns) {
        int from = get_lsb(pawns);
        clear_bit(pawns, from);
        if (side == WHITE) {
            int to = from + 8;
            if (to <= SQ_H8 && !(board.all & (1ULL << to))) {
                if (from >= SQ_A7 && from <= SQ_H7) {
                    ml.add(Move(from, to, PROMOTION_QUEEN));
                    ml.add(Move(from, to, PROMOTION_ROOK));
                    ml.add(Move(from, to, PROMOTION_BISHOP));
                    ml.add(Move(from, to, PROMOTION_KNIGHT));
                } else {
                    ml.add(Move(from, to, QUIET));
                    if (from >= SQ_A2 && from <= SQ_H2 && !(board.all & (1ULL << (from + 16)))) {
                        ml.add(Move(from, from + 16, DOUBLE_PAWN_PUSH));
                    }
                }
            }
            Bitboard attacks = PawnAttacks[WHITE][from] & enemies;
            while (attacks) {
                int target = get_lsb(attacks);
                clear_bit(attacks, target);
                if (from >= SQ_A7 && from <= SQ_H7) {
                    ml.add(Move(from, target, PROMOTION_QUEEN_CAPTURE));
                    ml.add(Move(from, target, PROMOTION_ROOK_CAPTURE));
                    ml.add(Move(from, target, PROMOTION_BISHOP_CAPTURE));
                    ml.add(Move(from, target, PROMOTION_KNIGHT_CAPTURE));
                } else {
                    ml.add(Move(from, target, CAPTURE));
                }
            }
            if (board.enPassant != SQ_NONE) {
                Bitboard ep_attacks = PawnAttacks[WHITE][from] & (1ULL << board.enPassant);
                if (ep_attacks) ml.add(Move(from, board.enPassant, EN_PASSANT));
            }
        } else {
            int to = from - 8;
            if (to >= SQ_A1 && !(board.all & (1ULL << to))) {
                if (from >= SQ_A2 && from <= SQ_H2) {
                    ml.add(Move(from, to, PROMOTION_QUEEN));
                    ml.add(Move(from, to, PROMOTION_ROOK));
                    ml.add(Move(from, to, PROMOTION_BISHOP));
                    ml.add(Move(from, to, PROMOTION_KNIGHT));
                } else {
                    ml.add(Move(from, to, QUIET));
                    if (from >= SQ_A7 && from <= SQ_H7 && !(board.all & (1ULL << (from - 16)))) {
                        ml.add(Move(from, from - 16, DOUBLE_PAWN_PUSH));
                    }
                }
            }
            Bitboard attacks = PawnAttacks[BLACK][from] & enemies;
            while (attacks) {
                int target = get_lsb(attacks);
                clear_bit(attacks, target);
                if (from >= SQ_A2 && from <= SQ_H2) {
                    ml.add(Move(from, target, PROMOTION_QUEEN_CAPTURE));
                    ml.add(Move(from, target, PROMOTION_ROOK_CAPTURE));
                    ml.add(Move(from, target, PROMOTION_BISHOP_CAPTURE));
                    ml.add(Move(from, target, PROMOTION_KNIGHT_CAPTURE));
                } else {
                    ml.add(Move(from, target, CAPTURE));
                }
            }
            if (board.enPassant != SQ_NONE) {
                Bitboard ep_attacks = PawnAttacks[BLACK][from] & (1ULL << board.enPassant);
                if (ep_attacks) ml.add(Move(from, board.enPassant, EN_PASSANT));
            }
        }
    }
}

void generate_knight_moves(const BoardState &board, Color side, MoveList &ml) {
    Bitboard knights = board.pieces[side][KNIGHT];
    Bitboard own_pieces = board.combined[side];
    while (knights) {
        int from = get_lsb(knights);
        clear_bit(knights, from);
        Bitboard attacks = KnightAttacks[from] & ~own_pieces;
        while (attacks) {
            int target = get_lsb(attacks);
            clear_bit(attacks, target);
            if (board.all & (1ULL << target)) ml.add(Move(from, target, CAPTURE));
            else ml.add(Move(from, target, QUIET));
        }
    }
}

void generate_king_moves(const BoardState &board, Color side, MoveList &ml) {
    int from = get_lsb(board.pieces[side][KING]);
    Bitboard own_pieces = board.combined[side];
    Bitboard attacks = KingAttacks[from] & ~own_pieces;
    while (attacks) {
        int target = get_lsb(attacks);
        clear_bit(attacks, target);
        if (board.all & (1ULL << target)) ml.add(Move(from, target, CAPTURE));
        else ml.add(Move(from, target, QUIET));
    }
    if (side == WHITE) {
        if (board.castlingRights & 1) {
            if (!(board.all & (1ULL << SQ_F1)) && !(board.all & (1ULL << SQ_G1))) ml.add(Move(SQ_E1, SQ_G1, KING_CASTLE));
        }
        if (board.castlingRights & 2) {
            if (!(board.all & (1ULL << SQ_D1)) && !(board.all & (1ULL << SQ_C1)) && !(board.all & (1ULL << SQ_B1))) ml.add(Move(SQ_E1, SQ_C1, QUEEN_CASTLE));
        }
    } else {
        if (board.castlingRights & 4) {
            if (!(board.all & (1ULL << SQ_F8)) && !(board.all & (1ULL << SQ_G8))) ml.add(Move(SQ_E8, SQ_G8, KING_CASTLE));
        }
        if (board.castlingRights & 8) {
            if (!(board.all & (1ULL << SQ_D8)) && !(board.all & (1ULL << SQ_C8)) && !(board.all & (1ULL << SQ_B8))) ml.add(Move(SQ_E8, SQ_C8, QUEEN_CASTLE));
        }
    }
}

void generate_slider_moves(const BoardState &board, Color side, PieceType type, MoveList &ml) {
    Bitboard sliders = board.pieces[side][type];
    Bitboard own_pieces = board.combined[side];
    while (sliders) {
        int from = get_lsb(sliders);
        clear_bit(sliders, from);
        Bitboard attacks = (type == ROOK || type == QUEEN) ? get_rook_attacks_on_the_fly(from, board.all) : 0ULL;
        if (type == BISHOP || type == QUEEN) attacks |= get_bishop_attacks_on_the_fly(from, board.all);
        attacks &= ~own_pieces;
        while (attacks) {
            int target = get_lsb(attacks);
            clear_bit(attacks, target);
            if (board.all & (1ULL << target)) ml.add(Move(from, target, CAPTURE));
            else ml.add(Move(from, target, QUIET));
        }
    }
}

void generate_all_moves(const BoardState &board, MoveList &ml) {
    Color side = board.sideToMove;
    generate_pawn_moves(board, side, ml);
    generate_knight_moves(board, side, ml);
    generate_slider_moves(board, side, BISHOP, ml);
    generate_slider_moves(board, side, ROOK, ml);
    generate_slider_moves(board, side, QUEEN, ml);
    generate_king_moves(board, side, ml);
}
