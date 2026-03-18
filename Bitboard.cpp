#include <iostream>
#include <vector>
#include <cstdint>

typedef uint64_t Bitboard;

enum Square {
    SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1,
    SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2,
    SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3,
    SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4,
    SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5,
    SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6,
    SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7,
    SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8,
    SQ_NONE
};

enum Color {
    WHITE,
    BLACK,
    COLOR_NB = 2
};

enum PieceType {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
    PIECE_TYPE_NB = 6
};

Bitboard FileAMask = 0x0101010101010101ULL;
Bitboard FileBMask = 0x0202020202020202ULL;
Bitboard FileCMask = 0x0404040404040404ULL;
Bitboard FileDMask = 0x0808080808080808ULL;
Bitboard FileEMask = 0x1010101010101010ULL;
Bitboard FileFMask = 0x2020202020202020ULL;
Bitboard FileGMask = 0x4040404040404040ULL;
Bitboard FileHMask = 0x8080808080808080ULL;

Bitboard Rank1Mask = 0x00000000000000FFULL;
Bitboard Rank2Mask = 0x000000000000FF00ULL;
Bitboard Rank3Mask = 0x0000000000FF0000ULL;
Bitboard Rank4Mask = 0x00000000FF000000ULL;
Bitboard Rank5Mask = 0x000000FF00000000ULL;
Bitboard Rank6Mask = 0x0000FF0000000000ULL;
Bitboard Rank7Mask = 0x00FF000000000000ULL;
Bitboard Rank8Mask = 0xFF00000000000000ULL;

Bitboard KnightAttacks[64];
Bitboard KingAttacks[64];
Bitboard PawnAttacks[2][64];

void set_bit(Bitboard &bb, int square) {
    bb |= (1ULL << square);
}

void clear_bit(Bitboard &bb, int square) {
    bb &= ~(1ULL << square);
}

int get_lsb(Bitboard bb) {
    if (bb == 0) return -1;
    return __builtin_ctzll(bb);
}

int pop_count(Bitboard bb) {
    int count = 0;
    while (bb) {
        bb &= (bb - 1);
        count++;
    }
    return count;
}

void init_knight_attacks() {
    for (int s = 0; s < 64; ++s) {
        Bitboard b = 0;
        set_bit(b, s);
        Bitboard attacks = 0;
        if ((b << 17) & ~(FileAMask)) attacks |= (b << 17);
        if ((b << 15) & ~(FileHMask)) attacks |= (b << 15);
        if ((b << 10) & ~(FileAMask | FileBMask)) attacks |= (b << 10);
        if ((b << 6) & ~(FileGMask | FileHMask)) attacks |= (b << 6);
        if ((b >> 17) & ~(FileHMask)) attacks |= (b >> 17);
        if ((b >> 15) & ~(FileAMask)) attacks |= (b >> 15);
        if ((b >> 10) & ~(FileGMask | FileHMask)) attacks |= (b >> 10);
        if ((b >> 6) & ~(FileAMask | FileBMask)) attacks |= (b >> 6);
        KnightAttacks[s] = attacks;
    }
}

void init_king_attacks() {
    for (int s = 0; s < 64; ++s) {
        Bitboard b = 0;
        set_bit(b, s);
        Bitboard attacks = 0;
        if ((b << 8)) attacks |= (b << 8);
        if ((b >> 8)) attacks |= (b >> 8);
        if ((b << 1) & ~FileAMask) attacks |= (b << 1);
        if ((b >> 1) & ~FileHMask) attacks |= (b >> 1);
        if ((b << 7) & ~FileHMask) attacks |= (b << 7);
        if ((b << 9) & ~FileAMask) attacks |= (b << 9);
        if ((b >> 7) & ~FileAMask) attacks |= (b >> 7);
        if ((b >> 9) & ~FileHMask) attacks |= (b >> 9);
        KingAttacks[s] = attacks;
    }
}

void init_pawn_attacks() {
    for (int s = 0; s < 64; ++s) {
        Bitboard b = 0;
        set_bit(b, s);
        PawnAttacks[WHITE][s] = ((b << 7) & ~FileHMask) | ((b << 9) & ~FileAMask);
        PawnAttacks[BLACK][s] = ((b >> 7) & ~FileAMask) | ((b >> 9) & ~FileHMask);
    }
}

