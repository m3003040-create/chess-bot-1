#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm>

const int INFINITY_SCORE = 30000;
const int MATE_SCORE = 29000;

struct TTEntry {
    uint64_t key;
    int depth;
    int score;
    int flags;
    Move bestMove;
};

TTEntry TranspositionTable[0x100000];

enum TTFlags { EXACT, LOWERBOUND, UPPERBOUND };

class Searcher {
public:
    Position& pos;
    int nodes;
    int selDepth;

    Searcher(Position& p) : pos(p), nodes(0), selDepth(0) {}

    int evaluate() {
        int score = 0;
        Bitboard whitePawns = pos.board.pieces[WHITE][PAWN];
        Bitboard blackPawns = pos.board.pieces[BLACK][PAWN];
        score += pop_count(whitePawns) * 100;
        score -= pop_count(blackPawns) * 100;
        score += pop_count(pos.board.pieces[WHITE][KNIGHT]) * 320;
        score -= pop_count(pos.board.pieces[BLACK][KNIGHT]) * 320;
        score += pop_count(pos.board.pieces[WHITE][BISHOP]) * 330;
        score -= pop_count(pos.board.pieces[BLACK][BISHOP]) * 330;
        score += pop_count(pos.board.pieces[WHITE][ROOK]) * 500;
        score -= pop_count(pos.board.pieces[BLACK][ROOK]) * 500;
        score += pop_count(pos.board.pieces[WHITE][QUEEN]) * 900;
        score -= pop_count(pos.board.pieces[BLACK][QUEEN]) * 900;
        
        return (pos.board.sideToMove == WHITE) ? score : -score;
    }

    int quiescence(int alpha, int beta) {
        nodes++;
        int stand_pat = evaluate();
        if (stand_pat >= beta) return beta;
        if (alpha < stand_pat) alpha = stand_pat;

        MoveList ml;
        generate_all_moves(pos.board, ml);

        for (int i = 0; i < ml.count; i++) {
            if (ml.moves[i].flags() != CAPTURE && ml.moves[i].flags() < 12) continue;
            
            if (!pos.make_move(ml.moves[i])) continue;
            int score = -quiescence(-beta, -alpha);
            pos.unmake_move(ml.moves[i]);

            if (score >= beta) return beta;
            if (score > alpha) alpha = score;
        }
        return alpha;
    }

    int negamax(int depth, int alpha, int beta) {
        nodes++;
        if (depth <= 0) return quiescence(alpha, beta);

        bool inCheck = pos.is_square_attacked(get_lsb(pos.board.pieces[pos.board.sideToMove][KING]), pos.board.sideToMove ^ 1);
        if (inCheck) depth++;

        MoveList ml;
        generate_all_moves(pos.board, ml);
        
        int legalMoves = 0;
        int bestScore = -INFINITY_SCORE;

        for (int i = 0; i < ml.count; i++) {
            if (!pos.make_move(ml.moves[i])) continue;
            legalMoves++;
            
            int score = -negamax(depth - 1, -beta, -alpha);
            pos.unmake_move(ml.moves[i]);

            if (score >= beta) return beta;
            if (score > alpha) {
                alpha = score;
                bestScore = score;
            }
        }

        if (legalMoves == 0) {
            if (inCheck) return -MATE_SCORE + (selDepth - depth);
            return 0;
        }

        return alpha;
    }

    void iterative_deepening(int maxDepth) {
        nodes = 0;
        for (int d = 1; d <= maxDepth; d++) {
            selDepth = d;
            int score = negamax(d, -INFINITY_SCORE, INFINITY_SCORE);
            std::cout << "info depth " << d << " nodes " << nodes << " score cp " << score << std::endl;
        }
    }
};

void start_search(Position& pos, int depth) {
    Searcher s(pos);
    s.iterative_deepening(depth);
}
