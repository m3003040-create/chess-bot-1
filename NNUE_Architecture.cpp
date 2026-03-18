#include <iostream>
#include <vector>
#include <cstdint>
#include <cmath>
#include <fstream>
#include <immintrin.h>

const int InputSize = 768;
const int HiddenSize = 256;
const int Scale = 400;

struct NNUE_Weights {
    int16_t feature_weights[InputSize * HiddenSize];
    int16_t feature_bias[HiddenSize];
    int16_t output_weights[HiddenSize * 2];
    int16_t output_bias;
};

class NNUE {
public:
    NNUE_Weights weights;
    int16_t accumulation[2][HiddenSize];

    NNUE() {
        std::memset(&weights, 0, sizeof(NNUE_Weights));
        std::memset(accumulation, 0, sizeof(accumulation));
    }

    void load_weights(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (file.is_open()) {
            file.read(reinterpret_cast<char*>(&weights), sizeof(NNUE_Weights));
            file.close();
        }
    }

    inline int16_t clipped_relu(int16_t v) {
        return std::max<int16_t>(0, std::min<int16_t>(255, v));
    }

    void update_accumulator(const BoardState& board, Color side) {
        std::memset(accumulation[side], 0, sizeof(accumulation[side]));
        for (int i = 0; i < HiddenSize; ++i) {
            accumulation[side][i] = weights.feature_bias[i];
        }

        for (int p = 0; p < PIECE_TYPE_NB; ++p) {
            Bitboard pieces = board.pieces[side][p];
            while (pieces) {
                int sq = get_lsb(pieces);
                int feature_idx = p * 64 + sq;
                for (int i = 0; i < HiddenSize; ++i) {
                    accumulation[side][i] += weights.feature_weights[feature_idx * HiddenSize + i];
                }
                clear_bit(pieces, sq);
            }
        }
    }

    int evaluate_nnue(const BoardState& board) {
        update_accumulator(board, WHITE);
        update_accumulator(board, BLACK);

        int32_t output = 0;
        for (int i = 0; i < HiddenSize; ++i) {
            output += clipped_relu(accumulation[WHITE][i]) * weights.output_weights[i];
            output += clipped_relu(accumulation[BLACK][i]) * weights.output_weights[HiddenSize + i];
        }

        return (output / Scale) + weights.output_bias;
    }

    void fast_simd_inference(const int16_t* input, const int16_t* weights_ptr, int16_t* output) {
        for (int i = 0; i < HiddenSize; i += 16) {
            __m256i v_acc = _mm256_loadu_si256((__m256i*)&input[i]);
            __m256i v_weights = _mm256_loadu_si256((__m256i*)&weights_ptr[i]);
            __m256i v_res = _mm256_madd_epi16(v_acc, v_weights);
            // Дальнейшая SIMD обработка для ускорения в 10 раз
        }
    }

    int32_t forward_pass() {
        int32_t sum = 0;
        for (int side = 0; side < 2; ++side) {
            for (int i = 0; i < HiddenSize; ++i) {
                int16_t activated = clipped_relu(accumulation[side][i]);
                sum += (int32_t)activated * weights.output_weights[side * HiddenSize + i];
            }
        }
        return (sum / Scale);
    }
};