Bitboard mask_rook_attacks(int square) {
    Bitboard attacks = 0;
    int r, f;
    int tr = square / 8;
    int tf = square % 8;
    for (r = tr + 1; r <= 6; r++) set_bit(attacks, r * 8 + tf);
    for (r = tr - 1; r >= 1; r--) set_bit(attacks, r * 8 + tf);
    for (f = tf + 1; f <= 6; f++) set_bit(attacks, tr * 8 + f);
    for (f = tf - 1; f >= 1; f--) set_bit(attacks, tr * 8 + f);
    return attacks;
}

Bitboard mask_bishop_attacks(int square) {
    Bitboard attacks = 0;
    int r, f;
    int tr = square / 8;
    int tf = square % 8;
    for (r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++) set_bit(attacks, r * 8 + f);
    for (r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--) set_bit(attacks, r * 8 + f);
    for (r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++) set_bit(attacks, r * 8 + f);
    for (r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--) set_bit(attacks, r * 8 + f);
    return attacks;
}

void print_bitboard(Bitboard bb) {
    for (int r = 7; r >= 0; r--) {
        for (int f = 0; f < 8; f++) {
            int s = r * 8 + f;
            if ((bb >> s) & 1) std::cout << "1 ";
            else std::cout << ". ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

struct BoardState {
    Bitboard pieces[COLOR_NB][PIECE_TYPE_NB];
    Bitboard combined[COLOR_NB];
    Bitboard all;
    Color sideToMove;
    int castlingRights;
    Square enPassant;
};

void reset_board(BoardState &board) {
    for (int c = 0; c < COLOR_NB; ++c) {
        for (int p = 0; p < PIECE_TYPE_NB; ++p) {
            board.pieces[c][p] = 0ULL;
        }
        board.combined[c] = 0ULL;
    }
    board.all = 0ULL;
    board.sideToMove = WHITE;
    board.castlingRights = 0xF;
    board.enPassant = SQ_NONE;
}

void init_all_tables() {
    init_knight_attacks();
    init_king_attacks();
    init_pawn_attacks();
}

int main() {
    init_all_tables();
    BoardState mainBoard;
    reset_board(mainBoard);
    
    mainBoard.pieces[WHITE][PAWN] = Rank2Mask;
    mainBoard.pieces[BLACK][PAWN] = Rank7Mask;
    
    set_bit(mainBoard.pieces[WHITE][ROOK], SQ_A1);
    set_bit(mainBoard.pieces[WHITE][ROOK], SQ_H1);
    set_bit(mainBoard.pieces[BLACK][ROOK], SQ_A8);
    set_bit(mainBoard.pieces[BLACK][ROOK], SQ_H8);
    
    set_bit(mainBoard.pieces[WHITE][KNIGHT], SQ_B1);
    set_bit(mainBoard.pieces[WHITE][KNIGHT], SQ_G1);
    set_bit(mainBoard.pieces[BLACK][KNIGHT], SQ_B8);
    set_bit(mainBoard.pieces[BLACK][KNIGHT], SQ_G8);
    
    set_bit(mainBoard.pieces[WHITE][BISHOP], SQ_C1);
    set_bit(mainBoard.pieces[WHITE][BISHOP], SQ_F1);
    set_bit(mainBoard.pieces[BLACK][BISHOP], SQ_C8);
    set_bit(mainBoard.pieces[BLACK][BISHOP], SQ_F8);
    
    set_bit(mainBoard.pieces[WHITE][QUEEN], SQ_D1);
    set_bit(mainBoard.pieces[BLACK][QUEEN], SQ_D8);
    
    set_bit(mainBoard.pieces[WHITE][KING], SQ_E1);
    set_bit(mainBoard.pieces[BLACK][KING], SQ_E8);
    
    for (int p = 0; p < PIECE_TYPE_NB; ++p) {
        mainBoard.combined[WHITE] |= mainBoard.pieces[WHITE][p];
        mainBoard.combined[BLACK] |= mainBoard.pieces[BLACK][p];
    }
    mainBoard.all = mainBoard.combined[WHITE] | mainBoard.combined[BLACK];
    
    std::cout << "ApexChess Bitboard Initialized" << std::endl;
    print_bitboard(mainBoard.all);
    
    return 0;
}
